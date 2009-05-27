#include "WBImageTranscriber.h"

using boost::shared_ptr;
using namespace std;

WBImageTranscriber::WBImageTranscriber(shared_ptr<Sensors> s)
    :ImageTranscriber(s)
{
    camera = wb_robot_get_device("camera");
    wb_camera_enable(camera,40);
}


WBImageTranscriber::~WBImageTranscriber(){}


void WBImageTranscriber::releaseImage(){}

void WBImageTranscriber::waitForImage(){
    //in this case, we don't wait at all...

    //First, get the RGB buffer from webots
    const unsigned char *image = wb_camera_get_image (camera);


    //next we need to translate the buffer to YUV, and make it
    //the correct size (half VGA) (it comes in quarter VGA)

    subscriber->notifyNextVisionImage();
}



