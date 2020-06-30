#include "threadPool.h"

ThreadPool::ThreadPool(int minSize, int maxSize, int maxQueueSize, int timewait)
    :stop_(false)
{
    assert(minSize > 0);
    assert(maxSize >= minSize);
    assert(maxQueueSize > 0);
    assert(timewait > 0);
    
    minSize_ = minSize;
    maxSize_ = maxSize_;
    maxQueueSize_ = maxQueueSize;
    timewait_ = timewait;

    wakeupfd_ = ::eventfd(0, EFD_NONBLOCK |EFD_CLOEXEC);
    if( -1 == wakeupfd_)
    {
        perror("eventfd");
        exit(1);
    }

    timerfd_ = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
    if(timerfd_ == -1)
    {
        perror("timerfd_create");
        exit(1);
    }

    threads_.resize(maxSize_);
    stop_ = vector<bool> (maxSize_,false);
    for(int i = 0; i < minSize_; i++)
    {
        thread temp(&ThreadPool::threadFunc,this, i);
        threads_[i].swap(temp);
    }
    curSize_ = minSize_;
    thread temp(&ThreadPool::watchFunc,this);
    temp.swap(watchThread_);
}

ThreadPool::~ThreadPool()
{
    stop();
    for(auto iter = threads_.begin(); iter != threads_.end(); iter ++)
    {
        iter->join();
    }
    watchThread_.join();
}

void ThreadPool::threadFunc(int i)
{
    curSize_ ++;
    while (1)
    {
        function<void ()> task;
        {
            unique_lock<mutex> lock(mutex_);
            while( gStop_ == false && stop_[i] == false && taskQueue_.empty())
                cv_go_.wait(lock);
            if(stop_[i] == true || gStop_ == true)
                break;
            threadState_[i] = BUZY;
            task = taskQueue_.front();
            taskQueue_.pop();
        }
        task();
        {
            unique_lock<mutex> lock(mutex_);
            threadState_[i] = UNBUZY;
            if(curSize_ > minSize_ && taskQueue_.empty())
            {
                if( timerOn_ == false)
                    wakeup();
            }
            else
            {
                if( timerOn_ == true)
                    wakeup();   
            }
        }
    }

    {
        unique_lock<mutex> lock(mutex_);
        threadState_[i] = LEAVE;
        curSize_ --;
    }
}


void ThreadPool::stop()
{
    unique_lock<mutex> lock(mutex_);
    gStop_ = true;
    cv_go_.notify_all();
    wakeup();
}


void ThreadPool::wakeup()
{
    uint64_t cout = 1;
    int ret = ::write(wakeupfd_,&cout, sizeof(cout));
    if(-1 == ret)
    {
        perror("write eventfd");
        exit(1);
    }
}


void ThreadPool::readWakeup()
{
    uint64_t cout = 0;
    int ret = ::read(wakeupfd_,&cout, sizeof(cout));
    if(-1 == ret)
    {
        perror("write eventfd");
        exit(1);
    }
}


void ThreadPool::handleTime()
{
    if(taskQueue_.size() == 0 && timerOn_ == false)
    {
        // 设置时钟
    }
    else
    {
        // 撤销时钟
        timerOn_ = false;
    }
}

void ThreadPool::watchFunc()
{
    fd_set readfd;
    int ret;
    int maxfd = max(wakeupfd_ , timerfd_);
    while(1)
    {
        FD_ZERO(&readfd);
        FD_SET(maxfd,&readfd);
        ret = select(wakeupfd_+1,&readfd,NULL,NULL,NULL);
        if(FD_ISSET(wakeupfd_,&readfd))
        {
            unique_lock<mutex> lock(mutex_);
            if(gStop_ == true)
                break;
            
            readWakeup();
            
            if( curSize_ > minSize_)
            {
                handleTime();
            }
        }

        if(FD_ISSET(timerfd_, &readfd))
        {
            unique_lock<mutex> lock(mutex_);
            readTimerfd();
            killSomeThread();
        }
    }
}

bool ThreadPool::push(const function<void()> & task)
{
    unique_lock<mutex> lock(mutex_);
    if(taskQueue_.size() <= maxQueueSize_/3)
    {
        taskQueue_.push(task);
    }
    else if(taskQueue_.size() < maxQueueSize_)
    {
        taskQueue_.push(task);
        if(curSize_ < maxSize_)
        {
            int i = 0;
            for(; i < maxSize_ ; i++)
            {
                if(threadState_[i] == LEAVE);
            }
            thread temp(&ThreadPool::threadFunc,this, i);
            threadState_[i] = UNBUZY;
            threads_[i].swap(temp);
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool ThreadPool::push(function<void()> && task)
{
    unique_lock<mutex> lock(mutex_);
    if(taskQueue_.size() <= maxQueueSize_/3)
    {
        taskQueue_.emplace(task);
    }
    else if(taskQueue_.size() < maxQueueSize_)
    {
        taskQueue_.emplace(task);
        if(curSize_ < maxSize_)
        {
            int i = 0;
            for(; i < maxSize_ ; i++)
            {
                if(threadState_[i] == LEAVE);
            }
            thread temp(&ThreadPool::threadFunc,this, i);
            threadState_[i] = UNBUZY;
            threads_[i].swap(temp);
        }
    }
    else
    {
        return false;
    }
    return true;
}
