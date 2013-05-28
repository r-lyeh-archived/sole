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
    void benchmark( const FN &fn ) {
        std::cout << "Benchmarking... " << std::flush;

        auto then = epoch();

        while( epoch() == then );
        then = epoch();

        unsigned c = 0;
        while( epoch() == then ) c = ( fn(), ++c );

        std::cout << (c) << " uuids/sec" << std::endl;
    }

    template<typename FN>
    void tests( const FN &fn ) {
        std::cout << "Testing... " << std::endl;

        auto then = epoch();

        std::set<uuid> all;
        for( unsigned i = 0; i < (unsigned)(~0); ++i ) {
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
}

int main()
{
    run::benchmark(uuid0);
    run::benchmark(uuid1);
    run::benchmark(uuid4);

//  run::tests(uuid0);              //  not applicable
//  run::tests(uuid1);              //  not applicable
    run::tests(uuid4);

    return 0;
}
