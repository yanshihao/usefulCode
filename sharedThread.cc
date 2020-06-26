#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <list>
#include <chrono>
#include <functional>
using namespace std;


class Test
{
public:
    Test(int n)
    :nothing_(n)
    {
        cout << "Test  " << nothing_ << "  " <<  this_thread::get_id() << endl;
    }
    ~Test()
    {
        cout << "~Test " << nothing_ << "  "<<this_thread::get_id() << endl;
    }
private:
    int nothing_;
};

class SThread
{
public:
    SThread()
    : t_(std::bind(&SThread::threadFunc, this))
    {

    }
    void insert(shared_ptr<Test>& sp)
    {
        unique_lock<mutex> lock;
        sharedList_.push_back(sp);
    }
private:
    void threadFunc()
    {
        while(1)
        {
            this_thread::sleep_for(chrono::seconds(2));
            list<shared_ptr<Test>> temp;
            {
                unique_lock<mutex> lock;
                temp.swap(sharedList_);
            }
            for(auto iter = temp.begin(); iter != temp.end();)
            {
                if(iter->unique())
                {
                    iter = temp.erase(iter);
                }
                else
                {
                    iter ++;
                }
            }
            {
                unique_lock<mutex> lock;
                temp.merge(sharedList_);
                sharedList_.swap(temp);
            }
        }
    }
    thread t_;
    mutex mutex_;
    list<shared_ptr<Test>> sharedList_;
};

int main()
{
    SThread s1;
    int n = 0;
    while(1)
    {
        shared_ptr<Test> pt(new Test(n++));
        s1.insert(pt);
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}
