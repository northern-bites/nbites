#include "WBImageTranscriber.h"
#include "VisionDef.h"
#include <webots/servo.h>
using boost::shared_ptr;
using namespace std;

#include <cmath>

const int WBImageTranscriber::WEBOTS_IMAGE_HEIGHT = 120;
const int WBImageTranscriber::WEBOTS_IMAGE_WIDTH  = 160;

const int WBImageTranscriber::HEIGHT_SCALE = IMAGE_HEIGHT / WEBOTS_IMAGE_HEIGHT;
const int WBImageTranscriber::WIDTH_SCALE  = IMAGE_WIDTH / WEBOTS_IMAGE_WIDTH ;
const int WBImageTranscriber::Y1_OFFSET    = 0;
const int WBImageTranscriber::U_OFFSET    = 1;
const int WBImageTranscriber::Y2_OFFSET    = 2;
const int WBImageTranscriber::V_OFFSET    = 3;


WBImageTranscriber::WBImageTranscriber(shared_ptr<Sensors> s)
    :ImageTranscriber(s),
     image(new unsigned char[IMAGE_BYTE_SIZE])
{
    camera = wb_robot_get_device("camera");
    wb_camera_enable(camera,40);


    for (int i =0; i< IMAGE_BYTE_SIZE; i++){
        image[i] = 0;
    }

    WbDeviceTag camServo  = wb_robot_get_device("CameraSelect");
    wb_servo_enable_position(camServo,20);
    wb_servo_set_position(camServo,0.6981);
}


WBImageTranscriber::~WBImageTranscriber(){}


void WBImageTranscriber::releaseImage(){}


const YUV WBImageTranscriber::getWBYUVFromRGB(const unsigned char * wimage,
                                              const int baseIndex){

    const int R = wimage[baseIndex + 0];
    const int G = wimage[baseIndex + 1];
    const int B = wimage[baseIndex + 2];

    //This formula was reverse engineered from the forumla in the TOOL using
    //mathmematica (just solving an eq in 3 vars)
    //And then the U and V channels were switched.  The formula is the same
    //as the one published, except it has R and B switched in the input
    const unsigned char Y =
        static_cast<unsigned char>(16+ ((  25  * R + 129 * G +  66*B)>>8));
    const unsigned char V =
        static_cast<unsigned char>(128+ (( 112 * R + -74 * G + -37*B)>>8));
    const unsigned char U =
        static_cast<unsigned char>(128+ (( -18 * R + -94 * G + 112*B)>>8));

    //This is the old "correct" forumla
    //     const unsigned char Y =16  + (( 66 * R + 129 * G + 25  *   + 128)) >> 8;
    //     const unsigned char U =128 + ((-38 * R - 74  * G + 112 * B + 128)) >> 8;
    //     const unsigned char V =128 + ((112 * R - 94  * G - 18  * B + 128)) >> 8;

    const YUV result = {Y,U,V};
    return result;
}

//must guarantee j is always even..
void WBImageTranscriber::setTwoYUV(unsigned char *image, const int baseIndex,
                                   const YUV yuv1, const YUV yuv2){
    image[ baseIndex + Y1_OFFSET] = yuv1.Y;
    image[ baseIndex + U_OFFSET]  = yuv1.U;
    image[ baseIndex + Y2_OFFSET] = yuv2.Y;
    image[ baseIndex + V_OFFSET]  = yuv1.V;
}

void WBImageTranscriber::waitForImage(){
    //in this case, we don't wait at all...

    //First, get the RGB buffer from webots
    const unsigned char *wbimage = wb_camera_get_image (camera);

    //next we need to translate the buffer to YUV, and make it
    //the correct size (half VGA) (it comes in quarter VGA)
    for(int i = 0; i < WEBOTS_IMAGE_HEIGHT; i++){
        for(int j = 0; j < WEBOTS_IMAGE_WIDTH; j+=2){//read two at a time

            const int baseIndex = (i*WEBOTS_IMAGE_WIDTH + j)*3;
            const int baseIndex2 = baseIndex + 3;

            //start by converting the RGB values into corresponding YUV
            const YUV yuv1 = getWBYUVFromRGB(wbimage, baseIndex);
            const YUV yuv2 = getWBYUVFromRGB(wbimage, baseIndex2);

            //TODO: (BUG). This method of expanding the image
            //causes streaking in the final image. fixing this requires
            //really understanding how two YUV values are packed into the same
            //place, and probably requires only setting on YUV pixel at a time
            const int yuvIndex = (i*HEIGHT_SCALE*IMAGE_WIDTH + j*WIDTH_SCALE)*2;
            setTwoYUV(image,yuvIndex,yuv1,yuv1);
            setTwoYUV(image,yuvIndex+4,yuv2,yuv2);
            setTwoYUV(image,yuvIndex+IMAGE_WIDTH*2,yuv1,yuv1);
            setTwoYUV(image,yuvIndex+IMAGE_WIDTH*2+4,yuv2,yuv2);
        }
    }

    //Tell sensors that we have a new image for it
    sensors->lockImage();
    sensors->setImage(image);
    sensors->releaseImage();

    subscriber->notifyNextVisionImage();
}



