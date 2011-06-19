#include "ALSpeech.h"

#include <algorithm>
#include <iostream>

#include "alcore/alerror.h"

#include "manconfig.h"


ALSpeech::ALSpeech(AL::ALPtr<AL::ALBroker> broker) : Speech(), volume(0)
{
#ifdef USING_TTS
    try {
        alProxy =
            AL::ALPtr<AL::ALTextToSpeechProxy>(
                new AL::ALTextToSpeechProxy(broker));

    } catch(AL::ALError &e) {
        std::cout << "Failed to initialize proxy to ALTextToSpeech"
                  << std::endl;
    }
    volume = alProxy->getVolume();
#endif /* USING_TTS */
}

ALSpeech::~ALSpeech()
{

}

void ALSpeech::say(std::string text)
{
#ifdef USING_TTS
    if (isEnabled){
        replaceSymbols(text);
        alProxy->say(text);
    }
#endif /* USING_TTS */
}

void ALSpeech::setVolume(float v)
{
#ifdef USING_TTS
    if (v != volume){
        volume = v;
        alProxy->setVolume(v);
    }
#endif /* USING_TTS */
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
