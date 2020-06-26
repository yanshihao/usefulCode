#include <mutex>
#include <thread>
using namespace std;
// 懒汉模式，非线程安全
template<typename Type>
class Singleton
{
private:
    static Type* data;
public:
    static Type* getInstance()
    {
        if(data == nullptr)
            data = new Type;
        printf("Singleton %p\n", data);
        return data;
    }
};
template<typename Type> Type* Singleton<Type>::data = nullptr;

// 懒汉模式，线程安全,但是每次调用都要获取锁，不太行
template<typename Type>
class SingletonSafe1
{
private:
    static Type* data;
    static mutex mutex_;
public:
    static Type* getInstance()
    {
        unique_lock<mutex> lock(mutex_);    
        if(data == nullptr)
            data = new Type;
        printf("SingletonSafe1 %p\n", data);
        return data;
    }
};
template<typename Type> Type* SingletonSafe1<Type>::data = nullptr;
template<typename Type> mutex SingletonSafe1<Type>::mutex_;

// 饿汉模式，天生线程安全，因为会在所有线程前构造
template<typename Type>
class SingletonSafe2
{
private:
    static Type* data;
public:
    static Type* getInstance()
    {
        printf("SingletonSafe2 %p\n", data);
        return data;
    }
};
template<typename Type> Type* SingletonSafe2<Type>::data = new Type;


// 懒汉模式，双检查
template<typename Type>
class SingletonSafe3
{
private:
    static Type* data;
    static mutex mutex_;
public:
    static Type* getInstance()
    {
        if(data == nullptr)
        {
            unique_lock<mutex> lock(mutex_);
            if(data == nullptr)
                data = new Type;
        }
        printf("SingletonSafe3 %p\n", data);
        return data;
    }
};
template<typename Type> Type* SingletonSafe3<Type>::data = nullptr;
template<typename Type> mutex SingletonSafe3<Type>::mutex_;

int main()
{
    thread t1(
        []()
        {
            int* p1 = SingletonSafe1<int>::getInstance();
            int* p2 = SingletonSafe2<int>::getInstance();
            int* p3 = SingletonSafe3<int>::getInstance();
            SingletonSafe1<int>::getInstance();
            SingletonSafe2<int>::getInstance();
            SingletonSafe3<int>::getInstance();
        }
    );
    int* p0 = Singleton<int>::getInstance();
    int* p1 = SingletonSafe1<int>::getInstance();
    int* p2 = SingletonSafe2<int>::getInstance();
    int* p3 = SingletonSafe3<int>::getInstance();
    SingletonSafe1<int>::getInstance();
    SingletonSafe2<int>::getInstance();
    SingletonSafe3<int>::getInstance();
    t1.join();
    return 0;
}