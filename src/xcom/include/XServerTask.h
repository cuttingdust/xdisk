/**
 * @file   XServerTask.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-12
 */

#ifndef XSERVERTASK_H
#define XSERVERTASK_H

#include "XCom_Global.h"
#include "XTask.h"

#include <memory>

using ListenCBFunc = void (*)(int sock, struct sockaddr *, int socklen, void *);

class XCOM_EXPORT XServerTask : public XTask
{
public:
    XServerTask();
    virtual ~XServerTask();

public:
    auto         set_server_port(int server_port) -> void;
    auto         init() -> bool override;
    ListenCBFunc listen_cb_ = nullptr;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XSERVERTASK_H
