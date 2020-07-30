#include <iostream>
#include <vector>
using namespace std;
template < typename T, typename C = less<T> >
class heap {
public:
	heap(const vector<T>& d) : data_(d)
	{
		make_heap();
	}
	heap(vector<T>&& d)
	{
		d.swap(data_);
		make_heap();
	}
	void push(const T & newval)
	{
		data_.push_back(newval);
		int cur = data_.size() - 1;
		while (1)
		{
			cur = parent(cur);
			if (cur == -1) return;
			if (modify_heap(cur) == false)
				return;
		}
	}

	void pop()
	{
		std::swap(data_.front(), data_.back());
		data_.pop_back();
		if( data_.size() )
			modify_heap(0);
	}

	T& top()
	{
		return data_.front();
	}
	int size() const { return data_.size(); }
private:
	void make_heap() {
		for (int i = data_.size() / 2; i >= 0; --i)
			modify_heap(i);
	} 

	bool modify_heap(int index)
	{
		int left = leftchild(index);
		int right = rightchild(index);
		int maxloc = index;
		if (left != -1 && c_( data_[maxloc], data_[left] ) == true )
		{
			maxloc = left;
		}
		if (right != -1 && c_(data_[maxloc], data_[right]) == true)
		{
			maxloc = right;
		}
		if (maxloc == index) return false;
		else
		{
			std::swap(data_[maxloc], data_[index]);
			modify_heap(maxloc);
			return true;
		}
	}

	inline int leftchild(int index);
	inline int rightchild(int index);
	inline int parent(int index);
	vector<T> data_;
	const C c_;
};


template <typename T, typename C> 
int heap<T,C>::leftchild(int index)
{
	int tmp = index * 2 + 1;
	if (tmp > data_.size() - 1) return -1;
	else return tmp;
}
template <typename T, typename C>
int heap<T, C>::rightchild(int index)
{
	int tmp = index * 2 + 2;
	if (tmp > data_.size() - 1) return -1;
	else return tmp;
}

template <typename T, typename C>
int heap<T, C>::parent(int index)
{
	return (index - 1) / 2;
}


int main()
{
	vector<int> num{ 2,5,8,1,3,9,10 };
	heap<int> h(num);
	for (int i = 10; i < 30; ++i)
		h.push(i);
	while (h.size())
	{
		cout << h.top() << "  ";
		h.pop();
	}
	cout << endl;
	return 0;
} 
