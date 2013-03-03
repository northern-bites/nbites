#include "ImageTranscriberModule.h"
//#include "Profiler.h"

#include <iostream>

namespace man {
namespace image {

ImageTranscriberModule::ImageTranscriberModule()
    : Module(),
      topYUVImageOut(base()),
      bottomYUVImageOut(base()),
      topImageOut(base()),
      bottomImageOut(base()),
      topImageTranscriber(Camera::TOP),
      bottomImageTranscriber(Camera::BOTTOM),
      params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit),
      topTable(new unsigned char[yLimit * uLimit * vLimit]),
      bottomTable(new unsigned char[yLimit * uLimit * vLimit])
{
    initTable("/home/nao/nbites/lib/table/top_table.mtb", 0);
    initTable("/home/nao/nbites/lib/table/bottom_table.mtb", 1);
}

void ImageTranscriberModule::run_()
{
    // four portals -- YUVImages are provided for logging and vision debugging
    //              -- ThresHoldedImage is provided for use in all of vision, will change in the future
    portals::Message<YUVImage> topYUVMessage(0);
    portals::Message<YUVImage> bottomYUVMessage(0);
    portals::Message<ThresholdedImage> topThrMessage(0);
    portals::Message<ThresholdedImage> bottomThrMessage(0);

    // get YUVImages from the message pool
    // this copying is slow and bad, but it will have to do until Bill figures out a better solution
    //topYUVMessage.get()->makeMeCopyOf(topImageTranscriber.acquireImage(),
    //                                   YUVImage::ExactSize);
    //bottomYUVMessage.get()->makeMeCopyOf(bottomImageTranscriber.acquireImage(), 
    //                                      YUVImage::ExactSize);

    // bug fix so that Ben can test vision code, but no support for logging as of now
    *(topYUVMessage.get()) = topImageTranscriber.acquireImage();
    *(bottomYUVMessage.get()) = bottomImageTranscriber.acquireImage();

    ImageAcquisition::acquire_image_fast(topTable, params, topYUVMessage.get()->pixelAddress(0,0), 
					 topThrMessage.get()->get_mutable_image());
    ImageAcquisition::acquire_image_fast(bottomTable, params, bottomYUVMessage.get()->pixelAddress(0,0),
					 bottomThrMessage.get()->get_mutable_image());

    topYUVImageOut.setMessage(topYUVMessage);
    bottomYUVImageOut.setMessage(bottomYUVMessage);
    topImageOut.setMessage(topThrMessage);
    bottomImageOut.setMessage(bottomThrMessage);

    // all messages contain copies from buffer, so we can releaseBuffer()
    topImageTranscriber.releaseBuffer();
    bottomImageTranscriber.releaseBuffer();
}

    // if which == 0, top camera, else bottom camera
    // this may be made cleaner in the future
    void ImageTranscriberModule::initTable(const std::string& filename, int which)
    {
    	FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    	if (fp == NULL) {
    	    printf("CAMERA::ERROR::initTable() FAILED to open filename: %s\n",
       	        filename.c_str());
       	 return;
    	}

    	// actually read the table into memory
	// Color table is in VUY ordering
    	int rval;
    	if(which == 0){
       		for(int v=0; v < vLimit; ++v){
        	    for(int u=0; u< uLimit; ++u){
	        	rval = fread(&topTable[v * uLimit * yLimit + u * yLimit],
                        	       sizeof(unsigned char), yLimit, fp);
	    	    }	 
		}
    	}
    	else{
            for(int v=0; v < vLimit; ++v){
            	for(int u=0; u< uLimit; ++u){
	    	    rval = fread(&bottomTable[v * uLimit * yLimit + u * yLimit],
                	               sizeof(unsigned char), yLimit, fp);
	    	} 
	    }
    	}

    	printf("CAMERA::Loaded colortable %s.\n",filename.c_str());
    	fclose(fp);
    }
}
}
