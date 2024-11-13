/**
 * @file   XComTask.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-14
 */

#ifndef XCOMTASK_H
#define XCOMTASK_H

#include "XCom_Global.h"
#include "XTask.h"

#include <memory>
#include <string>

class XCOM_EXPORT XComTask : public XTask
{
public:
    XComTask();
    virtual ~XComTask();

public:
    auto init() -> bool override;
    void setServerIp(const std::string ip);
    void setServerPort(int port);
    void setServerRoot(const std::string path);

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XCOMTASK_H