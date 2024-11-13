#include "XDiskGui.h"

#include <QtWidgets/QApplication>

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

    QApplication app(argc, argv);

    XDiskGui w;
    w.show();

    app.exec();
    return 0;
}
