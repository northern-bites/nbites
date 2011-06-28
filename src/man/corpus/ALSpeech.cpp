#include "ALSpeech.h"

#include <algorithm>
#include <iostream>

#include "alcore/alerror.h"

#include "manconfig.h"


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

void ALSpeech::say(std::string text)
{
    if (isEnabled){
        replaceSymbols(text);
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

void ALSpeech::replaceSymbols(std::string& text)
{
    replace(text.begin(), text.end(), '_', ' ');
    replace(text.begin(), text.end(), ':', ' ');
    replace(text.begin(), text.end(), '-', ' ');
}
