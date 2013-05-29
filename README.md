sole
====

- Sole is a lightweight C++11 library to generate universally unique identificators.
- Sole provides interface for UUID both versions 1 and 4. Custom version 0 is provided additionally.
- Sole is tiny. One header and one source file.
- Sole is cross-platform. Compiles under MSVC/GCC. Works on Windows/Linux.
- Sole is MIT licensed.

some theory
-----------
- UUID version 1 (48-bit MAC address + 60-bit clock with a resolution of 100 ns)
- UUID version 4 (122-bits of randomness)
- Use v1 if you are worried about leaving it up to probabilities.
- Use v4 if you are worried about security issues and determinism.

about custom version 0
----------------------
- UUID version 0 (16-bit PID + 48-bit MAC address + 60-bit clock with a resolution of 100ns since Unix epoch)
- Format is EPOCH_LOW-EPOCH_MID-VERSION(0)|EPOCH_HI-PID-MAC

public API
----------
- `sole::uuid` 128-bit UUID base type that allows comparison and sorting. `std::ostream <<` friendly. `.str()` to get cooked string. `.pretty()` to get a pretty report.
- `sole::uuid0()` creates an UUID v0.
- `sole::uuid1()` creates an UUID v1.
- `sole::uuid4()` creates an UUID v4.
- `sole::rebuild()` rebuilds an UUID from given string or 64-bit tuple.

sample
------
```
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

    return 0;
}
```

possible output
---------------
```
D:\prj\sole>cl sample.cc sole.cpp
D:\prj\sole>sample.exe
uuid v0 sample : ef300c1d-aaaa-0030-0e04-00ffb77bd832 -> version=0,timestamp="2013-05-29 18:30:01",mac=00ffb77bd832,pid=3588,
uuid v1 sample : 02b25d97-c87d-11e2-9d97-00ffb77bd832 -> version=1,timestamp="2013-05-29 18:30:01",mac=00ffb77bd832,clock_seq=7575,
uuid v4 sample : bda27a84-f567-44ce-8654-cc459e23353c -> version=4,randbits=bda27a84f56704ce654cc459e23353c,
uuid v1 rebuilt: f81d4fae-7dec-11d0-a765-00a0c91e6bf6 -> version=1,timestamp="1997-02-03 18:43:12",mac=00a0c91e6bf6,clock_seq=10085,
D:\prj\sole>
```

special notes
-------------
- g++ users: both `-std=c++11` and `-lrt` may be required when compiling `sole.cpp`
