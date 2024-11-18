/**
 * @file   XDownLoadTask.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-18
 */

#ifndef XDOWNLOADTASK_H
#define XDOWNLOADTASK_H

#include "XCom_Global.h"
#include "XComTask.h"

typedef void      (*DownLoadCBFunc)();
class XCOM_EXPORT XDownLoadTask : public XComTask
{
public:
    XDownLoadTask();
    virtual ~XDownLoadTask();

public:
    void read(void *data, int size) override;

    bool read(const XMsg *msg) override;

    void connectCB() override;

    void setFilePath(const std::string &path);

    void setLocalPath(const std::string &path);

    DownLoadCBFunc downloadCB = nullptr;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XDOWNLOADTASK_H
