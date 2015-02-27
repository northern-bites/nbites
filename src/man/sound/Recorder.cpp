#include "Recorder.h"
#include <alcommon/alproxy.h>
#include <iostream>



namespace sound {

    Recorder::Recorder(boost::shared_ptr<AL::ALBroker> pBroker,
                                     std::string pName)
    : ALSoundExtractor(pBroker, pName)
{
    setModuleDescription("This module processes the data collected by the "
                         "microphones and output in the ALMemory the RMS power "
                         "of each of the four channels.");
    
}

void Recorder::init()
{
    SOUNDO("Recorder init!");
    audioDevice->callVoid("setClientPreferences",
                          getName(),                //Name of this module
                          48000,                    //48000 Hz requested
                          (int)ALLCHANNELS,         //4 Channels requested
                          1                         //Deinterleaving requested
                          );
    
    startDetection();
}

Recorder::~Recorder()
{
    stopDetection();
}


/// Description: The name of this method should not be modified as this
/// method is automatically called by the AudioDevice Module.
void Recorder::process(const int & nbOfChannels,
                                const int & nbOfSamplesByChannel,
                                const AL_SOUND_FORMAT * buffer,
                                const ALValue & timeStamp)
{
    SOUNDO("Recorder asked to process!\n");
}
    
}   //sound


