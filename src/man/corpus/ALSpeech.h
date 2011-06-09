#ifndef _ALSpeech_h_DEFINED
#define _ALSpeech_h_DEFINED

#include <string>
#include "alproxies/altexttospeechproxy.h"
#include "Speech.h"

/**
 * An interface to the Aldebaran Text To Speech
 * functionality. Supports simple TTS operations.
 */
class ALSpeech : public Speech
{
public:
    ALSpeech(AL::ALPtr<AL::ALBroker> broker);
    virtual ~ALSpeech();

    // Speech class interface
public:
    virtual void say(std::string text);
    virtual void setVolume(float v);
    virtual float getVolume();
    void replaceSymbols(std::string& text);

private:
    AL::ALPtr<AL::ALTextToSpeechProxy> alProxy;
    float volume;

};

#endif /* _ALSpeech_h_DEFINED */
