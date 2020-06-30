#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <assert.h>
#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <queue>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/select.h>
#include <unistd.h>
#include <unordered_set>
#include <atomic>

using namespace std;

class ThreadPool
{
public:
    ThreadPool(int minSize, int maxSize, int maxQueueSize, int timewait);

    ~ThreadPool();

    bool push(function<void ()>&& task);
    bool push(const function<void()> & task);

private:
    void threadFunc(int i);    // 线程函数
    void watchFunc();          // 监控线程

    void stop();

    void wakeup();             // 唤醒
    void readWakeup();         // 读唤醒
    void handleTime();
    void readTimerfd();
    void killSomeThread();

    int minSize_;              // 最小线程数
    atomic<int> curSize_;      // 当前线程
    int maxSize_;              // 最大线程数

    int maxQueueSize_;         // 最大等待任务数
    int timewait_;             // 最大等待时长 , ms

    vector<thread>  threads_;  // 线程实例

    enum State{UNBUZY, BUZY, LEAVE};
    vector<atomic<State>>   threadState_;

    thread  watchThread_;      // 监控线程

    bool gStop_;                    // 全局停止
    vector<bool> stop_;             // 停止
    condition_variable  cv_go_;     // 工作的条件变量
    mutex      mutex_;              // 锁
    queue<function<void ()>>  taskQueue_;    // 任务队列

    int wakeupfd_;
    int timerfd_;
    bool timerOn_;
};