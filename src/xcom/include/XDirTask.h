/**
 * @file   XDirTask.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-14
 */

#ifndef XDIRTASK_H
#define XDIRTASK_H

#include "XCom_Global.h"
#include "XComTask.h"

typedef void      (*DirDBFunc)(std::string dirs);
class XCOM_EXPORT XDirTask : public XComTask
{
public:
    /// \brief 连接成功的回调
    void connectCB() override;

    /// \brief 接受消息的回调
    /// \param msg
    bool read(const XMsg *msg) override;

    void setServerRoot(const std::string path);

public:
    DirDBFunc   dirCB = 0;
    std::string serverRoot_;
};

#endif // XDIRTASK_H
