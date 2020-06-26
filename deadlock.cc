#include <mutex>
#include <thread>

using namespace std;

mutex mutex1;
mutex mutex2;

int main()
{    
    thread t1(
        []()
        {
            mutex1.lock();
            this_thread::sleep_for(chrono::seconds(1));
            mutex2.lock();
        }
    );
    mutex2.lock();
    this_thread::sleep_for(chrono::seconds(1));
    mutex1.lock();
    return 0;
}


