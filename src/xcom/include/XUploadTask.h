/**
 * @file   XUploadTask.h
 * @brief  
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-15
 */

#ifndef XUPLOADTASK_H
#define XUPLOADTASK_H

#include "XCom_Global.h"
#include "XComTask.h"

typedef void      (*UploadCBFunc)();
class XCOM_EXPORT XUploadTask : public XComTask

{
public:
    XUploadTask();
    ~XUploadTask() override;

public:
    void writeCB() override;

    bool read(const XMsg *msg) override;

    void connectCB() override;

    void setFilePath(const std::string &path);

    /// \brief 上传成功后的回调
    UploadCBFunc uploadCB = nullptr;

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XUPLOADTASK_H
