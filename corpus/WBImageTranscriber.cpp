#include "WBImageTranscriber.h"

using boost::shared_ptr;
using namespace std;

WBImageTranscriber::WBImageTranscriber(shared_ptr<Sensors> s)
    :ImageTranscriber(s)
{
}


WBImageTranscriber::~WBImageTranscriber(){}


void WBImageTranscriber::releaseImage(){}

void WBImageTranscriber::waitForImage(){
        subscriber->notifyNextVisionImage();
}


