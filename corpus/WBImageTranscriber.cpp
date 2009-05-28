#include "WBImageTranscriber.h"
#include "VisionDef.h"

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
}


WBImageTranscriber::~WBImageTranscriber(){}


void WBImageTranscriber::releaseImage(){}


const YUV WBImageTranscriber::getWBYUV(const unsigned char * wimage,
                                       const int baseIndex){
    const float R = wimage[baseIndex + 0];
    const float G = wimage[baseIndex + 1];
    const float B = wimage[baseIndex + 2];

    const unsigned char Y =16 + (static_cast<int>( 65.738 * R +
                                                   129.057 * G +
                                                   25.064 * B  )) >> 8;
    const unsigned char U =128 + (static_cast<int>(-37.945 * R +
                                                   -74.494 * G +
                                                   112.439 * B  )) >> 8;
    const unsigned char V =128 + (static_cast<int>(112.439 * R +
                                                   -97.154 * G +
                                                   -18.285 * B  )) >> 8;
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

    cout << "Height scale is  " << HEIGHT_SCALE <<endl;
    cout << "Image width is " << IMAGE_WIDTH <<endl;
    //First, get the RGB buffer from webots
    const unsigned char *wbimage = wb_camera_get_image (camera);

    //next we need to translate the buffer to YUV, and make it
    //the correct size (half VGA) (it comes in quarter VGA)
    int maxIndex = 0;
    for(int i = 0; i < WEBOTS_IMAGE_HEIGHT; i++){
        for(int j = 0; j < WEBOTS_IMAGE_WIDTH; j+=2){//read two at a time

            const int baseIndex = (i*WEBOTS_IMAGE_WIDTH + j)*3;
            const int baseIndex2 = baseIndex + 3;

            //start by converting the RGB values into corresponding YUV
            const YUV yuv1 = getWBYUV(wbimage, baseIndex);
            const YUV yuv2 = getWBYUV(wbimage, baseIndex2);



            const int yuvBaseIndex = (i*IMAGE_WIDTH/2 + j)*4;
            const int yuvBaseIndex2 = (i*IMAGE_WIDTH/2 + j +1)*4;
            const int yuvBaseIndex3 = ((i+1)*IMAGE_WIDTH/2 + j)*4;
            const int yuvBaseIndex4 = ((i+1)*IMAGE_WIDTH/2 + j +1)*4;
            const int yuvBaseIndex5 = ((i+2)*IMAGE_WIDTH/2 + j)*4;
            const int yuvBaseIndex6 = ((i+2)*IMAGE_WIDTH/2 + j +1)*4;
            const int yuvBaseIndex7 = ((i+3)*IMAGE_WIDTH/2 + j)*4;
            const int yuvBaseIndex8 = ((i+3)*IMAGE_WIDTH/2 + j +1)*4;

            if(i  == 0 && j == 0){
                cout << "YUV at 60,80 is "<< (int)yuv1.Y <<","
                     << (int)yuv1.U<<"," <<(int)yuv1.V<<endl;
                cout << "RGB at 60,80 is "<< (int)wbimage[baseIndex + 0] <<","
                     << (int)wbimage[baseIndex + 1]<<","
                     <<(int)wbimage[baseIndex + 2]<<endl;

                cout << "yuv indices :" << yuvBaseIndex << ","
                     << yuvBaseIndex2 << ","
                     << yuvBaseIndex3 << ","
                     << yuvBaseIndex4 << ","<<endl;
            }
            //Now, we need to back the values in like YUYV, but we also need
            //to place them in four places...

//            const int yuvBaseIndex = (i*IMAGE_WIDTH + j)*4;
//             setTwoYUV(image,yuvBaseIndex,yuv1,yuv2);
//             

            const YUV test1 = {87,245,4};
            const YUV test2 = {87,245,4};

            setTwoYUV(image,yuvBaseIndex,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex2,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex3,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex4,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex5,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex6,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex7,yuv1,yuv2);
            setTwoYUV(image,yuvBaseIndex8,yuv1,yuv2);

//             for(int k = i*HEIGHT_SCALE;
//                 k<(i + HEIGHT_SCALE-1)*HEIGHT_SCALE; k++ ){
//                 for(int l = j*WIDTH_SCALE;
//                     j<(j +WIDTH_SCALE-1)*WIDTH_SCALE; l+=2){

//                     const int yuvBaseIndex = (k*IMAGE_WIDTH + l)*4;
//                     cout << "yuvbaseindex" << yuvBaseIndex<<endl;
//                     maxIndex = std::max(maxIndex,yuvBaseIndex);
//                     //setTwoYUV(image,yuvBaseIndex,yuv1,yuv2);
//                     setTwoYUV(&image[0],yuvBaseIndex,test1,test2);
//                 }
//             }
            //This means we are throwing out every other U and V value

        }
    }
    cout << "Max index was balls" << maxIndex<<endl;
    //Tell sensors that we have a new image for it
    sensors->lockImage();
    sensors->setImage(image);
    sensors->releaseImage();


    cout << "The values of first four rows of the image are "<<
        (int)image[0]<< ","<<
        (int)image[640]<< ","<<
        (int)image[1280]<< ","<<
        (int)image[1920]<< ","<<endl;
    subscriber->notifyNextVisionImage();
}



