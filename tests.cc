// g++ users: `g++ tests.cc sole.cpp -std=c++11 -lrt -o tests`
// visual studio: `cl.exe tests.cc sole.cpp`

#include <cassert>
#include <set>
#include <ratio>
#include <chrono>
#include <iostream>

#include "sole.hpp"
using namespace sole;

namespace run
{
    auto epoch = [](){
        return std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    };

    template<typename FN>
    void benchmark( const FN &fn, const std::string &name ) {
        std::cout << "Benchmarking " << name << "... " << std::flush;

        auto then = epoch();

        while( epoch() == then );
        then = epoch();

        unsigned c = 0;
        while( epoch() == then ) c = ( fn(), ++c );

        std::cout << (c) << " uuids/sec" << std::endl;
    }

    template<typename FN>
    void tests( const FN &fn ) {
        unsigned numtests = ~0;
        std::cout << "Testing for " << numtests << " collisions... " << std::endl;

        auto then = epoch();

        std::set<uuid> all;
        for( unsigned i = 0; i < numtests; ++i ) {
            auto now = epoch();
            if( now != then ) {
                then = now;
                double pct6digs = ( int( ( double(i) / (unsigned)(~0) ) * 1e4 ) / double(1e4) );
                std::cout << '\r' << i << " uuids generated, no collision (" << pct6digs << "%)" << std::flush;
            }
            sole::uuid my_uuid = fn();
            assert( all.find(my_uuid) == all.end() && "error: UUIDs just collided! is std::random_device a real random generator?" );
            all.insert( my_uuid );
        }
    }

    template<typename FN>
    void verify( const FN &fn ) {
        std::cout << "Verifying serialization of 1 million UUIDs... " << std::flush;

        for( unsigned i = 0; i < 1000000; ++i ) {
            sole::uuid uuid = fn();
            sole::uuid rebuilt1 = sole::rebuild( uuid.str() );
            sole::uuid rebuilt2 = sole::rebuild( uuid.base62() );
            assert( rebuilt1 == uuid && "error: rebuild() or .str() failed" );
            assert( rebuilt2 == uuid && "error: rebuild() or .base62() failed" );
        }

        std::cout << "ok" << std::endl;
    }
}

int main()
{
    assert( sizeof(sole::uuid      )  * 8 == 128 );
    assert( sizeof(sole::uuid0().ab)  * 8 ==  64 );
    assert( sizeof(sole::uuid0().cd)  * 8 ==  64 );

    run::benchmark(uuid0, "v0");
    run::benchmark(uuid1, "v1");
    run::benchmark(uuid4, "v4");

    run::verify(uuid4);             // use fastest implementation

//  run::tests(uuid0);              // not applicable
//  run::tests(uuid1);              // not applicable
    run::tests(uuid4);

    return 0;
}
