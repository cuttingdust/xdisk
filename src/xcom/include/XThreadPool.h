/**
 * @file   XThreadPool.h
 * @brief  �̳߳� 
 *
 * Detailed description if necessary.
 *
 * @author 31667
 * @date   2024-11-05
 */

#ifndef XTHREADPOOL_H
#define XTHREADPOOL_H

#include "XCom_Global.h"
#include <memory>

class XTask;

class XCOM_EXPORT XThreadPool
{
public:
    static XThreadPool *getInstance()
    {
        static XThreadPool pool;
        return &pool;
    }

    /// \brief ��ʼ�������̲߳������߳�
    /// \param threadNum
    auto init(int threadNum = 4) -> void;

    /// \brief �ַ��߳�
    /// \param task
    auto dispatch(XTask *task) -> void;

private:
    XThreadPool();
    virtual ~XThreadPool();

private:
    class PImpl;
    std::shared_ptr<PImpl> impl_;
};

#endif // XTHREADPOOL_H
