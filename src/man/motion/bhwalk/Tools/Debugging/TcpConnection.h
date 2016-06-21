/**
* @file Tools/Debugging/TcpConnection.h
*
* Declaration of class TcpConnection.
*
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

#include "Platform/TcpComm.h"

#define MAX_PACKAGE_SIZE    67108864      // max package size that can be received. prevent from allocating to much buffer (max ~64 MB)

/**
* @class TcpConnection
* The class implements a tcp connection.
*/
class TcpConnection
{
public:
  enum Handshake
  {
    noHandshake,
    sender,
    receiver
  };

private:
  TcpComm* tcpComm; /**< The TCP/IP connection. */
  bool ack,
       client;
  Handshake handshake; /**< The handshake mode. */

  /**
  * The function tries to receive a package.
  * @param buffer A pointer that will be initialized with the address of
  *               the data received. It is valid and has to be freed only
  *               if the function returns a value larger than 0.
  * @return Success of the function: -1: failure, 0: nothing read,
  *         > 0: success, size of data, and buffer points to data.
  */
  int receive(unsigned char*& buffer);

public:
  /**
  * Default constructor.
  */
  TcpConnection() : tcpComm(0), client(false) {}

  /**
  * Constructor.
  * The constructor will first try to connect another process as
  * a client. If this does not work, it will open the port as a
  * server.
  * @param ip The ip address of the communication partner. If 0, the port
  *           will be opened as server.
  * @param port The port under which will be communicated.
  * @param handshake The handshake mode.
  * @param maxPackageSendSize The maximum size of an outgoing package.
  *                           If 0, this setting is ignored.
  * @param maxPackageReceiveSize The maximum size of an incouming package.
  *                              If 0, this setting is ignored.
  */
  TcpConnection(const char* ip, int port, Handshake handshake = noHandshake,
                int maxPackageSendSize = 0, int maxPackageReceiveSize = 0)
  {connect(ip, port, handshake, maxPackageSendSize, maxPackageReceiveSize);}


  /**
  * Destructor.
  */
  ~TcpConnection() {if(tcpComm) delete tcpComm;}

  /**
  * The function will first try to connect another process as
  * a client. If this does not work, it will open the port as a
  * server.
  * @param ip The ip address of the communication partner.
  * @param port The port under which will be communicated.
  * @param handshake The handshake mode.
  * @param maxPackageSendSize The maximum size of packages to send.
  * @param maxPackageReceiveSize The maximum size of packages to receive.
  */
  void connect(const char* ip, int port, Handshake handshake = noHandshake,
               int maxPackageSendSize = 0, int maxPackageReceiveSize = 0);

  /**
  * The function sends and receives data.
  * @param dataToSend The data to be send. The function will not free the buffer.
  * @param sendSize The size of data to send. If 0, no data is sent.
  * @param dataRead If data has been read, the parameter is initialzed with
  *                 the address of a buffer pointing to that data. The
  *                 buffer has to be freed manually.
  * @param readSize The size of the block read. "dataRead" is only valid
  *                 (and has to be freed) if this parameter contains a
  *                 positive number after the call to the function.
  * @return Returns true if the data has been sent.
  */
  bool sendAndReceive(const unsigned char* dataToSend, int sendSize, unsigned char*& dataRead, int& readSize);

  /**
  * The function states whether the connection is still established.
  * @return Does the connection still exist?
  */
  bool isConnected() const {return tcpComm && tcpComm->connected();}

  /**
  * The function states whether this system is the client in the connection.
  * @return Is it the client?
  */
  bool isClient() const {return client;}

  /**
  * The function returns the overall number of bytes sent so far by this object.
  * @return The number of bytes sent since this object was created.
  */
  int getOverallBytesSent() const {return tcpComm ? tcpComm->getOverallBytesSent() : 0;}

  /**
  * The function returns the overall number of bytes received so far by this object.
  * @return The number of bytes received since this object was created.
  */
  int getOverallBytesReceived() const {return tcpComm ? tcpComm->getOverallBytesReceived() : 0;}

  /**
  * The functions sends a heartbeart.
  * @return Was the heartbeat successfully sent?
  */
  bool sendHeartbeat();
};
