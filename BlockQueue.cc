#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <condition_variable>
#include <iostream>
using namespace std;
template <typename Type>

class BlockQueue
{
public:
    BlockQueue(size_t maxSize)
    :maxSize_(maxSize)
    {}
    void push(const Type& ton)
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.size >= maxSize_)
        {
            notfull_.wait(lock);
        }
        queue_.push(ton);
        notempty_.notify_one();
    }

    void push(Type&& ton)
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.size() >= maxSize_)
        {
            notfull_.wait(lock);
        }
        queue_.emplace(ton);
        notempty_.notify_one();
    }

    Type pop()
    {
        unique_lock<mutex> lock(mutex_);
        while(queue_.empty())
        {
            notempty_.wait(lock);
        }
        Type data = queue_.front();
        queue_.pop();
        notfull_.notify_one();
        return data;
    }
private:
    queue<Type> queue_;
    mutex mutex_;
    condition_variable notfull_;
    condition_variable notempty_;
    size_t maxSize_;
};


int main()
{
    mutex mutex1;
    BlockQueue<std::function<void ()>> blockqueue(5);

    thread t1(
        [&]()
        {
            while(1)
            {
                auto functor = blockqueue.pop();
                functor();
            }
        }
    );

    thread t2(
        [&]()
        {
            while(1)
            {
                auto functor = blockqueue.pop();
                functor();
            }
        }
    );

    thread t3(
        [&]()
        {
            while(1)
            {
                auto functor = blockqueue.pop();
                functor();
            }
        }
    );

    int i = 0;
    while(1)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
        blockqueue.push(
            [&, i]()
            {
                unique_lock<mutex> lock(mutex1);
                cout << this_thread::get_id()<< "   " << i << endl;
            }
        );
        i++;
    }
    t1.join();
    return 0;
}

