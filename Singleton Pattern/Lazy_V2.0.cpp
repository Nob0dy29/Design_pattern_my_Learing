#include <iostream>
#include <memory> // shared_ptr
#include <mutex>  // mutex

using namespace std;

//版本2.0
//1.使用互斥锁解决线程安全问题
//2.使用智能指针解决内存泄露问题

class Lazy_V2_Singleton
{
private:
    Lazy_V2_Singleton()
    {
        cout<<"构造函数"<<endl;
    }

    Lazy_V2_Singleton(Lazy_V2_Singleton&) = delete;
    Lazy_V2_Singleton& operator= (const Lazy_V2_Singleton&) = delete;

    static shared_ptr<Lazy_V2_Singleton> m_instance_ptr;
    static mutex m_mutex;

public:
    ~Lazy_V2_Singleton()
    {
        cout<<"析构函数"<<endl;
    }

    static shared_ptr<Lazy_V2_Singleton> get_instance()
    {
        if(m_instance_ptr == nullptr)
        {
            //上锁 RAII
            lock_guard<mutex> lk(m_mutex);
            if(m_instance_ptr == nullptr)
            {
                m_instance_ptr = shared_ptr<Lazy_V2_Singleton>(new Lazy_V2_Singleton);
            }
        }
        return m_instance_ptr;
    }
};

shared_ptr<Lazy_V2_Singleton> Lazy_V2_Singleton::m_instance_ptr = nullptr;
mutex Lazy_V2_Singleton::m_mutex;
int main()
{
    shared_ptr<Lazy_V2_Singleton> instance = Lazy_V2_Singleton::get_instance();
    shared_ptr<Lazy_V2_Singleton> instance_2 = Lazy_V2_Singleton::get_instance();
    return 0;
}