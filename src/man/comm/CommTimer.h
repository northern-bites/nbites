
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

    inline llong timestamp(void) { return time() - epoch + offsetMicros; }

    inline bool timeToSend(void) { return timestamp() - lastPacketSentAt() > MICROS_PER_PACKET; }

    inline void sent_packet(void) {
      lastPacketSent = timestamp();
    }
    inline void mark(void) {
      mark_time = timestamp();
    }
    inline llong elapsed(void) {
      return timestamp() - mark_time;
    }

    llong lastPacketSentAt() const { return lastPacketSent; }

    void packetReceived() { lastPacketReceived = timestamp(); }

    llong lastPacketReceivedAt() const { return lastPacketReceived; }

    void setOffset(llong micros) { offsetMicros += micros; }

    llong getOffset() const { return offsetMicros; }

    bool check_packet(const CommPacketHeader &packet);
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
    llong mark_time;
    std::vector<CommTeammatePacketInfo> teamPackets;
    unsigned int packets_checked;
    bool need_to_update;
};

#endif // _CommTimer_h_DEFINED
