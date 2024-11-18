#include "XUploadTask.h"

#include "XMsg.h"

#include <iostream>
#include <fstream>
#include <filesystem>

class XUploadTask::PImpl
{
public:
    PImpl(XUploadTask *owenr);
    ~PImpl() = default;

public:
    XUploadTask  *owenr_    = nullptr;
    std::string   filePath_ = "";
    std::ifstream ifs_;
    int           fileSize          = 0;
    char          readBuffer_[1024] = { 0 };
    std::ofstream ofs_;
};

XUploadTask::PImpl::PImpl(XUploadTask *owenr) : owenr_(owenr)
{
}


XUploadTask::XUploadTask()
{
    impl_ = std::make_shared<XUploadTask::PImpl>(this);
}

XUploadTask::~XUploadTask() = default;

void XUploadTask::writeCB()
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

bool XUploadTask::read(const XMsg *msg)
{
    switch (msg->type)
    {
        case MSG_UPLOAD_ACCEPT:
            beginWriteCB();
            break;
        case MSG_UPLOAD_COMPLETE:
            {
                /// 界面刷新
                std::cout << "upload file complete." << std::endl;
                if (uploadCB)
                    uploadCB();
                break;
            }
        default:
            break;
    }

    return true;
}

void XUploadTask::connectCB()
{
    if (impl_->filePath_.empty())
        return;

    /// 1. 打开本地文件
    impl_->ifs_.open(impl_->filePath_, std::ios::in | std::ios::binary | std::ios::ate);
    if (!impl_->ifs_.is_open())
    {
        std::cerr << "open file " << impl_->filePath_ << " failed!" << std::endl;
        return;
    }


    ///	2. 获取文件名称 文件大小 filname.zip,1024
    impl_->fileSize = impl_->ifs_.tellg();
    impl_->ifs_.seekg(0, std::ios::beg);
    std::cerr << "open file " << impl_->filePath_ << " success." << std::endl;
    std::string fileName = "";
    int         pos      = impl_->filePath_.find_last_of("/");
    if (pos < 0)
        pos = 0;
    fileName = impl_->filePath_.substr(pos, impl_->filePath_.size() - pos);

    std::string data = std::format("{},{};", fileName, impl_->fileSize);

    ///	3. 发送上传文件请求
    XMsg msg;
    msg.type = MSG_UPLOAD_INFO;
    msg.data = data.data();
    msg.size = data.size() + 1;
    write(&msg);
}

void XUploadTask::setFilePath(const std::string &path)
{
    impl_->filePath_ = path;
}
