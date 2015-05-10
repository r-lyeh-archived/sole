/* Sole is a lightweight C++11 library to generate universally unique identificators.
 * Sole provides interface for UUID versions 1 and 4. Custom v0 is provided additionally.
 * Copyright (c) 2013, Mario 'rlyeh' Rodriguez, zlib/libpng licensed.

 * Based on code by Dmitri Bouianov, Philip O'Toole, Poco C++ libraries and
 * anonymous code found on the net. Thanks guys!

 * Theory: (see Hoylen's answer at [1])
 * - UUID version 1 (48-bit MAC address + 60-bit clock with a resolution of 100ns)
 *   Clock wraps in 3603 A.D.
 *   Up to 10000000 UUIDs per second.
 *   MAC address revealed.
 *
 * - UUID Version 4 (122-bits of randomness)
 *   See [2] or other analysis that describe how very unlikely a duplicate is.
 *
 * - Use v1 if you need to sort or classify UUIDs per machine.
 *   Use v1 if you are worried about leaving it up to probabilities (e.g. your are the
 *   type of person worried about the earth getting destroyed by a large asteroid in your
 *   lifetime). Just use a v1 and it is guaranteed to be unique till 3603 AD.
 *
 * - Use v4 if you are worried about security issues and determinism. That is because
 *   v1 UUIDs reveal the MAC address of the machine it was generated on and they can be
 *   predictable. Use v4 if you need more than 10 million uuids per second, or if your
 *   application wants to live past 3603 A.D.

 * Additionally a custom UUID v0 is provided:
 * - 16-bit PID + 48-bit MAC address + 60-bit clock with a resolution of 100ns since Unix epoch
 * - Format is EPOCH_LOW-EPOCH_MID-VERSION(0)|EPOCH_HI-PID-MAC
 * - Clock wraps in 3991 A.D.
 * - Up to 10000000 UUIDs per second.
 * - MAC address and PID revealed.

 * References:
 * - [1] http://stackoverflow.com/questions/1155008/how-unique-is-uuid
 * - [2] http://en.wikipedia.org/wiki/UUID#Random%5FUUID%5Fprobability%5Fof%5Fduplicates
 * - http://en.wikipedia.org/wiki/Universally_unique_identifier
 * - http://en.cppreference.com/w/cpp/numeric/random/random_device
 * - http://www.itu.int/ITU-T/asn1/uuid.html f81d4fae-7dec-11d0-a765-00a0c91e6bf6

 * - rlyeh ~~ listening to Hedon Cries / Until The Sun Goes up
 */

//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <stdio.h>     // for size_t; should be stddef.h instead; however, clang+archlinux fails on compiling it (@Travis-Ci)
#include <stdint.h>
#include <functional>
#include <string>

namespace sole
{
    // 128-bit basic UUID type that allows comparison and sorting.
    // Use .str() for printing and .pretty() for pretty printing.
    // Also, ostream friendly.
    struct uuid
    {
        uint64_t ab;
        uint64_t cd;

        bool operator==( const uuid &other ) const;
        bool operator!=( const uuid &other ) const;
        bool operator <( const uuid &other ) const;

        std::string pretty() const;
        std::string base62() const;
        std::string str() const;

        template<typename ostream>
        inline friend ostream &operator<<( ostream &os, const uuid &self ) {
            return os << self.str(), os;
        }
    };

    // Generators
    uuid uuid0(); // UUID v0, pro: unique; cons: MAC revealed, pid revealed, predictable.
    uuid uuid1(); // UUID v1, pro: unique; cons: MAC revealed, predictable.
    uuid uuid4(); // UUID v4, pros: anonymous, fast; con: uuids "can clash"

    // Rebuilders
    uuid rebuild( uint64_t ab, uint64_t cd );
    uuid rebuild( const std::string &uustr );
}

namespace std {
    template<>
    class hash< sole::uuid > : public std::unary_function< sole::uuid, size_t > {
    public:
        // hash functor: hash uuid to size_t value by pseudorandomizing transform
        size_t operator()( const sole::uuid &uuid ) const {
            if( sizeof(size_t) > 4 ) {
                return size_t( uuid.ab ^ uuid.cd );
            } else {
                uint64_t hash64 = uuid.ab ^ uuid.cd;
                return size_t( uint32_t( hash64 >> 32 ) ^ uint32_t( hash64 ) );
            }
        }
    };
}
