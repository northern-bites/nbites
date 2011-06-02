#include "ALSpeech.h"

#include "alcore/alerror.h"
#include <iostream>

ALSpeech::ALSpeech(AL::ALPtr<AL::ALBroker> broker) : Speech(), volume(0)
{
    try {
        alProxy =
            AL::ALPtr<AL::ALTextToSpeechProxy>(
                new AL::ALTextToSpeechProxy(broker));

    } catch(AL::ALError &e) {
        std::cout << "Failed to initialize proxy to ALTextToSpeech"
                  << std::endl;
    }
    volume = alProxy->getVolume();
}

ALSpeech::~ALSpeech()
{

}

void ALSpeech::say(const std::string& text)
{
    if (isEnabled){
        alProxy->say(text);
    }
}

void ALSpeech::setVolume(float v)
{
    if (v != volume){
        volume = v;
        alProxy->setVolume(v);
    }
}

float ALSpeech::getVolume()
{
    return volume;
}

