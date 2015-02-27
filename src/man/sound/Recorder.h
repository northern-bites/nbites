#ifndef RECORDER_H
#define RECORDER_H
#include <string>
#include <rttools/rttime.h>

#include <boost/shared_ptr.hpp>
#include <alvalue/alvalue.h>
#include <alproxies/almemoryproxy.h>
#include <alaudio/alsoundextractor.h>

#define SOUND_NRM  "\x1B[0m"
#define SOUND_MRK   "\x1B[31m"

#define SOUNDO(format, ...) printf(SOUND_MRK format SOUND_NRM, __VA_ARGS__)

namespace sound {
    
class Recorder : public AL::ALSoundExtractor
{
    
public:
    
    Recorder(boost::shared_ptr<AL::ALBroker> pBroker, std::string pName);
    virtual ~Recorder();
    
    //method inherited from almodule that will be called after constructor
    void init();
    
public:
    void process(const int & nbOfChannels,
                 const int & nbrOfSamplesByChannel,
                 const AL::AL_SOUND_FORMAT * buffer,
                 const AL::ALValue & timeStamp);
};
    
}   //sound
#endif //RECORDER_H
