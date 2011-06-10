
#ifndef _CommTimer_h_DEFINED
#define _CommTimer_h_DEFINED

#include <vector>

#include "CommDef.h"


class CommTimer
{
  public:
    CommTimer(llong (*f)());
    virtual ~CommTimer() { }

    inline llong timestamp(void) { return time() - epoch; }

    inline bool time_to_send(void) {
	return timestamp() - packet_timer > MICROS_PER_PACKET;
    }
    inline void sent_packet(void) {
      packet_timer = timestamp();
    }
    inline void mark(void) {
      mark_time = timestamp();
    }
    inline llong elapsed(void) {
      return timestamp() - mark_time;
    }
    inline llong elapsed_seconds(void) {
      return timestamp() - mark_time;
    }

    llong lastPacketSentAt() const { return packet_timer; }

    void packetReceived() { lastPacketReceived = timestamp(); }

    llong lastPacketReceivedAt() const { return lastPacketReceived; }

    bool check_packet(const CommPacketHeader &packet);
    void get_time_from_others();
    void reset();

  private:
    llong (*time)();                 // Pointer to function that returns current
                                     // time.
    llong epoch;
    llong lastPacketReceived;        // Time last packet was received.
    llong packet_timer;              // Time last packet was sent. 
    llong mark_time;
    std::vector<llong> team_times;
    unsigned int packets_checked;
    bool need_to_update;
};

#endif // _CommTimer_h_DEFINED
