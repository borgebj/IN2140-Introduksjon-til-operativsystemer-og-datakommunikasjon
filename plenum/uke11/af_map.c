// #define _POSIX_C_SOURCE
#include <sys/socket.h>

const char* af_map( int value )
{
    switch( value )
    {
        case AF_UNSPEC          : return "AF_UNSPEC";
        case AF_UNIX            : return "AF_UNIX";
        case AF_INET            : return "AF_INET";
        case AF_INET6           : return "AF_INET6";
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
            // case AF_LOCAL           : return "AF_LOCAL";
    case AF_IMPLINK         : return "AF_IMPLINK";
    case AF_PUP             : return "AF_PUP";
    case AF_CHAOS           : return "AF_CHAOS";
    case AF_NS              : return "AF_NS";
    case AF_ISO             : return "AF_ISO";
    // case AF_OSI             : return "AF_OSI";
    case AF_ECMA            : return "AF_ECMA";
    case AF_DATAKIT         : return "AF_DATAKIT";
    case AF_CCITT           : return "AF_CCITT";
    case AF_SNA             : return "AF_SNA";
    case AF_DECnet          : return "AF_DECnet";
    case AF_DLI             : return "AF_DLI";
    case AF_LAT             : return "AF_LAT";
    case AF_HYLINK          : return "AF_HYLINK";
    case AF_APPLETALK       : return "AF_APPLETALK";
    case AF_ROUTE           : return "AF_ROUTE";
    case AF_LINK            : return "AF_LINK";
    case pseudo_AF_XTP      : return "pseudo_AF_XTP";
    case AF_COIP            : return "AF_COIP";
    case AF_CNT             : return "AF_CNT";
    case pseudo_AF_RTIP     : return "pseudo_AF_RTIP";
    case AF_IPX             : return "AF_IPX";
    case AF_SIP             : return "AF_SIP";
    case pseudo_AF_PIP      : return "pseudo_AF_PIP";
    case AF_NDRV            : return "AF_NDRV";
    case AF_ISDN            : return "AF_ISDN";
    // case AF_E164            : return "AF_E164";
    case pseudo_AF_KEY      : return "pseudo_AF_KEY";
    case AF_NATM            : return "AF_NATM";
    case AF_SYSTEM          : return "AF_SYSTEM";
    case AF_NETBIOS         : return "AF_NETBIOS";
    case AF_PPP             : return "AF_PPP";
    case pseudo_AF_HDRCMPLT : return "pseudo_AF_HDRCMPLT";
    case AF_RESERVED_36     : return "AF_RESERVED_36";
    case AF_IEEE80211       : return "AF_IEEE80211";
    case AF_UTUN            : return "AF_UTUN";
    case AF_VSOCK           : return "AF_VSOCK";
    case AF_MAX             : return "AF_MAX";
#endif  /* (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */
    }
    return "undefined";
}