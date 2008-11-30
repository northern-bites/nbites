
#ifndef Comm_H
#define Comm_H

#if ROBOT(NAO)
// Comment out the ENTIRE FILE if we are compiling for the Aibo


#include <vector>
#include <string>
#include <pthread.h>
#include <arpa/inet.h>  // inet_aton(), htonl()
#include <boost/shared_ptr.hpp>

#include "Common.h"
#include "corpus/synchro.h"
#include "Sensors.h"
#include "CommDef.h"
#include "RoboCupGameControlData.h"
#include "GameController.h"
#include "TOOLConnect.h"
#include "Vision.h"
#include "CommTimer.h"


class Comm
  : public Thread
{
  public:
    Comm(boost::shared_ptr<Synchro> _synchro, Sensors *s, Vision* v);
    ~Comm();

    void run();
    int  startTOOL();
    void stopTOOL();

    void error(socket_error err) throw();

    void discover_broadcast();

    GameController* getGC() { return &gc; }

    void send(const char *msg, int len, sockaddr_in &addr) throw(socket_error);

    int getTOOLState();
    std::string getRobotName();
    std::list<std::vector<float> >* latestComm();
    void setData(std::vector<float> &data);

  private:
    void bind()                 throw(socket_error);
    void bind_gc()              throw(socket_error);
    void handle_comm(struct sockaddr_in &addr,
                     const char *msg,
                     int len
                     )          throw();
    void handle_gc(struct sockaddr_in &addr,
                   const char *msg,
                   int len
                   )            throw();
    void receive()              throw(socket_error);
    void receive_gc()           throw(socket_error);
    void send()                 throw(socket_error);

    void parse_packet(const CommPacketHeader& packet, const char* data,
                      int size)  throw();
    bool validate_packet(const char* msg, int len, CommPacketHeader& packet)
      throw();

    static void* runThread  (void*);

    void add_to_module();

  private:
    // TOOLConnect sub-thread controller
    TOOLConnect tool;
    int toolCommandState;

    // mutex lock for threaded data access
    pthread_mutex_t comm_mutex;
    // Sending packet data
    std::vector<float> data;
    // Received data
    std::list<std::vector<float> >* latest;

    // References to global data structures
    Sensors *sensors; // thread-safe access to sensor information
    CommTimer timer;
    GameController gc;

    // Socket information
    int sockn;
    int gc_sockn;
    struct sockaddr_in bind_addr;
    struct sockaddr_in broadcast_addr;
    char buf[UDP_BUF_SIZE];

};

#endif // ROBOT(NAO) - commenting out the entire file

#endif /* Comm_H */
