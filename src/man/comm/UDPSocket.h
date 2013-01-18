/**
 * Encapsulates a udp socket for use in the Northern-Bites Man package.
 * @author Wils Dawson 4/19/2012
 */

#ifndef UDPSocket_H
#define UDPSocket_H

#include <cstring>

// Define structs so we can compile without including socket header.
struct sockaddr;
struct sockaddr_in;

class UDPSocket
{
public:
    /**
     * Constructor.
     */
    UDPSocket();

    /**
     * Destructor.
     */
    ~UDPSocket();

    /**
     * Set the target address.
     * @param ip:   The target ip address of the host system.
     * @param port: The port used for the data transfer.
     * @return:     true for success, false for error.
     */
    bool setTarget(const char* ip, const int port);

    /**
     * Set the target address.
     * @param addr: The address to set as the target
     * @retrun:     true for success, false for error.
     */
    bool setTarget(const struct sockaddr addr);

    /**
     * Set broadcast mode.
     * @param enable: true to enable broadcast, false to disable.
     * @return:       true for success, false for error.
     */
    bool setBroadcast(bool enable);

    /**
     * Set blocking mode for socket.
     * @param enable: true to enable blocking, false to disable.
     * @return:       true for success, false for error.
     */
    bool setBlocking(bool enable);

    /**
     * Set Multicast Time-To-Live (TTL) which corresponds to
     * the number of router hops until the packet dies.
     * @param ttl: The byte-length value for TTL.
     * @return:    true for success, false for error.
     */
    bool setMulticastTTL(const char ttl);

    /**
     * Set receive buffer size. Will set the size of the receiving
     * buffer to specified size to not waste space, or get more.
     * @param size: The size to make the receive buffer.
     * @return:     true for success, false for error.
     */
    bool setRcvBufSize(unsigned int size);

    /**
     * Set Multicast Loopback (receive own multicast packets).
     * @param enable: true to enable loopback, false to disable.
     * @return:       true for success, false for error.
     */
    bool setMulticastLoopback(bool enable);

    /**
     * Subscribe to a multicast address. In our system each robot
     * sends its information to 239.<my.low.bits>, where
     * <my.low.bits> is the low-order three bytes of its IPv4
     * address. Every other robot will join this address.
     * This allows for tools to selectively subscribe to robots.
     * @param addrStr: The mulicast address to join to in string form.
     * @return:        true for success, false for error.
     */
    bool joinMulticast(const char* addrStr);

    /**
     * Bind the socket to the specified address and port.
     * @param addr: IP address to bind the socket to in string form.
     *              If it is the empty string, INADDR_ANY is used.
     * @param port: Port to bind the socket to as an int.
     *              If 0 is specified, the kernel will choose a port.
     * @return:     true for success, false for error.
     */
    bool bind(const char* addrStr, int port);

    /**
     * Receive some datagram in the socket's buffer and gather
     * information about who sent the packet.
     * @param data:    Buffer to which the data will be moved.
     * @param datalen: Number of bytes to read from the socket.
     * @param from:    Address structure which gets information about
     *                 who sent the packet.
     * @param addrlen: Size of the from structure (used by the kernel).
     * @return:        Number of bytes read or -1 if error occured.
     */
    int receiveFrom(char* data, int datalen,
                    struct sockaddr* from = NULL, int* addrlen = NULL);

    /**
     * Receive some datagram in the socket's buffer. Does not
     * gather information about the sender. Uses receiveFrom.
     * @param data: Buffer to which the data will be moved.
     * @param len:  Number of bytes to read from the socket.
     * @return:     Number of bytes read or -1 on error.
     */
    int receive(char* data, int len) {return receiveFrom(data, len);}

    /**
     * Sends data via the socket to target address, which is
     * settable with setTarget(ip, port).
     * @param data: Information to send to the target.
     * @param len:  Number of bytes to send to the target.
     * @return:     Number of bytes written or -1 on error.
     */
    int sendToTarget(const char* data, const int len);

private:
    struct sockaddr* target; // Address struct for sending.
    int sock;                // Socket ID.
    /**
     * Verifies IP address and sets up the addr structure.
     * @param addrStr: IP address in string form to verify.
     * @param port:    Port number to verify.
     * @param addr:    Structure to set up with IP and port info.
     * @return:        true for success, false for error.
     */
    bool resolve(const char* addrStr, int port, struct sockaddr_in* addr);
};

#endif
