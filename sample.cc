// g++ users: `g++ sample.cc sole.cpp -std=c++11 -lrt -o sample`
// visual studio: `cl.exe sample.cc sole.cpp`

#include <iostream>
#include "sole.hpp"

int main()
{
    sole::uuid u0 = sole::uuid0(), u1 = sole::uuid1(), u4 = sole::uuid4();

    std::cout << "uuid v0 sample : " << u0 << " -> " << u0.pretty() << std::endl;
    std::cout << "uuid v1 sample : " << u1 << " -> " << u1.pretty() <<  std::endl;
    std::cout << "uuid v4 sample : " << u4 << " -> " << u4.pretty() <<  std::endl;

    u1 = sole::rebuild("F81D4FAE-7DEC-11D0-A765-00A0C91E6BF6");
    std::cout << "uuid v1 rebuilt: " << u1 << " -> " << u1.pretty() <<  std::endl;

    std::cout << sizeof(u1) * 8 << " bits" << std::endl;
    std::cout << sizeof(sole::uuid) * 8 << " bits" << std::endl;

    return 0;
}
