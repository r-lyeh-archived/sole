/* Sole is a lightweight C++11 library to generate universally unique identificators.
 * Sole provides interface for UUID versions 1 and 4. Custom v0 is provided additionally.
 * Copyright (c) 2013, Mario 'rlyeh' Rodriguez

 * Based on code by Dmitri Bouianov, Philip O'Toole, Poco C++ libraries and
 * anonymous code found on the net. Thanks guys!

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

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

 * - rlyeh ~~ listening to Hedon Cries / Until The Sun Goes up
 */

//////////////////////////////////////////////////////////////////////////////////////
// UUID v1 headers mostly

#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <cstring>

#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#   include <process.h>
#   include <iphlpapi.h>
#   pragma comment(lib,"iphlpapi.lib")
#   define $windows $yes
#elif defined(__FreeBSD__) || defined(__NetBSD__) || \
        defined(__OpenBSD__) || defined(__MINT__) || defined(__bsdi__)
#   include <ifaddrs.h>
#   include <net/if_dl.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <sys/types.h>
#   define $bsd $yes
#elif defined(__linux__)
#   include <arpa/inet.h>
#   include <net/if.h>
#   include <netinet/in.h>
#   include <sys/ioctl.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <unistd.h>
#   define $linux $yes
#else //elif defined(__unix__)
#   if defined(__VMS)
#      include <ioctl.h>
#      include <inet.h>
#   else
#      include <sys/ioctl.h>
#      include <arpa/inet.h>
#   endif
#   if defined(sun) || defined(__sun)
#      include <sys/sockio.h>
#   endif
#   include <net/if.h>
#   include <net/if_arp.h>
#   include <netdb.h>
#   include <netinet/in.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <sys/types.h>
#   include <unistd.h>
#   if defined(__VMS)
        namespace { enum { MAXHOSTNAMELEN = 64 }; }
#   endif
#   define $unix $yes
#endif

////////////////////////////////////////////////////////////////////////////////////

#include "sole.hpp"

#ifdef  $windows
#define $welse   $no
#else
#define $windows $no
#define $welse   $yes
#endif

#ifdef  $bsd
#define $belse   $no
#else
#define $bsd     $no
#define $belse   $yes
#endif

#ifdef  $linux
#define $lelse   $no
#else
#define $linux   $no
#define $lelse   $yes
#endif

#ifdef  $unix
#define $uelse   $no
#else
#define $unix    $no
#define $uelse   $yes
#endif

#define $yes(...) __VA_ARGS__
#define $no(...)

bool sole::uuid::operator==( const sole::uuid &other ) const {
    return ab == other.ab && cd == other.cd;
}
bool sole::uuid::operator!=( const sole::uuid &other ) const {
    return !operator==(other);
}
bool sole::uuid::operator<( const sole::uuid &other ) const {
    if( ab < other.ab ) return true;
    if( ab > other.ab ) return false;
    if( cd < other.cd ) return true;
    return false;
}

std::string sole::uuid::str() const {
    std::stringstream ss;
    ss << std::hex << std::nouppercase << std::setfill('0');

    uint32_t a = (ab >> 32);
    uint32_t b = (ab & 0xFFFFFFFF);
    uint32_t c = (cd >> 32);
    uint32_t d = (cd & 0xFFFFFFFF);

    ss << std::setw(8) << (a) << '-';
    ss << std::setw(4) << (b >> 16) << '-';
    ss << std::setw(4) << (b & 0xFFFF) << '-';
    ss << std::setw(4) << (c >> 16 ) << '-';
    ss << std::setw(4) << (c & 0xFFFF);
    ss << std::setw(8) << d;

    return ss.str();
}

std::ostream &operator<<( std::ostream &os, const sole::uuid &u ) {
    return os << u.str(), os;
}

//////////////////////////////////////////////////////////////////////////////////////
// UUID v4 impl

namespace sole {

uuid uuid4() {

    std::random_device rd;
    std::uniform_int_distribution<uint64_t> dist(0, (uint64_t)(~0));

    uuid my;

    my.ab = dist(rd);
    my.cd = dist(rd);

    my.ab = (my.ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    my.cd = (my.cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    return my;
}

} // ::sole

//////////////////////////////////////////////////////////////////////////////////////
// UUID v1 / multiplatform gettimeofday()

namespace {
$windows(
    struct timespec {
        uint64_t tv_sec;
        uint64_t tv_nsec;
    };
    struct timezone {
        int  tz_minuteswest; /* minutes W of Greenwich */
        int  tz_dsttime;     /* type of dst correction */
    };
    int gettimeofday( struct timeval *tv, struct timezone *tz ) {
        FILETIME ft;
        uint64_t tmpres = 0;

        if( NULL != tv ) {
            GetSystemTimeAsFileTime(&ft);

            // The GetSystemTimeAsFileTime returns the number of 100 nanosecond
            // intervals since Jan 1, 1601 in a structure. Copy the high bits to
            // the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
            tmpres |= ft.dwHighDateTime;
            tmpres <<= 32;
            tmpres |= ft.dwLowDateTime;

            // Convert to microseconds by dividing by 10
            tmpres /= 10;

            // The Unix epoch starts on Jan 1 1970.  Need to subtract the difference
            // in seconds from Jan 1 1601.
            tmpres -= 11644473600000000ULL;

            // Finally change microseconds to seconds and place in the seconds value.
            // The modulus picks up the microseconds.
            tv->tv_sec = (tmpres / 1000000UL);
            tv->tv_usec = (tmpres % 1000000UL);
        }

        if( NULL != tz ) {
            static bool once = true;
            if( once ) {
                once = false;
                _tzset();
            }
            tz->tz_minuteswest = _timezone / 60;
            tz->tz_dsttime = _daylight;
        }

        return 0;
    }
)
$lelse( $belse( // if not linux, if not bsd... valid for apple/win32
    int clock_gettime( int /*clk_id*/, struct timespec* t ) {
        struct timeval now;
        int rv = gettimeofday(&now, NULL);
        if( rv ) return rv;
        t->tv_sec  = now.tv_sec;
        t->tv_nsec = now.tv_usec * 1000;
        return 0;
    }
))
}

namespace {

// Returns number of 100ns intervals
uint64_t gettime( uint64_t offset )
{
    struct timespec tp;
    clock_gettime(0 /*CLOCK_REALTIME*/, &tp);

    // Convert to 100-nanosecond intervals
    uint64_t uuid_time;
    uuid_time = tp.tv_sec * 10000000;
    uuid_time = uuid_time + (tp.tv_nsec / 100);
    uuid_time = uuid_time + offset;

    // If the clock looks like it went backwards, or is the same, increment it.
    static uint64_t last_uuid_time = 0;
    if( last_uuid_time > uuid_time )
        last_uuid_time = uuid_time;
    else
        last_uuid_time = ++uuid_time;

    return uuid_time;
}

// Looks for first MAC address of any network device, any size.
bool get_any_mac( std::vector<unsigned char> &_node )
{
$windows({
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = 0;
    ULONG len    = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[len]);

    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into len
    DWORD rc = GetAdaptersInfo(pAdapterInfo, &len);
    if (rc == ERROR_BUFFER_OVERFLOW)
    {
        delete [] reinterpret_cast<char*>(pAdapterInfo);
        pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[len]);
    }
    else if (rc != ERROR_SUCCESS)
    {
        return ("cannot get network adapter list"), false;
    }

    bool found = false, gotten = false;
    if (GetAdaptersInfo(pAdapterInfo, &len) == NO_ERROR)
    {
        gotten = true;

        pAdapter = pAdapterInfo;
        while (pAdapter && !found)
        {
            if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && pAdapter->AddressLength > 0 )
            {
                _node.resize( pAdapter->AddressLength );
                std::memcpy(_node.data(), pAdapter->Address, _node.size() );
                found = true;
            }
            pAdapter = pAdapter->Next;
        }
    }

    delete [] reinterpret_cast<char*>(pAdapterInfo);

    if( !gotten )
        return ("cannot get network adapter list"), false;

    if (!found)
        return ("no Ethernet adapter found"), false;

    return true;
})

$bsd({
    struct ifaddrs* ifaphead;
    int rc = getifaddrs(&ifaphead);
    if (rc) return ("cannot get network adapter list"), false;

    bool foundAdapter = false;
    for (struct ifaddrs* ifap = ifaphead; ifap; ifap = ifap->ifa_next)
    {
        if (ifap->ifa_addr && ifap->ifa_addr->sa_family == AF_LINK)
        {
            struct sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifap->ifa_addr);
            caddr_t ap = (caddr_t) (sdl->sdl_data + sdl->sdl_nlen);
            int alen = sdl->sdl_alen;
            if (ap && alen > 0)
            {
                _node.resize( alen );
                std::memcpy(_node.data(), ap, _node.size() );
                foundAdapter = true;
                break;
            }
        }
    }
    freeifaddrs(ifaphead);
    if (!foundAdapter) return ("cannot determine MAC address (no suitable network adapter found)"), false;
    return true;
})

$linux({
    struct ifreq ifr;

    int s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s == -1) return ("cannot open socket"), false;

    std::strcpy(ifr.ifr_name, "eth0");
    int rc = ioctl(s, SIOCGIFHWADDR, &ifr);
    close(s);
    if (rc < 0) return ("cannot get MAC address"), false;
    struct sockaddr* sa = reinterpret_cast<struct sockaddr*>(&ifr.ifr_addr);
    _node.resize( sizeof(sa->sa_data) );
    std::memcpy(_node.data(), sa->sa_data, _node.size() );
    return true;
})

$unix({
    char name[MAXHOSTNAMELEN];
    if (gethostname(name, sizeof(name)))
        return ("cannot get host name"), false;

    struct hostent* pHost = gethostbyname(name);
    if (!pHost) return ("cannot get host IP address"), false;

    int s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == -1) return ("cannot open socket"), false;

    struct arpreq ar;
    std::memset(&ar, 0, sizeof(ar));
    struct sockaddr_in* pAddr = reinterpret_cast<struct sockaddr_in*>(&ar.arp_pa);
    pAddr->sin_family = AF_INET;
    std::memcpy(&pAddr->sin_addr, *pHost->h_addr_list, sizeof(struct in_addr));
    int rc = ioctl(s, SIOCGARP, &ar);
    close(s);
    if (rc < 0) return ("cannot get MAC address"), false;
    _node.resize( sizeof(ar.arp_ha.sa_data) );
    std::memcpy(_node.data(), ar.arp_ha.sa_data, _node.size());
    return true;
})
}

// Looks for first MAC address of any network device, size truncated to 48bits.
uint64_t get_any_mac48() {
    std::vector<unsigned char> node;
    if( !get_any_mac(node) )
        return 0;
    node.resize(6);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for( unsigned i = 0; i < 6; ++i )
        ss << std::setw(2) << int(node[i]);
    uint64_t t;
    if( ss >> t )
        return t;
    return 0;
}

} // namespace ::anon

namespace sole {

uuid uuid1()
{
    // Number of 100-ns intervals since 00:00:00.00 15 October 1582; [ref] uuid.py
    uint64_t ns100_intervals = gettime( 0x01b21dd213814000ULL );
    uint16_t clock_seq = (uint16_t)( ns100_intervals & 0x3fff );  // 14-bits max
    uint64_t mac = get_any_mac48();                               // 48-bits max

    uint32_t time_low = ns100_intervals & 0xffffffff;
    uint16_t time_mid = (ns100_intervals >> 32) & 0xffff;
    uint16_t time_hi_version = (ns100_intervals >> 48) & 0xfff;
    uint8_t clock_seq_low = clock_seq & 0xff;
    uint8_t clock_seq_hi_variant = (clock_seq >> 8) & 0x3f;

    uuid u;
    uint64_t &upper_ = u.ab;
    uint64_t &lower_ = u.cd;

    // Build the high 4 bytes.
    upper_  = (uint64_t) time_low << 32;
    upper_ |= (uint64_t) time_mid << 16;
    upper_ |= (uint64_t) time_hi_version;

    // Build the low 4 bytes, using the clock sequence number.
    lower_  = (uint64_t) ((clock_seq_hi_variant << 8) | clock_seq_low) << 48;
    lower_ |= mac;

    // Set the variant to RFC 4122.
    lower_ &= ~((uint64_t)0xc000 << 48);
    lower_ |=   (uint64_t)0x8000 << 48;

    // Set the version number.
    enum { version = 1 };
    upper_ &= ~0xf000;
    upper_ |= version << 12;

    return u;
}

uuid uuid0()
{
    // Number of 100-ns intervals since Unix epoch time
    uint64_t ns100_intervals = gettime( 0 );
    uint64_t pid = $windows( _getpid() ) $welse( getpid() );
    uint16_t pid_seq = (uint16_t)( pid & 0xffff ); // 16-bits max
    uint64_t mac = get_any_mac48();                // 48-bits max

    uint32_t time_low = ns100_intervals & 0xffffffff;
    uint16_t time_mid = (ns100_intervals >> 32) & 0xffff;
    uint16_t time_hi_version = (ns100_intervals >> 48) & 0xfff;
    uint8_t pid_low = pid_seq & 0xff;
    uint8_t pid_hi = (pid_seq >> 8) & 0xff;

    uuid u;
    uint64_t &upper_ = u.ab;
    uint64_t &lower_ = u.cd;

    // Build the high 4 bytes.
    upper_  = (uint64_t) time_low << 32;
    upper_ |= (uint64_t) time_mid << 16;
    upper_ |= (uint64_t) time_hi_version;

    // Build the low 4 bytes, using the pid number.
    lower_  = (uint64_t) ((pid_hi << 8) | pid_low) << 48;
    lower_ |= mac;

    // Set the variant to RFC 4122.
    lower_ &= ~((uint64_t)0xc000 << 48);
    lower_ |=   (uint64_t)0x8000 << 48;

    // Set the version number.
    enum { version = 0 };
    upper_ &= ~0xf000;
    upper_ |= version << 12;

    return u;
}

} // ::sole

#undef $bsd
#undef $belse
#undef $linux
#undef $lelse
#undef $unix
#undef $uelse
#undef $windows
#undef $welse
#undef $yes
#undef $no
