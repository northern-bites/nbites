
#include "OfflineImageTranscriber.h"
#include "ImageAcquisition.h"
#include "VisionDef.h" // IMAGE_BYTE_SIZE
#include "Camera.h"

using namespace std;

namespace man {
namespace corpus {

OfflineImageTranscriber::OfflineImageTranscriber(
		boost::shared_ptr<Sensors> s,
		memory::MTopImage::const_ptr topImage,
		memory::MBottomImage::const_ptr bottomImage)
		: ThreadedImageTranscriber(s, "OfflineImageTranscriber"),
		  mTopImage(topImage),
		  mBottomImage(bottomImage),
		  table(new unsigned char[yLimit * uLimit * vLimit]),
		  params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit),
		  topImage(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])),
		  bottomImage(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE]))
{
}

OfflineImageTranscriber::~OfflineImageTranscriber() {
}

void OfflineImageTranscriber::run() {
	while(running) {
		this->waitForNewImage();
		this->acquireNewImage();
		subscriber->notifyNextVisionImage();
	}
}


void OfflineImageTranscriber::initTable(const string &filename) {
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        printf("initTable() FAILED to open filename: %s", filename.c_str());
        return;
    }

    // actually read the table into memory
    // Color table is in VUY ordering
    int rval;
    for(int v=0; v < vLimit; ++v){
        for(int u=0; u< uLimit; ++u){
            rval = fread(&table[v * uLimit * yLimit + u * yLimit],
                         sizeof(unsigned char), yLimit, fp);
        }
    }

    printf("Loaded colortable %s\n",filename.c_str());
    fclose(fp);
}

void OfflineImageTranscriber::acquireNewImage() {
    if (sensors->getWriteableNaoImage(Camera::BOTTOM) != NULL) {
        // TODO:
        // this is how we make the image that gets passed to man offline
        // appear valid in the memory viewer; this is slow; investigate a
        // better way to do this
        memcpy(sensors->getWriteableNaoImage(Camera::BOTTOM),
              reinterpret_cast<const uint8_t*>(mBottomImage->get()->
                                               image().data()),
               NAO_IMAGE_BYTE_SIZE);
        ImageAcquisition::acquire_image_fast(table, params,
                                       sensors->getNaoImage(Camera::BOTTOM),
                                             bottomImage);
        sensors->setImage(bottomImage, Camera::BOTTOM);
    } else {
        ImageAcquisition::acquire_image_fast(table, params,
                reinterpret_cast<const uint8_t*>(mBottomImage->get()
                                                 ->image().data()),
                bottomImage);
        sensors->setImage(bottomImage, Camera::BOTTOM);
    }

    if (sensors->getWriteableNaoImage(Camera::TOP) != NULL) {
        memcpy(sensors->getWriteableNaoImage(Camera::TOP),
              reinterpret_cast<const uint8_t*>(mTopImage->get()->
                                               image().data()),
               NAO_IMAGE_BYTE_SIZE);
        ImageAcquisition::acquire_image_fast(table, params,
                                          sensors->getNaoImage(Camera::TOP),
                                             topImage);
        sensors->setImage(topImage, Camera::TOP);
    } else {
        ImageAcquisition::acquire_image_fast(table, params,
                reinterpret_cast<const uint8_t*>(mTopImage->get()
                                                 ->image().data()),
                topImage);
        sensors->setImage(topImage, Camera::TOP);
    }
}

}
}
