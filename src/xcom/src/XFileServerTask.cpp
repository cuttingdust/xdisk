#include "XFileServerTask.h"
#include "XMsg.h"
#include "XTools.h"

#include <iostream>
#include <fstream>
#include <mutex>

class XFileServerTask::PImpl
{
public:
    PImpl(XFileServerTask *owenr);
    ~PImpl() = default;

public:
    void getDir(const XMsg *msg);

    void uploadFile(const XMsg *msg);

    void downloadFile(const XMsg *msg);

    static std::string getCurDir();

    static void setCurDir(const std::string &dir);

public:
    XFileServerTask *owenr_ = nullptr;

    /// \brief 文件大小
    int fileSize_ = 0;

    std::string filePath_ = "";

    char readBuffer_[1024] = { 0 };

    /// \brief 客户端已上传文件大小
    int recvSize_ = 0;

    static std::string sCurDir;

    static std::mutex sMutex;

    std::ofstream ofs_;

    std::ifstream ifs_;
};

std::string XFileServerTask::PImpl::sCurDir = "./server_root";
std::mutex  XFileServerTask::PImpl::sMutex;

XFileServerTask::PImpl::PImpl(XFileServerTask *owenr) : owenr_(owenr)
{
}

void XFileServerTask::PImpl::getDir(const XMsg *msg)
{
    if (!msg->data)
        return;

    std::string root = msg->data;
    if (root.empty())
        root = "./";
    setCurDir(root);

    // std::string dir = "file1,1024;file2,4096;file3.zip,10240;";
    std::string dir = XTools::getDirData(msg->data);
    XMsg        resMsg;
    resMsg.type = MSG_DIRLIST;
    resMsg.size = dir.size() + 1;
    resMsg.data = (char *)dir.c_str();
    owenr_->write(&resMsg);
}

void XFileServerTask::PImpl::uploadFile(const XMsg *msg)
{
    /// 1.获取文件名称 文件大小 filname.zip,1024
    if (!msg || !msg->data || msg->size <= 0)
        return;
    const std::string data = msg->data;
    if (data.empty())
        return;
    int pos = data.find_first_of(",");
    if (pos <= 0)
        return;
    const std::string fileName = data.substr(0, pos);
    int               sizePos  = pos + 1;
    if (sizePos >= data.size())
        return;
    fileSize_ = std::stoi(data.substr(sizePos, data.size() - sizePos));
    if (fileSize_ <= 0)
        return;

    std::cout << "upload file: " << fileName << " size: " << fileSize_ << std::endl;

    /// 2. 打开本地文件
    const std::string filePath = getCurDir() + fileName;
    ofs_.open(filePath, std::ios::out | std::ios::binary);
    if (!ofs_.is_open())
    {
        std::cerr << "open file " << filePath << " failed!" << std::endl;
        return;
    }
    std::cout << "open file " << filePath << " success." << std::endl;

    /// 3. 回复accept
    XMsg resMsg;
    resMsg.type = MSG_UPLOAD_ACCEPT;
    resMsg.size = 3;
    resMsg.data = const_cast<char *>("OK");
    owenr_->write(&resMsg);
    owenr_->setIsRecvMsg(false);
    recvSize_ = 0;
}

void XFileServerTask::PImpl::downloadFile(const XMsg *msg)
{
    if (!msg || !msg->data || msg->size <= 0)
        return;

    /// 打开文件
    filePath_ = msg->data;
    if (filePath_.empty())
        return;

    /// 获取文件名
    ifs_.open(filePath_.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!ifs_.is_open())
    {
        std::cerr << "open file " << filePath_ << " failed!" << std::endl;
        return;
    }


    ///	2. 获取文件名称 文件大小 filname.zip,1024
    fileSize_ = ifs_.tellg();
    ifs_.seekg(0, std::ios::beg);
    std::cerr << "open file " << filePath_ << " success." << std::endl;
    char data[1024] = {};
    sprintf(data, "%d", fileSize_);


    /// 回复消息MSG_DOWNALOD_ACCEPT
    XMsg resMsg;
    resMsg.type = MSG_DOWNALOD_ACCEPT;
    resMsg.size = strlen(data) + 1;
    resMsg.data = data;
    owenr_->write(&resMsg);
}

std::string XFileServerTask::PImpl::getCurDir()
{
    sMutex.lock();
    std::string result = sCurDir;
    sMutex.unlock();

    return result;
}

void XFileServerTask::PImpl::setCurDir(const std::string &dir)
{
    sMutex.lock();
    sCurDir = dir;
    sMutex.unlock();
}


XFileServerTask::XFileServerTask()
{
    impl_ = std::make_shared<PImpl>(this);
}

XFileServerTask::~XFileServerTask() = default;

bool XFileServerTask::read(const XMsg *msg)
{
    switch (msg->type)
    {
        case MSG_GETDIR:
            {
                std::cout << "MSG_GETDIR" << std::endl;
                impl_->getDir(msg);
                break;
            }
        case MSG_UPLOAD_INFO:
            {
                std::cout << "MSG_UPLOAD_INFO" << std::endl;
                impl_->uploadFile(msg);
                break;
            }
        case MSG_DOWNLOAD_INFO:
            {
                std::cout << "MSG_DOWNLOAD_INFO" << std::endl;
                impl_->downloadFile(msg);
                break;
            }
        case MSG_DOWNLOAD_COMPLETE:
            {
                std::cout << "MSG_DOWNLOAD_COMPLETE" << std::endl;
                /// 清理网络资源
                close();
                return false;
                break;
            }
        default:
            break;
    }
    return true;
}

void XFileServerTask::read(void *data, int size)
{
    if (!data || size <= 0)
        return;
    if (!impl_->ofs_.is_open())
        return;
    impl_->ofs_.write(static_cast<char *>(data), size);
    impl_->recvSize_ += size;
    if (impl_->recvSize_ == impl_->fileSize_)
    {
        std::cout << "file write end." << std::endl;
        impl_->ofs_.close();

        XMsg resMsg;
        resMsg.type = MSG_UPLOAD_COMPLETE;
        resMsg.size = 3;
        resMsg.data = const_cast<char *>("OK");
        write(&resMsg);
    }
}

void XFileServerTask::writeCB()
{
    if (!impl_->ifs_.is_open())
        return;

    impl_->ifs_.read(impl_->readBuffer_, sizeof(impl_->readBuffer_));
    int readSize = impl_->ifs_.gcount();
    if (readSize <= 0)
    {
        impl_->ifs_.close();
        return;
    }
    write(impl_->readBuffer_, readSize);
    if (impl_->ifs_.eof())
    {
        impl_->ifs_.close();
    }
}
