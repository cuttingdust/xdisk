#include "XDirTask.h"

#include "XMsg.h"

#include <iostream>

void XDirTask::connectCB()
{
    std::cout << "XDirTask::connectCB" << std::endl;
    XMsg msg;
    msg.type = MSG_GETDIR;
    msg.size = serverRoot_.size() + 1;
    msg.data = (char *)serverRoot_.c_str();
    write(&msg);
}

void XDirTask::read(const XMsg *msg)
{
    switch (msg->type)
    {
        case MSG_DIRLIST: /// 服务器返回目录列表
            {
                std::cout << "MSG_DIRLIST" << std::endl;
                dirCB(msg->data);
                break;
            }
        default:
            break;
    }
}

void XDirTask::setServerRoot(const std::string path)
{
    this->serverRoot_ = path;
}
