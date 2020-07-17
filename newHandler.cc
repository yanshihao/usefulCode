#include <memory>
#include <iostream>
#include <stdlib.h>
#include <exception>

// RAII, 负责释放的时候重置new_hanler
class NewHandlerHolder{
public:
    explicit NewHandlerHolder(std::new_handler nh)
    :handler(nh){}

    NewHandlerHolder(const NewHandlerHolder&) = delete;
    NewHandlerHolder& operator-(const NewHandlerHolder&) = delete;
    
    ~NewHandlerHolder()
    {
        std::set_new_handler(handler);
    }
private:
    std::new_handler handler;
};


// 扩展为模板基类，更有效的复用
template <typename T>
class WidgetBase{
public:
    static std::new_handler set_new_handler(std::new_handler p) throw();
    static void* operator new(std::size_t size);
private:
    static std::new_handler currentHandler;
};

template <typename T>
std::new_handler WidgetBase<T>::currentHandler=0;
template <typename T>
std::new_handler WidgetBase<T>::set_new_handler(std::new_handler p) throw()
{
    std::new_handler oldHandler=currentHandler;
    currentHandler=p;
    return oldHandler;
}

template <typename T>
void* WidgetBase<T>::operator new(std::size_t size)
{
    NewHandlerHolder handle(std::set_new_handler( currentHandler ));
    return ::operator new(size);
}


// 继承，由于继承了不同的模板参数的Base类，static变量有多个实例
class Widget
: public WidgetBase<Widget>
{

private:
    int testNum_[0xf000000000];
};

// 继承
class Widget2
: public WidgetBase<Widget2>
{

private:
    int testNum_[0xf000000000];
};


void handler()
{
    std::cout << "heheda" << std::endl;
    exit(1);
}
void handler2()
{
    std::cout << "heheda2" << std::endl;
    exit(1);
}

int main()
{
    Widget::set_new_handler(handler);
    Widget2::set_new_handler(handler2);
    
    Widget* p = new Widget;
    Widget2* p2 = new Widget2;
    return 0;
}