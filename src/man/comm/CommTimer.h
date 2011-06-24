
#ifndef _CommTimer_h_DEFINED
#define _CommTimer_h_DEFINED

#include <vector>

#include "CommDef.h"
#include "commconfig.h"

class CommTimer
{
  public:
    CommTimer(llong (*f)());
    virtual ~CommTimer() { }

    llong timestamp() { return time() - epoch + offsetMicros; }

    bool timeToSend() { return timestamp() - lastPacketSentAt() > nextSendDelay; }

    void packetSent();

    void mark() { mark_time = timestamp(); }

    llong elapsed() { return timestamp() - mark_time; }

    llong lastPacketSentAt() const { return lastPacketSent; }

    void packetReceived() { lastPacketReceived = timestamp(); }

    llong lastPacketReceivedAt() const { return lastPacketReceived; }

    void setOffset(llong micros) { offsetMicros += micros; }

    llong getOffset() const { return offsetMicros; }

    int packetsDropped(const CommPacketHeader& packet);

    bool check_packet(const CommPacketHeader& packet);
    void updateTeamPackets(const CommPacketHeader& packet);
    void get_time_from_others();
    void reset();

  private:
    llong (*time)();                 // Pointer to function that returns current
                                     // time.
    llong epoch;
    llong lastPacketReceived;        // Time last packet was received.
    llong lastPacketSent;            // Time last packet was sent.
    llong offsetMicros;              // The number of microseconds by which the 
                                     // timer must be offset to be synced with the 
                                     // other robots' clocks.
    llong nextSendDelay;             // The timestamp when the next packet must be sent.
    llong mark_time;
    std::vector<CommTeammatePacketInfo> teamPackets;
    unsigned int packets_checked;
    bool need_to_update;
};

#endif // _CommTimer_h_DEFINED
