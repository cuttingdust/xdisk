/**
 * @file   XFileServerTask.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-14
 */

#ifndef XFILESERVERTASK_H
#define XFILESERVERTASK_H

#include "XCom_Global.h"
#include "XComTask.h"

class XCOM_EXPORT XFileServerTask : public XComTask
{
public:
    XFileServerTask();
    virtual ~XFileServerTask();

public:
    bool read(const XMsg *msg) override;

    void read(void *data, int size) override;

    void writeCB() override;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};


#endif // XFILESERVERTASK_H
