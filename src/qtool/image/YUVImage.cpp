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
    yImg = new int*[width];
    uImg = new int*[width];
    vImg = new int*[width];
    for (int i = 0; i < width; i++) {
        yImg[i] = new int[height];
        uImg[i] = new int[height];
        vImg[i] = new int[height];
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

bool YUVImage::areWithinImage(int x, int y) const {
    return 0 <= x && x < getWidth() && 0 <= y && y < getHeight();
}

int YUVImage::getY(int x, int y) const {
    if (areWithinImage(x, y))
        return yImg[x][y];
    else
        return 0;
}

int YUVImage::getU(int x, int y) const {
    if (areWithinImage(x, y))
        return uImg[x][y];
    else
        return 0;
}

int YUVImage::getV(int x, int y) const {
    if (areWithinImage(x, y))
        return vImg[x][y];
    else
        return 0;
}

int YUVImage::getRed(int x, int y) const {
    if (areWithinImage(x, y)) {
        ColorSpace c;
        c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
        return c.getRb();
    } else {
        return 0;
    }
}

int YUVImage::getGreen(int x, int y) const {
    if (areWithinImage(x, y)) {
        ColorSpace c;
        c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
        return c.getGb();
    } else {
        return 0;
    }
}

int YUVImage::getBlue(int x, int y) const {
    if (areWithinImage(x, y)) {
        ColorSpace c;
        c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
        return c.getBb();
    } else {
        return 0;
    }
}

int YUVImage::getH(int x, int y) const {
    if (areWithinImage(x, y)) {
        ColorSpace c;
        c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
        return c.getHb();
    } else {
        return 0;
    }
}

int YUVImage::getS(int x, int y) const {
    if (areWithinImage(x, y)) {
        ColorSpace c;
        c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
        return c.getSb();
    } else {
        return 0;
    }
}

int YUVImage::getZ(int x, int y) const {
    if (areWithinImage(x, y)) {
        ColorSpace c;
        c.setYuv(yImg[x][y], uImg[x][y], vImg[x][y]);
        return c.getZb();
    } else {
        return 0;
    }
}
