// g++ users: `g++ sample.cc sole.cpp -std=c++11 -lrt -o sample`
// visual studio: `cl.exe sample.cc sole.cpp`

#include <iostream>
#include "sole.hpp"

int main()
{
    using namespace sole;

    std::cout << "uuid v0 sample: " << uuid0() << std::endl;
    std::cout << "uuid v1 sample: " << uuid1() << std::endl;
    std::cout << "uuid v4 sample: " << uuid4() << std::endl;

    return 0;
}
