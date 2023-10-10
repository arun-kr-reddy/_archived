#include <iostream >
#include <memory>

class A
{
public:
    A(int a) { std ::cout << "alive" << std::endl; }
    ~A() { std ::cout << "dead" << std::endl; }
};

int main()
{
    float a = 1.2;
    int b = (int)a + 1;
    int c = int(a) + 1;

    // equivalent to std::shared_ptr<A>(new A(10));
    auto a_ptr = std ::make_shared<A>(10);          // alive
    std ::cout << a_ptr.use_count() << std ::endl;  // 1
    {
        auto b_ptr = a_ptr;                             // alive
        std ::cout << a_ptr.use_count() << std ::endl;  // 2
    }
    std ::cout << "main scope\n";
    std ::cout << a_ptr.use_count() << std ::endl;  // 1

    return 0;  // dead
}