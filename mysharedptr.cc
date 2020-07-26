#include< iostream >
namespace my {
	template <typename T>
	class shared_ptr {
	public:
		shared_ptr(T* ptr) {
			count_ = new int(1);
			data_ = ptr;
		}
		shared_ptr(const shared_ptr<T>& other) {
			count_ = other.count_;
			data_ = other.data_;
			(*count_)++;
		}
		const shared_ptr<T>& operator = (const shared_ptr<T>& other) = delete;

		T* get() const { return data_; }

		int use_count() const { return *count_; }

		T* operator->() { return data_; }

		~shared_ptr()
		{
			(*count_)--;
			if (*count_ == 0)
			{
				delete count_;
				delete data_;
			}
		}
	private:
		int* count_;
		T* data_;
	};
};
using namespace std;
class Emm
{
public:
	Emm(int a) :he_(a) {};
	~Emm()
	{
		cout << "~EMM" << endl;
	}
	void print()
	{
		cout << he_ << endl;
	}
private:
	int he_;
};
int main()
{
	my::shared_ptr<Emm> pe(new Emm(5));
	my::shared_ptr<Emm> pr(pe);
	cout << pe.use_count() << endl;
	pr->print();
	return 0;
}
