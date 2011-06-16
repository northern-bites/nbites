#include <iostream>
#include "CommTimer.h"

using namespace std;


CommTimer::CommTimer(llong (*f)())
  : time(f), epoch(time()), lastPacketSent(0), mark_time(epoch),
    teamPackets(NUM_PLAYERS_PER_TEAM), packets_checked(0), need_to_update(false), 
    lastPacketReceived(0), offsetMicros(0)
{

}

void
CommTimer::reset()
{
  epoch = time();
  teamPackets = vector<CommTeammatePacketInfo>(NUM_PLAYERS_PER_TEAM);
  lastPacketSent = 0;
  mark_time = 0;
  packets_checked = 0;
  lastPacketReceived = 0;
}

bool
CommTimer::check_packet(const CommPacketHeader &p)
{
  llong ts = timestamp();

  // INVALID TIMESTAMP
  if (p.timestamp == GAME_INITIAL_TIMESTAMP){
	  cout << "game init timestamp" << endl;
	  return false;
  }
  // TOO OLD CHECK
  if (p.timestamp + PACKET_GRACE_PERIOD < ts){
	  std::cout << "too old to check. pt" << p.timestamp/1000 <<" ts: " << ts/1000 << std::endl;
	  return false;
  }

  // OUT OF ORDER CHECK
  /*
  if (p.timestamp < team_times[p.player - 1]){
	  std::cout << "out of order" << std::endl;
	  return false;
  }
  */
  // Check whether the packet number is greater than the last 
  // packet number received for that particular teammate.
  if(p.number < teamPackets[p.player-1].lastNumber) {
      cout << "CommTimer::check_packet() packet received out of order!" << endl;
      return false;
  }

  // JUST RESET CHECK
  if (!need_to_update && ts < PACKET_GRACE_PERIOD &&
      ts + PACKET_GRACE_PERIOD < p.timestamp){
	  std::cout << "just reset" << std::endl;
	  need_to_update = true;
	  return false;
  }

  // Packet is good!
  teamPackets[p.player - 1].timestamp = p.timestamp;
  packets_checked++;

  if (need_to_update)
    get_time_from_others();

  return true;
}

void
CommTimer::get_time_from_others()
{
  if (packets_checked < 2) {
    // not enought data yet, get time when we can later
    need_to_update = true;
    return;
  }

  llong tsum = 0;
  unsigned int num = 0;
  llong tstamp = timestamp();

  // average the times of those ahead of us
  for (vector<CommTeammatePacketInfo>::iterator tp = teamPackets.begin(); tp != teamPackets.end();
       tp++) {
      if (tstamp + PACKET_TIME_DISRUPTED < tp->timestamp) {
	  tsum += tp->timestamp;
	  num++;
      }
  }

  if (num > 0) {
      tsum /= num;
      epoch -= tsum - tstamp;
      need_to_update = false;
  }
}
