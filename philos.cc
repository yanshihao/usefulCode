// 哲学家就餐问题

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <mutex>
#include <chrono>
#include <condition_variable>
using namespace std;

vector<bool> phi(5, false);
vector<mutex> fork(5);
mutex mu;
condition_variable cv;
int counts = 4;
void eat( int i )
{
	{
		unique_lock<mutex> lock(mu);
		cv.wait(lock, []() {
			return counts > 0;
			});
		counts--;
	}

	int j = (i + 1) % 5;
	fork[i].lock();
	fork[j].lock();
	this_thread::sleep_for( chrono::microseconds(500) );
	printf("%d eat\n", i);
	fork[j].unlock();
	fork[i].unlock();

	{
		unique_lock<mutex> lock(mu);
		counts++;
		cv.notify_one();
	}
}


int main()
{
	vector<thread> ths(5);
	for (int i = 0; i < 5; ++i)
	{
		thread t(eat, i);
		ths[i].swap(t);
	}

	for (int i = 0; i < 5; ++i)
		ths[i].join();
	return 0;
}
