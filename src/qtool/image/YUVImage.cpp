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

void YUVImage::resizeYUVArrays() {

    height = rawImage->get()->height();
    width = rawImage->get()->width();

    deallocateYUVArrays();

    yImg = new int*[width];
    uImg = new int*[width];
    vImg = new int*[width];
    for (int i = 0; i < width; i++) {
        yImg[i] = new int[height];
        uImg[i] = new int[height];
        vImg[i] = new int[height];
    }
}

void YUVImage::updateFromRawImage() {
	const byte* data =
	        reinterpret_cast<const byte*>(rawImage->get()->image().data());

	if (rawImageDimensionsEnlarged()) {
	    resizeYUVArrays();
	}

	assert(data != NULL);
	//copy the Y/U/V stuff
	int i = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; x += 2) {

			yImg[x][y] = data[i++];
			while (yImg[x][y] < 0) {
				yImg[x][y] = 256 + yImg[x][y];
			}
			yImg[x][y] = yImg[x][y] % 256;
			uImg[x][y] = data[i++];
			while (uImg[x][y] < 0) {
				uImg[x][y] = 256 + uImg[x][y];
			}
			uImg[x][y] = uImg[x][y] % 256;
			uImg[x + 1][y] = uImg[x][y];
			yImg[x + 1][y] = data[i++];
			while (yImg[x + 1][y] < 0) {
				yImg[x + 1][y] = 256 + yImg[x + 1][y];
			}
			yImg[x + 1][y] = yImg[x + 1][y] % 256;
			vImg[x][y] = data[i++];
			while (vImg[x][y] < 0) {
				vImg[x][y] = 256 + vImg[x][y];
			}
			vImg[x][y] = vImg[x][y] % 256;
			vImg[x + 1][y] = vImg[x][y];
		}
	}
}

void YUVImage::read(string s) {

	const char* data = s.data();
	int i = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; x += 2) {

			yImg[x][y] = data[i++];
			while (yImg[x][y] < 0) {
				yImg[x][y] = 256 + yImg[x][y];
			}
			yImg[x][y] = yImg[x][y] % 256;
			uImg[x][y] = data[i++];
			while (uImg[x][y] < 0) {
				uImg[x][y] = 256 + uImg[x][y];
			}
			uImg[x][y] = uImg[x][y] % 256;
			uImg[x + 1][y] = uImg[x][y];
			yImg[x + 1][y] = data[i++];
			while (yImg[x + 1][y] < 0) {
				yImg[x + 1][y] = 256 + yImg[x + 1][y];
			}
			yImg[x + 1][y] = yImg[x + 1][y] % 256;
			vImg[x][y] = data[i++];
			while (vImg[x][y] < 0) {
				vImg[x][y] = 256 + vImg[x][y];
			}
			vImg[x][y] = vImg[x][y] % 256;
			vImg[x + 1][y] = vImg[x][y];
		}
	}

}

int YUVImage::getRed(int x, int y) const {
	ColorSpace c;
	c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
	return c.getRb();
}

int YUVImage::getGreen(int x, int y) const {
	ColorSpace c;
	c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
	return c.getGb();
}

int YUVImage::getBlue(int x, int y) const {
	ColorSpace c;
	c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
	return c.getBb();
}

int YUVImage::getH(int x, int y) const {
    ColorSpace c;
    c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
    return c.getHb();
}

int YUVImage::getS(int x, int y) const {
    ColorSpace c;
    c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
    return c.getSb();
}

int YUVImage::getZ(int x, int y) const {
    ColorSpace c;
    c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
    return c.getZb();
}
