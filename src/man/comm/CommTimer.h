
#ifndef _CommTimer_h_DEFINED
#define _CommTimer_h_DEFINED

#include <vector>

#include "CommDef.h"


class CommTimer
{
  public:
    CommTimer(llong (*f)());
    virtual ~CommTimer() { }

    inline llong timestamp(void) {
      return time() - epoch;
    }
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

    bool check_packet(const CommPacketHeader &packet);
    void get_time_from_others();
    void reset();


  private:
    llong (*time)();
    llong epoch;

    llong packet_timer;
    llong mark_time;
    std::vector<llong> team_times;
    unsigned int packets_checked;
    bool need_to_update;
    //float point_fps;
    //float fps;
    //std::vector<float> fps_list;

};

#endif // _CommTimer_h_DEFINED
