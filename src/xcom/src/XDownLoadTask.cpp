#include "XDownLoadTask.h"

#include "XMsg.h"

#include <fstream>
#include <iostream>

class XDownLoadTask::PImpl
{
public:
    PImpl(XDownLoadTask *owenr);
    ~PImpl() = default;

public:
    XDownLoadTask *owenr_ = nullptr;

    /// \brief 文件大小
    int fileSize_ = 0;

    std::string filePath_ = "";

    std::string localPath_ = "";

    /// \brief 客户端已上传文件大小
    int recvSize_ = 0;

    std::ofstream ofs_;
};

XDownLoadTask::PImpl::PImpl(XDownLoadTask *owenr) : owenr_(owenr)
{
}


XDownLoadTask::XDownLoadTask()
{
    impl_ = std::make_shared<PImpl>(this);
}

XDownLoadTask::~XDownLoadTask() = default;

void XDownLoadTask::read(void *data, int size)
{
    if (!data || size <= 0)
        return;
    if (!impl_->ofs_.is_open())
        return;
    impl_->ofs_.write(static_cast<char *>(data), size);
    impl_->recvSize_ += size;
    if (impl_->recvSize_ == impl_->fileSize_)
    {
        if (downloadCB)
            downloadCB();

        std::cout << "file write end." << std::endl;
        impl_->ofs_.close();

        XMsg resMsg;
        resMsg.type = MSG_DOWNLOAD_COMPLETE;
        resMsg.size = 3;
        resMsg.data = const_cast<char *>("OK");
        write(&resMsg);
    }
}

bool XDownLoadTask::read(const XMsg *msg)
{
    switch (msg->type)
    {
        case MSG_DOWNALOD_ACCEPT:
            {
                /// 不接受消息， 开始接受文件
                setIsRecvMsg(false);
                if (msg->data)
                {
                    impl_->fileSize_ = atoi(msg->data);
                }
                else
                {
                    close();
                    return false;
                }
                break;
            }

        default:
            break;
    }

    return true;
}

void XDownLoadTask::connectCB()
{
    if (impl_->filePath_.empty())
    {
        close();
        return;
    }

    std::string data = impl_->filePath_;


    /// 获取文件名
    int pos = impl_->filePath_.find_last_of('/');
    if (pos < 0)
        pos = 0;
    const std::string fileName = impl_->filePath_.substr(pos, impl_->filePath_.size() - pos);
    const std::string path     = impl_->localPath_ + fileName;


    impl_->ofs_.open(path, std::ios::out | std::ios::binary);
    if (!impl_->ofs_.is_open())
    {
        std::cerr << "open file " << path << " failed!" << std::endl;
        return;
    }
    std::cout << "open file " << path << " success." << std::endl;

    XMsg msg;
    msg.type = MSG_DOWNLOAD_INFO;
    msg.size = data.size() + 1;
    msg.data = data.data();
    write(&msg);
}

void XDownLoadTask::setFilePath(const std::string &path)
{
    impl_->filePath_ = path;
}

void XDownLoadTask::setLocalPath(const std::string &path)
{
    impl_->localPath_ = path;
}
