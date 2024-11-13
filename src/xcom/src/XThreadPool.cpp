#include "XThreadPool.h"

#include "XThread.h"

#include <vector>
#include <thread>
#include <iostream>

class XThreadPool::PImpl
{
public:
    PImpl(XThreadPool *owenr);
    ~PImpl();

public:
    XThreadPool           *owenr_     = nullptr;
    int                    threadNum_ = 0;
    std::vector<XThread *> threads_;
    int                    lastThread_ = -1;
};

XThreadPool::PImpl::PImpl(XThreadPool *owenr) : owenr_(owenr)
{
}

XThreadPool::PImpl::~PImpl() = default;

/// ��ʼ�������̲߳������߳�
auto XThreadPool::init(int threadNum) -> void
{
    impl_->threadNum_  = threadNum;
    impl_->lastThread_ = -1;
    for (int i = 0; i < threadNum; ++i)
    {
        XThread *t = new XThread();
        t->setId(i + 1);
        std::cout << "create thread " << i << std::endl;
        /// �����߳�
        t->start();
        impl_->threads_.emplace_back(t);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

auto XThreadPool::dispatch(XTask *task) -> void
{
    /// ��ѯ
    if (!task)
    {
        return;
    }

    int tid            = (impl_->lastThread_ + 1) % impl_->threadNum_;
    impl_->lastThread_ = tid;
    XThread *t         = impl_->threads_[tid];

    /// �������
    t->addTask(task);

    /// �����߳�
    t->activate();
}

XThreadPool::XThreadPool()
{
    impl_ = std::make_shared<PImpl>(this);
}

XThreadPool::~XThreadPool()
{
}
