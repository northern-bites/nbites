#include <iostream>
#include <cstdlib>
#include "CommTimer.h"

using namespace std;


CommTimer::CommTimer(llong (*f)())
    : time(f), epoch(time()), lastPacketReceived(0), lastPacketSent(0),
      offsetMicros(0), nextSendDelay(0), mark_time(epoch), 
      teamPackets(NUM_PLAYERS_PER_TEAM), numPacketsChecked(0), 
      need_to_update(false)
{
    // Default is six packets/second, but should vary randomly.
    // Right now, there are no randomized packet send delays!
    nextSendDelay = MICROS_PER_SECOND / 6;
    srand(static_cast<int>(time()));
}

void CommTimer::reset()
{
    epoch = time();
    teamPackets = vector<CommTeammatePacketInfo>(NUM_PLAYERS_PER_TEAM);
    lastPacketSent = 0;
    mark_time = 0;
    numPacketsChecked = 0;
    lastPacketReceived = 0;
}

void CommTimer::packetSent() 
{
    lastPacketSent = timestamp();
    // Calculate the next time a packet needs to be sent by randomly
    // choosing a value for the time between packets sent within
    // acceptable bounds.
    // int randPacketsPerSecond = rand() % (MAX_PACKETS_PER_SECOND + 1)
    // 	+ MIN_PACKETS_PER_SECOND;
    // nextSendDelay = MICROS_PER_SECOND / randPacketsPerSecond;
}

int CommTimer::packetsDropped(const CommPacketHeader& packet)
{
    // Note that the lastNumber in the CommTeammatePacketInfo struct 
    // is initialized to 0, even though packet numbering begins at 1,
    // so if the lastNumber == 0, this means that the robot has been
    // restarted; not taking this into account leads to huge numbers
    // recorded for packets dropped.
    if(teamPackets[packet.player-1].lastNumber == 0)
	return 0;

    int dropped = packet.number - teamPackets[packet.player-1].lastNumber;
    if(dropped <= 1)
	return 0;
    else
	return dropped - 1;
}

bool CommTimer::check_packet(const CommPacketHeader &p)
{
    llong ts = timestamp();

    // INVALID TIMESTAMP
    if (p.timestamp == GAME_INITIAL_TIMESTAMP)
    {
#ifdef COMM_DEBUG
	cout << "CommTimer::check_packet() : Invalid timestamp, game init timestamp." << endl;
#endif
	return false;
    }
    // TOO OLD CHECK
    if (p.timestamp + PACKET_GRACE_PERIOD < ts)
    {
#ifdef COMM_DEBUG
	cout << "CommTimer::check_packet() : Packet is too old to check (pt: " << p.timestamp/1000 <<" ts: " << ts/1000 
	     << ")." << endl;
#endif      
	return false;
    }
    // Check whether the packet number is greater than the last 
    // packet number received for that particular teammate.
    if(p.number < teamPackets[p.player-1].lastNumber) 
    {
#ifdef COMM_DEBUG  
	cout << "CommTimer::check_packet() : Packet received out of order!" << endl;
#endif
	return false;
    }
    // JUST RESET CHECK
    if (!need_to_update && ts < PACKET_GRACE_PERIOD &&
	ts + PACKET_GRACE_PERIOD < p.timestamp)
    {
#ifdef COMM_DEBUG
	cout << "CommTimer::check_packet() : Just reset." << endl;
#endif
	need_to_update = true;
	return false;
    }
  
    // All tests passed, packet must be valid.
    return true;
}

// This should be called if the packet is found to be valid.
void CommTimer::updateTeamPackets(const CommPacketHeader& packet)
{  
    // Packet is good!
    // Update timestamp and last number received.
    teamPackets[packet.player - 1].timestamp = packet.timestamp;
    teamPackets[packet.player - 1].lastNumber = packet.number;
    numPacketsChecked++;

    // Also check if a teammate has not been heard from for more than 
    // the TEAMMATE_DEAD_THRESHOLD time.
    

    if (need_to_update)
	get_time_from_others(); 
}

void CommTimer::checkDeadTeammates()
{
    llong currentTime = timestamp();

    for(vector<CommTeammatePacketInfo>::iterator t = teamPackets.begin();
	t != teamPackets.end(); ++t)
    {
	// If a teammate has not been heard from in more than 5 seconds,
	// reset their lastNumber so that subsequent packets are not
	// considered too old to process if they have been reset.
	if(currentTime - t->timestamp > TEAMMATE_DEAD_THRESHOLD)
	{
	    // Needs to be reset.
	    t->lastNumber = 0;
	}
    }
}

// Not sure yet what this does...may be redundant.
void CommTimer::get_time_from_others()
{
    if (numPacketsChecked < 2) {
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
