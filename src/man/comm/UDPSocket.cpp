/**
 * Encapsulates a udp socket for use in the Northern-Bites Man package.
 * @author Wils Dawson 4/19/2012
 */

#include "UDPSocket.h"

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <ifaddrs.h>
#include <cstdio>

#include "DebugConfig.h"

namespace man {

namespace comm {

UDPSocket::UDPSocket()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1)
        std::cerr << "\nError Constructing UDPSocket!" << std::endl;

    target = (struct sockaddr*)(new struct sockaddr_in);
}

UDPSocket::~UDPSocket()
{
    close(sock);
    delete target;
}

bool UDPSocket::resolve(const char* addrStr, int port, struct sockaddr_in* addr)
{
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr -> sin_family = AF_INET;
    addr -> sin_port   = htons((uint16_t)port);

    // Fill in addr->sin_addr at the same time as checking validity!
    if (1 != inet_pton(AF_INET, addrStr, &(addr->sin_addr.s_addr)))
    {
        std::cerr << addrStr << " is not a valid dotted IPv4 address" << std::endl;
        return false;
    }
    return true;
}

bool UDPSocket::setTarget(const char* ip, const int port)
{
    struct sockaddr_in* addr = (struct sockaddr_in*)target;
    return resolve(ip, port, addr);
}

bool UDPSocket::setTarget(const struct sockaddr addr)
{
    return memcpy(target, &addr, sizeof(target));
}

bool UDPSocket::setBroadcast(bool enable)
{
    int yes = enable ? 1 : 0;
    if (0 == setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                        (const char *) &yes, sizeof(yes)))
    {
        return true;
    }
    std::cerr << "UDPSocket::setBroadcast() failed: " << strerror(errno)
              << std::endl;
    return false;
}

bool UDPSocket::setBlocking(bool enable)
{
    bool result = false;
    int flags;
    if ((flags = fcntl(sock, F_GETFL, 0)) < 0)
    {
        std::cerr << "UDPSocket::setBlocking() error in F_GETFL"
                  << std::endl;
        return false;
    }

    if (enable)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (-1 != fcntl(sock, F_SETFL, flags))
        result = true;

    return result;
}

bool UDPSocket::setMulticastTTL(const char ttl)
{
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,
                   &ttl, sizeof(unsigned char)        ) < 0)
    {
        std::cerr << "Unable to set Multicast TTL to "
                  << ttl << std::endl;
        return false;
    }
    return true;
}

bool UDPSocket::setRcvBufSize(unsigned int size)
{
    if (0 != setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                        (char*) &size, sizeof(size)))
    {
        std::cerr << "UDPSocket::setRcvBufSize() failed to set "
                  << size << " bytes. Error: " << strerror(errno) << std::endl;
        return false;
    }

    // Ensure we set the Buffer correctly
    int result;
    socklen_t result_len = sizeof(result);
    if (0 == getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*) &result, &result_len))
    {
#ifdef DEBUG_COMM
        std::cout << "UDPSocket: recieve buffer set to "
                  << result << " bytes." << std::endl;
#endif
        return true;
    }

    std::cerr << "UDPSocket::setRcvBufSize() could not verify SO_RCVBUF."
              << std::endl;
    return false;
}

bool UDPSocket::setMulticastInterface()
{
    struct in_addr lcl_iface;
    struct ifconf  ifc;
    struct ifreq*  item;
    char buf[1024];

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        std::cerr << "Cannot get interface list" << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < ifc.ifc_len / sizeof(struct ifreq); ++i)
    {
        item = &ifc.ifc_req[i];
        if (!strcmp(item->ifr_name, "wlan0"))
        {
            lcl_iface = ((struct sockaddr_in *)&item->ifr_addr)->sin_addr;
            if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF,
                           (char *)&lcl_iface,
                           sizeof(lcl_iface)) < 0)
            {
                std::cerr << "Could not set outgoing multicast interface"
                          << std::endl;
                return false;
            }
            else
            {
#ifdef DEBUG_COMM
                char outgoing_ip_address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(lcl_iface.s_addr),
                          outgoing_ip_address, INET_ADDRSTRLEN);
                std::cout << "Set outgoing multicast interface to "
                          << item->ifr_name << " with address "
                          << outgoing_ip_address << std::endl;
#endif
                return true;
            }
        }
    }
    std::cerr << "Could not find any interfaces to send on" << std::endl;
    return false;
}

bool UDPSocket::setMulticastLoopback(bool enable)
{
    char val = enable ? 1 : 0;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(char)) < 0)
    {
        std::cerr << "Could not set IP_MULTICAST_LOOP to " << val << std::endl;
        return false;
    }
    return true;
}

bool UDPSocket::joinMulticast(const char* addrStr)
{
    struct sockaddr_in group;
    if (!resolve(addrStr, 0, &group))
        return false;

    // Ensure address is a valid Multicast group address.
    if (IN_MULTICAST(ntohl(group.sin_addr.s_addr)))
    {
        struct ip_mreq mreq;
        struct ifconf  ifc;
        struct ifreq*  item;
        char buf[1024];

        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
        {
            std::cerr << "Cannot get interface list" << std::endl;
            return false;
        }

        bool couldJoin = false;
        mreq.imr_multiaddr = group.sin_addr;
        for (unsigned int i = 0; i < ifc.ifc_len / sizeof(struct ifreq); ++i)
        {
            item = &ifc.ifc_req[i];

            if (!strcmp(item->ifr_name, "wlan0"))
            {
#ifdef DEBUG_COMM
                std::cout << "Trying to join multicast group " << addrStr
                          << " on interface " << item->ifr_name << std::endl;
#endif
                mreq.imr_interface = ((struct sockaddr_in *)&item->ifr_addr)->sin_addr;
                if (0 == setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                                    (void*)&mreq, sizeof(mreq)))
                {
                    couldJoin = true;
                }
            }
        }

        if (!couldJoin)
        {
            std::cerr << "Failed to join multicast group "
                      << addrStr << std::endl;
            return false;
        }
        else
        {
#ifdef DEBUG_COMM
            std::cout << "Joined Multicast Address: "
                      << addrStr << std::endl;
#endif
        }
        return true;
    }

    else
        std::cerr << addrStr << " is not a multicast address." << std::endl;
    return false;
}

bool UDPSocket::bind(const char* addrStr = "", int port = 0)
{
    static const int one = 1;
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((uint16_t)port);
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, addrStr, &(addr.sin_addr)) )
    {
        std::cerr << "UDPSocket::bind() failed: invalid address "
                  << addrStr << std::endl;
        return false;
    }

#ifdef SO_REUSEADDR
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                         (const char*)&one, sizeof(one)))
        std::cerr << "UDPSocket: could not set SO_REUSEADDR" << std::endl;
#endif
#ifdef SO_REUSEPORT
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
                         (const char*)&one, sizeof(one)))
        std::cerr << "UDPSocket: could not set SO_REUSEPORT" << std::endl;
#endif
    if (-1 == ::bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) )
    {
        std::cerr << "UDPSocket::bind() failed: " << strerror(errno) << std::endl;
        return false;
    }
#ifdef DEBUG_COMM
    char bound_ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr),
              bound_ip_address, INET_ADDRSTRLEN);
    std::cout << "Socket bound to " << bound_ip_address << std::endl;
#endif
    return true;
}

int UDPSocket::receiveFrom(char* data, int datalen,
                struct sockaddr* from, int* addrlen)
{
    ssize_t nread;

    nread = ::recvfrom(sock, data, datalen, 0, from, (socklen_t *) addrlen);
    if (nread < 0)
    {
        // If the error is a blocking error, i.e. there is no data to read:
        if (errno == EAGAIN)
            return 0;
        else
        {
            std::cerr << "UDPSocket::receiveFrom() failed: "
                      << strerror(errno) << std::endl;
        }
    }
    return nread;
}

int UDPSocket::sendToTarget(const char* data, const int len)
{
    ssize_t nwritten;

    nwritten = ::sendto(sock, data, len, 0, target, sizeof(*target));

    if (nwritten < 0)
    {
        std::cerr << "UDPSocket::sendToTarget() failed: "
                  << strerror(errno) << std::endl;
    }
    else
    {
#ifdef DEBUG_COMM
        std::cout << "Sent a packet" << std::endl;
#endif
    }

    return nwritten;
}

}

}
