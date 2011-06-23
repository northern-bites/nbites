#ifndef Comm_H
#define Comm_H

#include <vector>
#include <string>
#include <pthread.h>
#include <arpa/inet.h>  // inet_aton(), htonl()
#include <boost/shared_ptr.hpp>

#include "Common.h"
#include "synchro.h"
#include "Sensors.h"
#include "CommDef.h"
#include "RoboCupGameControlData.h"
#include "GameController.h"
#include "TOOLConnect.h"
#include "Vision.h"
#include "CommTimer.h"
#include "NogginStructs.h"
#include "commconfig.h"
#include "NetworkMonitor.h"

class Comm : public Thread
{
public:
    Comm(boost::shared_ptr<Synchro> _synchro, boost::shared_ptr<Sensors> s,
         boost::shared_ptr<Vision> v);
    ~Comm();

    int start();
    void stop();
    void run();

    int  startTOOL();
    void stopTOOL();
    const boost::shared_ptr<TriggeredEvent> getTOOLTrigger() {
        return tool.getTrigger();
    }
    void setLocalizationAccess(boost::shared_ptr<LocSystem> _loc,
                               boost::shared_ptr<BallEKF> _ballEKF);

    void discover_broadcast();
    void error(socket_error err) throw();
    void send(const char *msg, int len, sockaddr_in &addr) throw(socket_error);

    boost::shared_ptr<GameController> getGC() { return gc; }

    int getTOOLState();
    std::string getRobotName();
    std::list<std::vector<float> >* latestComm();
    TeammateBallMeasurement getTeammateBallReport();
    void setData(std::vector<float> &data);

    void add_to_module();
    static const int NUM_PACKET_DATA_ELEMENTS = 17;
private:
    void bind() throw(socket_error);
    void bind_gc() throw(socket_error);
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

    // Comm monitoring methods.

    // Calculates the running average delay between received
    // transmissions. Called each time a new packet is received.
    // Note that it only calculates those packets received from
    // other robots, not from the TOOL, GameController, or itself.
    void updateAverageDelay();

    // Calculates the running percentage of packets received that are
    // "ours" by recording the total packets received and the percent
    // that have our header (but not packets from the TOOL, GameController,
    // or from itself.
    void updatePercentReceived();

    // Gives an estimate for the latency in communications (i.e., the 
    // difference between the timestamp of time sent contained in the 
    // packet data and the current time.)
    llong estimatePacketLatency(const CommPacketHeader &latestPacket);

private:
    // mutex lock for threaded data access
    pthread_mutex_t comm_mutex;
    // Sending packet data
    std::vector<float> data;
    // Received data
    std::list<std::vector<float> >* latest;
    int lastPacketNumber;                    // Stores the number of the last packet
                                             // sent. Should be unique.

    // References to global data structures
    boost::shared_ptr<Sensors> sensors; // thread-safe access to sensors
    CommTimer timer;
    boost::shared_ptr<GameController> gc;

    // TOOLConnect sub-thread controller
    TOOLConnect tool;
    int toolCommandState;

    // Socket information
    int sockn;                               // Socket file descriptor. 
    int gc_sockn;                            // GameController socket file descriptor.
    struct sockaddr_in bind_addr;
    struct sockaddr_in broadcast_addr;
    struct sockaddr_in gc_broadcast_addr;
    char buf[UDP_BUF_SIZE];

    // Data monitoring information.
    llong averagePacketDelay;                 // Stores the running average delay between
                                              // received packets.
    int totalPacketsReceived;                 // Running total packets received.
    int ourPacketsReceived;                   // Running count of "our" packets received.

    NetworkMonitor monitor;
};

bool c_init_comm(void);

#endif /* Comm_H */
