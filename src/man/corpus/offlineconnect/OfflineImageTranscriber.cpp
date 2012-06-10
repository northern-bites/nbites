
#include "OfflineImageTranscriber.h"
#include "ImageAcquisition.h"
#include "VisionDef.h" // IMAGE_BYTE_SIZE
#include "Camera.h"

using namespace std;

namespace man {
namespace corpus {

OfflineImageTranscriber::OfflineImageTranscriber(
        boost::shared_ptr<Sensors> s,
        memory::MRawImages::const_ptr rawImages)
    : ThreadedImageTranscriber(s, "OfflineImageTranscriber"),
      rawImages(rawImages),
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
    if (rawImages->get()->has_bottomimage()) {
        ImageAcquisition::acquire_image_fast(table, params,
                reinterpret_cast<const uint8_t*>(rawImages->get()->bottomimage().image().data()),
                bottomImage);
        sensors->setImage(bottomImage, Camera::BOTTOM);
    }

    if (rawImages->get()->has_topimage()) {
        ImageAcquisition::acquire_image_fast(table, params,
                     reinterpret_cast<const uint8_t*>(rawImages->get()->topimage().image().data()),
                     topImage);
             sensors->setImage(topImage, Camera::TOP);
    }

    if (rawImages->get()->vision_body_angles_size() > 0) {
        std::vector<float> body_angles(rawImages->get()->vision_body_angles().begin(),
                                       rawImages->get()->vision_body_angles().end());

        sensors->setVisionBodyAngles(body_angles);
    }

}

}
}
