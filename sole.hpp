#pragma once

#include <string>
#include <stdint.h>
#include <iostream>

namespace sole
{
    // 128-bit basic UUID type that allows comparison and sorting.
    // Use .str() for pretty printing.
    union uuid
    {
        struct {
            uint32_t a,b,c,d;
        };
        struct {
            uint64_t aa, bb;
        };
        uint32_t blob32[4];
        uint64_t blob64[2];

        bool operator==( const uuid &other ) const;
        bool operator!=( const uuid &other ) const;
        bool operator <( const uuid &other ) const;

        std::string str() const;
    };

    // Generators
    uuid uuid1(); // UUID v1, pro: unique; cons: MAC revealed, predictable.
    uuid uuid4(); // UUID v4, pros: anonymous, fast; con: uuids "can clash"
}

// ostream friendly
std::ostream &operator<<( std::ostream &os, const sole::uuid &u );

