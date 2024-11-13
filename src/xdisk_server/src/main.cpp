#include <XThreadPool.h>
#include <XServerTask.h>

#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <winsock2.h>
#endif

#include <iostream>
#include <thread>

#define SPORT 8080

static void SListenCB(struct evconnlistener *, intptr_t, struct sockaddr *, int socklen, void *)
{
    std::cout << "SListenCB in main" << std::endl;
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

#else
    /// 忽略管道信号，发送数据给已关闭的socket
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;
#endif

    int port      = SPORT;
    int threadNum = 10;

    /// 初始化线程池
    XThreadPool::getInstance()->init(threadNum);
    auto task = new XServerTask();
    task->set_server_port(port);
    task->listen_cb_ = SListenCB;
    XThreadPool::getInstance()->dispatch(task);

    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
