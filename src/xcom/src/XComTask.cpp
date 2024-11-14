#include "XComTask.h"
#include "XMsg.h"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include <iostream>

static void SReadCb(struct bufferevent *bev, void *ctx)
{
    auto task = static_cast<XComTask *>(ctx);
    task->readCB();
}

static void SWriteCb(struct bufferevent *bev, void *ctx)
{
    std::cout << "SWriteCb" << std::endl;
}

static void SEventCb(struct bufferevent *bev, short events, void *ctx)
{
    auto task = static_cast<XComTask *>(ctx);
    task->eventCB(events);
}

class XComTask::PImpl
{
public:
    PImpl(XComTask *owenr);
    ~PImpl();

public:
    bool write(const XMsg *msg);

public:
    XComTask           *owenr_      = nullptr;
    struct bufferevent *bev_        = nullptr;
    std::string         serverPath_ = "";
    std::string         serverIp_   = "";
    int                 serverPort_ = -1;
    char                buffer_[1024];
    XMsg                msg_;
};

XComTask::PImpl::PImpl(XComTask *owenr) : owenr_(owenr)
{
}

XComTask::PImpl::~PImpl() = default;

bool XComTask::PImpl::write(const XMsg *msg)
{
    if (!bev_ || !msg || !msg->data || msg->size <= 0)
    {
        return false;
    }
    /// 发送消息头
    int len = bufferevent_write(bev_, msg, sizeof(XMsgHead));
    if (len != 0)
    {
        return false;
    }

    /// 发送消息内容
    len = bufferevent_write(bev_, msg->data, msg->size);
    if (len != 0)
    {
        return false;
    }
    return true;
}

XComTask::XComTask()
{
    impl_ = std::make_shared<PImpl>(this);
}

XComTask::~XComTask() = default;

auto XComTask::init() -> bool
{
    /// 区分客户端和服务器
    int comSock = this->sock();
    if (comSock <= 0)
        comSock = -1;

    /// 用bufferevent建立连接
    impl_->bev_ = bufferevent_socket_new(base(), comSock, BEV_OPT_CLOSE_ON_FREE);
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

void XComTask::eventCB(short events)
{
    if (events & BEV_EVENT_CONNECTED)
    {
        std::cout << "BEV_EVENT_CONNECTED" << std::endl;

        XMsg msg;
        msg.type = MSG_GETDIR;
        msg.size = 3;
        msg.data = (char *)"./";
        impl_->write(&msg);
        // bufferevent_write(impl_->bev_, &msg, sizeof(XMsgHead));
    }

    /// 退出要处理缓冲内容
    if (events & (BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
    {
        std::cout << "BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << std::endl;
        bufferevent_free(impl_->bev_);
    }

    if (events & BEV_EVENT_EOF)
    {
        std::cout << "BEV_EVENT_EOF" << std::endl;
        bufferevent_free(impl_->bev_);
    }
}

void XComTask::readCB()
{
    /// 接受消息
    for (;;)
    {
        /// 接受消息头部
        if (!impl_->msg_.data)
        {
            int headSize = sizeof(XMsgHead);
            int len      = bufferevent_read(impl_->bev_, &impl_->msg_, headSize);
            if (len <= 0)
            {
                return;
            }
            if (len != headSize)
            {
                std::cerr << "readCB: msg head error!!!" << std::endl;
                return;
            }

            /// 验证消息头部的有效性
            if (impl_->msg_.type > MSG_MAX_TYPE || impl_->msg_.size <= 0 || impl_->msg_.size > MSG_MAX_SIZE)
            {
                std::cerr << "readCB: msg head error!!!" << std::endl;
                return;
            }
            impl_->msg_.data = new char[impl_->msg_.size];
        }

        /// 接受消息内容
        int readSize = impl_->msg_.size - impl_->msg_.recved;
        if (readSize <= 0)
        {
            delete impl_->msg_.data;
            memset(&impl_->msg_, 0, sizeof(impl_->msg_));
            return;
        }
        int len = bufferevent_read(impl_->bev_, impl_->msg_.data + impl_->msg_.recved, readSize);
        if (len <= 0)
        {
            delete impl_->msg_.data;
            memset(&impl_->msg_, 0, sizeof(impl_->msg_));
            return;
        }
        impl_->msg_.recved += len;
        if (impl_->msg_.size == impl_->msg_.recved)
        {
            /// 处理消息
            readCB(&impl_->msg_);
            delete impl_->msg_.data;
            memset(&impl_->msg_, 0, sizeof(impl_->msg_));
        }
    }
}

void XComTask::readCB(const XMsg *msg)
{
    std::cout << "recv Msg type: " << msg->type << " size: " << msg->size << std::endl;
}

void XComTask::writeCB()
{
    std::cout << "writeCB" << std::endl;
}
