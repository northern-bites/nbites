#include "YUVImage.h"

using namespace std;

using man::memory::MImage;

YUVImage::YUVImage(MImage::const_ptr rawImage) :
		rawImage(rawImage), width(0), height(0),
		yImg(NULL), uImg(NULL), vImg(NULL) {

	updateFromRawImage();
}

YUVImage::~YUVImage() {
	deallocateYUVArrays();
}

bool YUVImage::rawImageDimensionsEnlarged() {
    return this->height < rawImage->get()->height() ||
           this->width < rawImage->get()->width();
}

void YUVImage::allocateYUVArrays(unsigned width, unsigned height) {
    yImg = new byte*[width];
    uImg = new byte*[width];
    vImg = new byte*[width];
    for (int i = 0; i < width; i++) {
        yImg[i] = new byte[height];
        uImg[i] = new byte[height];
        vImg[i] = new byte[height];
    }
}

void YUVImage::deallocateYUVArrays() {
    if (yImg && uImg && vImg) {
        for (int i = 0; i < width; i++) {
            delete yImg[i];
            delete vImg[i];
            delete uImg[i];
        }
        delete yImg;
        delete uImg;
        delete vImg;
    }
}

void YUVImage::resizeYUVArraysToFitRawImage() {
    resizeYUVArrays(rawImage->get()->width(),
                    rawImage->get()->height());
}

void YUVImage::resizeYUVArrays(unsigned width, unsigned height) {
    deallocateYUVArrays();
    allocateYUVArrays(width, height);
    this->width = width;
    this->height = height;
}

void YUVImage::updateFromRawImage() {
	const byte* data =
	        reinterpret_cast<const byte*>(rawImage->get()->image().data());

	if (rawImageDimensionsEnlarged()) {
	    resizeYUVArraysToFitRawImage();
	}

	assert(data != NULL);
	//copy the Y/U/V stuff
	this->read(data);
}

void YUVImage::read(const byte* data) {

    int i = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; x +=2 ) {
            yImg[x][y] = data[i++];

            uImg[x][y] = data[i++];
            uImg[x+1][y] = uImg[x][y];

            yImg[x + 1][y] = data[i++];

            vImg[x][y] = data[i++];
            vImg[x+1][y] = vImg[x][y];
        }
    }
}
