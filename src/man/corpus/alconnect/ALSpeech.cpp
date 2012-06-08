#include "ALSpeech.h"

#include <algorithm>
#include <iostream>

#include "alerror/alerror.h"

#include "manconfig.h"


ALSpeech::ALSpeech(boost::shared_ptr<AL::ALBroker> broker) : Speech(), volume(0)
{
    try {
        alProxy =
            boost::shared_ptr<AL::ALTextToSpeechProxy>(
                new AL::ALTextToSpeechProxy(broker));
        volume = alProxy->getVolume();
    } catch(AL::ALError &e) {
        std::cout << "Failed to initialize proxy to ALTextToSpeech"
                  << std::endl;
    }
    setVolume(.95f);
}

ALSpeech::~ALSpeech()
{

}

void ALSpeech::say(std::string text)
{
    if (isEnabled){
        replaceSymbols(text);
        try {
            alProxy->say(text);
        } catch(AL::ALError &e) {
            std::cout << "Failed to say something in ALTextToSpeech"
                      << std::endl;
        }
    }
}

void ALSpeech::setVolume(float v)
{
    if (v != volume){
        volume = v;
        try {
            alProxy->setVolume(v);
        } catch(AL::ALError &e) {
            std::cout << "Failed to set ALTextToSpeech volume"
                      << std::endl;
        }
    }
}

float ALSpeech::getVolume()
{
    return volume;
}

/**
 * We don't want to pronounce punctuation
 */
void ALSpeech::replaceSymbols(std::string& text)
{
    replace(text.begin(), text.end(), '_', ' ');
    replace(text.begin(), text.end(), ':', ' ');
    replace(text.begin(), text.end(), '-', ' ');
}
