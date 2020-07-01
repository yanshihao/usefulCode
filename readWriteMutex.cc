#include <mutex>
#include <memory>
#include <functional>
#include <condition_variable>


#include <thread>
#include <vector>
using namespace std;

// 实现一个读优先的单一写者的

class RAII
{
public:
    RAII(const function<void()>& func)
    :func_(func) {}
    ~RAII()
    {
        func_();
    }
private:
    function<void()>  func_;
};


class Rwmutex
{
public:
    Rwmutex()
    :uniqueLock(false), sharedNum_(0)
    {
    }
    shared_ptr<RAII> lock()
    {
        {
            unique_lock<mutex> lock(mutex_);
            while(uniqueLock == true || sharedNum_ != 0)
            {
                canWrite_.wait(lock);
            }
            uniqueLock = true;
        }
        auto func = std::bind(&Rwmutex::unlock,this);
        return shared_ptr<RAII> (new RAII( func));
    }
    shared_ptr<RAII> shared_lock()
    {
        {
            unique_lock<mutex> lock(mutex_);
            while(uniqueLock == true)
            {
                canRead_.wait(lock);
            }
            sharedNum_ ++;
        }

        auto func = std::bind(&Rwmutex::shared_unlock,this);
        return shared_ptr<RAII> (new RAII( func));
    }

    void unlock()
    {
        unique_lock<mutex> lock(mutex_);
        uniqueLock = false;
        canRead_.notify_all();
        canWrite_.notify_one();
    }
    void shared_unlock()
    {
        unique_lock<mutex> lock(mutex_);
        sharedNum_ --;
        if(sharedNum_ == 0)
            canWrite_.notify_one();
    }
private:
    mutex mutex_;
    condition_variable canRead_;
    condition_variable canWrite_;
    bool uniqueLock;
    size_t sharedNum_;
};


int main()
{
    vector<int>  data{1,2,3,4,5,6,7,8,9};
    Rwmutex rwmutex;

    vector<thread> t;
    for(int i = 0; i < 10 ; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(30));
        t.push_back(
            thread([&]()
            {
                while(1)
                {
                    this_thread::sleep_for(chrono::milliseconds(300));
                    auto temp = rwmutex.shared_lock();
                    this_thread::sleep_for(chrono::milliseconds(100));
                    printf("%ld\n", data.size());
                }
            })
        );
    }

    
    while(1)
    {
        this_thread::sleep_for(chrono::seconds(1));
        {
            auto temp = rwmutex.lock();
            this_thread::sleep_for(chrono::milliseconds(300));
            data.push_back(10);
        }
        this_thread::sleep_for(chrono::seconds(1));
        {
            auto temp = rwmutex.lock();
            this_thread::sleep_for(chrono::milliseconds(300));
            data.pop_back();
        }
    }
}
