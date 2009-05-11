#include "WBTranscriber.h"

using boost::shared_ptr;
using namespace std;


WBTranscriber::WBTranscriber(shared_ptr<Sensors> s)
    :Transcriber(s){}


WBTranscriber::~WBTranscriber(){}



void WBTranscriber::postVisionSensors(){}
void WBTranscriber::postMotionSensors(){}
