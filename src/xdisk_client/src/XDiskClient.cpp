#include "XDiskClient.h"

#include "XComTask.h"
#include "XDirTask.h"
#include "XDownLoadTask.h"
#include "XThreadPool.h"
#include "XUploadTask.h"

#include <iostream>

class XDiskClient::PImpl
{
public:
    PImpl(XDiskClient *owenr);
    ~PImpl();

public:
    XDiskClient *owenr_      = nullptr;
    std::string  serverPath_ = "";
    std::string  serverIp_   = "";
    int          serverPort_ = -1;
};

XDiskClient::PImpl::PImpl(XDiskClient *owenr) : owenr_(owenr)
{
}

XDiskClient::PImpl::~PImpl() = default;


XDiskClient::XDiskClient()
{
    impl_ = std::make_shared<PImpl>(this);
}

XDiskClient::~XDiskClient() = default;

static void dirCB(std::string dirs)
{
    std::cout << dirs << std::endl;
    XDiskClient::get()->signalUpdateDir(dirs);
}

static void uploadCB()
{
    XDiskClient::get()->signalUploadComplete();
}

static void downloadCB()
{
    XDiskClient::get()->signalDownloadComplete();
}

void XDiskClient::getDir()
{
    std::cout << "getDir " << impl_->serverIp_ << ":" << impl_->serverPort_ << " /" << impl_->serverPath_ << std::endl;

    auto *task = new XDirTask();
    task->setServerIp(impl_->serverIp_);
    task->setServerPort(impl_->serverPort_);
    task->setServerRoot(impl_->serverPath_);
    task->dirCB = dirCB;
    XThreadPool::getInstance()->dispatch(task);
}

void XDiskClient::uploadFile(const std::string &filePath)
{
    auto *task = new XUploadTask();
    task->setServerIp(impl_->serverIp_);
    task->setServerPort(impl_->serverPort_);
    task->setFilePath(filePath);
    task->uploadCB = uploadCB;
    XThreadPool::getInstance()->dispatch(task);
}

void XDiskClient::downloadFile(const std::string &serverPath, const std::string &localPath)
{
    auto *task = new XDownLoadTask();
    task->setServerIp(impl_->serverIp_);
    task->setServerPort(impl_->serverPort_);
    task->setFilePath(serverPath);
    task->setLocalPath(localPath);
    task->downloadCB = downloadCB;
    XThreadPool::getInstance()->dispatch(task);
}

void XDiskClient::setServerIp(const std::string ip)
{
    impl_->serverIp_ = ip;
}

void XDiskClient::setServerPort(int port)
{
    impl_->serverPort_ = port;
}

void XDiskClient::setServerRoot(const std::string path)
{
    impl_->serverPath_ = path;
}

bool XDiskClient::init()
{
    std::cout << "XDiskClient::init" << std::endl;
    XThreadPool::getInstance()->init(10);

    return true;
}
