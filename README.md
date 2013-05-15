sole
====

- Sole is a lightweight C++11 library to generate unique global identificators.
- Sole provides interface for UUID versions 1 and 4.
- Sole is tiny. One header and one source file.
- Sole is cross-platform. Compiles under MSVC/GCC. Works on Windows/Linux.
- Sole is MIT licensed.

some theory
-----------
- UUID version 1 (48-bit MAC address + 60-bit clock with a resolution of 100 ns)
- UUID Version 4 (122-bits of randomness)
- Use v1 if you are worried about leaving it up to probabilities.
- Use v4 if you are worried about security issues.

public API
----------
- `sole::uuid` 128-bit UUID base type. Allows comparison and sorting. Use `str()` method for pretty printing.
- `sole::uuid1()` creates an UUID v1.
- `sole::uuid4()` creates an UUID v4.

sample
------
```
#include <iostream>
#include "sole.hpp"

int main()
{
    std::cout << "uuid v1 sample: " << sole::uuid1() << std::endl;
    std::cout << "uuid v4 sample: " << sole::uuid4() << std::endl;
    return 0;
}
```

possible output
---------------
```
D:\prj\sole>cl sample.cc sole.cpp
D:\prj\sole>sample.exe
uuid v1 sample: 71f0f271-8323-1735-1dd2-11b2e34d8322
uuid v4 sample: 21dab652-d092-4959-96b0-59d4cc64d611
D:\prj\sole>
```

special notes
-------------
- g++ users: both `-std=c++11` and `-lpthread` may be required when compiling `sole.cpp`
