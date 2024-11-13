#include "XComTask.h"

#include <iostream>

#include <event2/bufferevent.h>
#include <event2/event.h>

static void SReadCb(struct bufferevent *bev, void *ctx)
{
    std::cout << "SReadCb" << std::endl;
}

static void SWriteCb(struct bufferevent *bev, void *ctx)
{
    std::cout << "SWriteCb" << std::endl;
}

static void SEventCb(struct bufferevent *bev, short events, void *ctx)
{
    std::cout << "SEventCb: " << events << std::endl;
    if (events & BEV_EVENT_CONNECTED)
    {
        std::cout << "BEV_EVENT_CONNECTED" << std::endl;
    }
}

class XComTask::PImpl
{
public:
    PImpl(XComTask *owenr);
    ~PImpl();

public:
    XComTask           *owenr_      = nullptr;
    struct bufferevent *bev_        = nullptr;
    std::string         serverPath_ = "";
    std::string         serverIp_   = "";
    int                 serverPort_ = -1;
};

XComTask::PImpl::PImpl(XComTask *owenr) : owenr_(owenr)
{
}

XComTask::PImpl::~PImpl() = default;

XComTask::XComTask()
{
    impl_ = std::make_shared<PImpl>(this);
}

XComTask::~XComTask() = default;

auto XComTask::init() -> bool
{
    /// 用bufferevent建立连接
    impl_->bev_ = bufferevent_socket_new(base(), -1, BEV_OPT_CLOSE_ON_FREE);
    if (!impl_->bev_)
    {
        std::cerr << "bufferevent_socket_new failed" << std::endl;
        return false;
    }

    bufferevent_setcb(impl_->bev_, SReadCb, SWriteCb, SEventCb, this);
    bufferevent_enable(impl_->bev_, EV_READ | EV_WRITE);

    timeval tv = { 10, 0 };
    bufferevent_set_timeouts(impl_->bev_, &tv, &tv);

    /// 连接服务器
    if (impl_->serverIp_.empty())
    {
        return true;
    }

    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(impl_->serverPort_);
    evutil_inet_pton(AF_INET, impl_->serverIp_.c_str(), &sin.sin_addr.s_addr);

    int ret = bufferevent_socket_connect(impl_->bev_, (sockaddr *)&sin, sizeof(sin));
    if (ret < 0)
    {
        std::cerr << "bufferevent_socket_connect failed" << std::endl;
        return false;
    }

    return true;
}

void XComTask::setServerIp(const std::string ip)
{
    impl_->serverIp_ = ip;
}

void XComTask::setServerPort(int port)
{
    impl_->serverPort_ = port;
}

void XComTask::setServerRoot(const std::string path)
{
    impl_->serverPath_ = path;
}
