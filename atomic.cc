#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

int main()
{
    std::atomic<int> total1(0);
    int total2(0);
    std::thread t1(
        [&]()
        {
            for(int i = 0; i < 100000; i++)
            {
                total1 ++;
                total2 ++;
            }
        }
    );

    for(int i = 0; i < 100000; i++)
    {
        total1 ++;
        total2 ++;
    }
    
    t1.join();
    std::cout << "atomic     " <<total1 << std::endl;
    std::cout << "nonatomic  " <<total2 << std::endl;
    return 0;
}