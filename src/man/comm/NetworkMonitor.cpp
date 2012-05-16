
#include <fstream>
#include <iostream>
#include <iomanip>

#include "NetworkMonitor.h"

// Constants for the latency monitor.
static const int NUM_BINS_LATENCY = 30;
static const double LOW_BIN_LATENCY = 1.0f;        // Low is 1 microsecond.
static const double HIGH_BIN_LATENCY = 1000000.0f; // High is 1 second.
static const bool LOG_SCALE_LATENCY = true;

// Constants for the dropped packets monitor.
static const int NUM_BINS_DROPPED = 2;
static const double LOW_BIN_DROPPED = 0.0f;
static const double HIGH_BIN_DROPPED = 1.0f;
static const bool LOG_SCALE_DROPPED = false;
static const int PACKET_RECEIVED = 0;
static const int PACKET_DROPPED = 1;

// Constants for the Boxcar
static const int BOXCAR_WIDTH = 150;
// TODO: find actual constants...
static const double NETWORK_BAD  = 10.0f;
static const double NETWORK_GOOD = 2.0f;

// Not currently used, but might be useful as a sanity check
static const double PACKETS_DROPPED_THRESHOLD = 0.3333f;

NetworkMonitor::NetworkMonitor(long long time)
    :  Boxcar(BOXCAR_WIDTH),
       latency(NUM_BINS_LATENCY, LOW_BIN_LATENCY,
			   HIGH_BIN_LATENCY, LOG_SCALE_LATENCY),
       droppedPackets(NUM_BINS_DROPPED, LOW_BIN_DROPPED,
					  HIGH_BIN_DROPPED, LOG_SCALE_DROPPED),
       latencyPeak(0), sentWarning(false), lastOutput(time)
{
    Reset();

	std::cout << "Monitor Constructed" << std::endl;
}

NetworkMonitor::~NetworkMonitor()
{

}

void NetworkMonitor::Reset()
{
    Boxcar::Reset();
    latency.Reset();
    droppedPackets.Reset();
}

double NetworkMonitor::X(double x)
{
    return Boxcar::X(x);
}

void NetworkMonitor::packetReceived(long long timeSent, long long timeReceived)
{
    // Add the packet as not dropped.
    droppedPackets.X(0.0f);

	// Calculate the latency.
	latency.X(double(timeReceived - timeSent));

	// Boxcar
	X(timeReceived - timeSent);
}

void NetworkMonitor::packetsDropped(int numDropped)
{
    // For each dropped packet, increment the "dropped" bin.
    for(int i = 0; i < numDropped; ++i)
    {
		droppedPackets.X(1.0f);
    }
}

const int NetworkMonitor::totalPacketsReceived() const
{
    // 0 indicates that a packet has been received.
    return droppedPackets.binCount(PACKET_RECEIVED);
}

const int NetworkMonitor::totalPacketsDropped() const
{
    // 1 indicates a dropped packet.
    return droppedPackets.binCount(PACKET_DROPPED);
}

int NetworkMonitor::findPeakLatency()
{
    int maxBin = 0;
    for(int i = 0; i < NUM_BINS_LATENCY; ++i)
    {
		if(latency.binCount(i) > latency.binCount(maxBin))
			maxBin = i;
    }

    return maxBin;
}

int NetworkMonitor::performHealthCheck(long long time)
{
	if (time - warningTime > 30 * 1000000)  //TODO: switch to MICROS_PER_SECOND  
	{
		setSentWarning(false);
	}

	double health = Y();
	// Check to see if the latency has changed drastically.
	if (health > NETWORK_BAD)
	{
		if(!sentWarning)
		{
			std::cout << "NETWORK WARNING: Latency is BAD at: " << health
					  << std::endl;
			warningTime = time;
			setSentWarning(true);
		}
		return 3;
	}
	// Also, are we dropping more packets than we should be suddenly?
	if(health < NETWORK_GOOD)
	{
		return 1;
	}
	return 2;
}

void NetworkMonitor::logOutput(long long time)
{
    using namespace std;

	// Don't log too often. ~30 sec.
	if (time - lastOutput <= 30 * 1000000)  //TODO: switch to MICROS_PER_SECOND  
		return;

	// Update the output time.
	lastOutput = time;

    ofstream logFile;
    logFile.open("/home/nao/nbites/log/network.xls", ios::out);

    if(logFile.is_open())
    {
		const int width = 12;

		logFile << "Network Monitor Report" << endl;
		logFile << "Latency: " << endl;
		for(int i = 0; i < NUM_BINS_LATENCY; ++i)
		{
			logFile << setw(width) << setprecision(4) << latency.binMidPoint(i);
			logFile << setw(width) << latency.binCount(i);
			logFile << endl;
		}
		logFile << "Dropped packets: " << endl;
		logFile << setw(width) << "RECEIVED" << setw(width)
				<< droppedPackets.binCount(PACKET_RECEIVED)
				<< endl;
		logFile << setw(width) << "DROPPED" << setw(width)
				<< droppedPackets.binCount(PACKET_DROPPED)
				<< endl;
		logFile << setw(width) << "Current average" << setw(width)
				<< Y() << endl;

		logFile.close();
    }
    else
		cout << "NetworkMonitor::logOutput() : error opening log file!" << endl;
}

void NetworkMonitor::setSentWarning(bool sent)
{
    sentWarning = sent;
}
