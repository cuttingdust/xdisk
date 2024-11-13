#include "XServerTask.h"
#include <iostream>

#include <event2/listener.h>

#ifdef _WIN32
#include <winsock2.h>
#endif

static auto SListenCB(struct evconnlistener *ev, evutil_socket_t fd, struct sockaddr *sin, int socklen, void *arg)
        -> void
{
    std::cout << __func__ << std::endl;
    auto task = static_cast<XServerTask *>(arg);
    if (task && task->listen_cb_)
        task->listen_cb_(ev, fd, sin, socklen, arg);
    else
    {
        std::cerr << "please set listen callback!" << std::endl;
    }
}


class XServerTask::PImpl
{
public:
    PImpl(XServerTask *owenr);
    ~PImpl() = default;

public:
    XServerTask *owenr_       = nullptr;
    int          server_port_ = -1;
};

XServerTask::PImpl::PImpl(XServerTask *owenr) : owenr_(owenr)
{
}


XServerTask::XServerTask()
{
    impl_ = std::make_shared<XServerTask::PImpl>(this);
}

XServerTask::~XServerTask()
{
}

auto XServerTask::set_server_port(int server_port) -> void
{
    impl_->server_port_ = server_port;
}

auto XServerTask::init() -> bool
{
    if (impl_->server_port_ <= 0)
    {
        std::cerr << "XServerTask::init() failed server port is not set!" << std::endl;
        return false;
    }

    /// 绑定端口和地址
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(impl_->server_port_); // NOLINT(clang-diagnostic-implicit-int-conversion)

    /// 监听端口
    ///	设置回调函数
    evconnlistener *ev = evconnlistener_new_bind(
            base(),                                    ///  libevent的上下文
            SListenCB,                                 /// 接收到连接的回调函数
            this,                                      /// 回调函数获取的参数 arg
            LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, /// 地址重用，evconnlistener关闭同时关闭socket
            10,                                        /// 连接队列大小，对应listen函数
            reinterpret_cast<sockaddr *>(&sin),        /// 绑定的地址和端口
            sizeof(sin));
    if (!ev)
    {
        std::cerr << "evconnlistener_new_bind failed!" << std::endl;
        return false;
    }
    std::cout << "evconnlistener_new_bind success!" << std::endl;

    return true;
}
