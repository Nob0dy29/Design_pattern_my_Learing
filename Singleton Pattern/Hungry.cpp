#include <iostream>

using namespace std;

class Hungry
{
private:
    Hungry()
    {
        cout<<"构造函数"<<endl;
    }
    Hungry(Hungry&) = delete;
    Hungry operator= (Hungry&) = delete;
    static Hungry* m_instance_ptr;

public:
    ~Hungry()
    {
        cout<<"析构函数"<<endl;
    }

    static Hungry* get_instance()
    {
        return m_instance_ptr;
    }
};
Hungry* Hungry::m_instance_ptr = new Hungry();

int main()
{
    Hungry* instance = Hungry::get_instance();
    Hungry* instance_1 = Hungry::get_instance();
    return 0;
}