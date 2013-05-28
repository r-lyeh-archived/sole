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
-----------------------
- UUID version 0 (16-bit PID + 48-bit MAC address + 60-bit clock with a resolution of 100ns since Unix epoch)
- Format is EPOCH_LOW-EPOCH_MID-VERSION(0)|EPOCH_HI-PID-MAC

public API
----------
- `sole::uuid` 128-bit UUID base type that allows comparison and sorting. `std::ostream` friendly.
- `sole::uuid0()` creates an UUID v0.
- `sole::uuid1()` creates an UUID v1.
- `sole::uuid4()` creates an UUID v4.

sample
------
```
#include <iostream>
#include "sole.hpp"

int main()
{
    std::cout << "uuid v0 sample: " << sole::uuid0() << std::endl;
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
uuid v0 sample: cc31727b-a9e5-0030-9014-00ffb77bd832
uuid v1 sample: dfb76d93-c7b7-11e2-ad93-00ffb77bd832
uuid v4 sample: 2fb9a183-f0c1-43b6-aa59-3122c2eaf9b7
D:\prj\sole>
```

special notes
-------------
- g++ users: both `-std=c++11` and `-lrt` may be required when compiling `sole.cpp`
