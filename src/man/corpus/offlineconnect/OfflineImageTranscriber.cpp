
#include "OfflineImageTranscriber.h"
#include "ImageAcquisition.h"
#include "VisionDef.h" // IMAGE_BYTE_SIZE

using namespace std;

namespace man {
namespace corpus {

OfflineImageTranscriber::OfflineImageTranscriber(boost::shared_ptr<Sensors> s,
		memory::MImage::const_ptr mImage)
		: ThreadedImageTranscriber(s, "OfflineImageTranscriber"),
		  mImage(mImage),
		  table(new unsigned char[yLimit * uLimit * vLimit]),
		  params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit),
		  image(reinterpret_cast<uint16_t*>(new uint8_t[IMAGE_BYTE_SIZE])){
	initTable("/home/oneamtu/nbites/data/tables/120lab-lizzie.mtb");
}

void OfflineImageTranscriber::run() {
	while(running) {
		this->waitForNewImage();
		this->acquireNewImage();
	}
}


void OfflineImageTranscriber::initTable(string filename) {
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
	ImageAcquisition::acquire_image_fast(table, params,
			reinterpret_cast<const uint8_t>(mImage->get()->image().data()),
			image);
}

}
}
