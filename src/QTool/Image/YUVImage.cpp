#include "YUVImage.h"

using namespace std;

YUVImage::YUVImage(const RoboImage* _roboImage) :
		roboImage(_roboImage) {

	height = roboImage->getHeight();
	width = roboImage->getWidth();

	yImg = new int*[width];
	uImg = new int*[width];
	vImg = new int*[width];
	for (int i = 0; i < width; i++) {
		yImg[i] = new int[height];
		vImg[i] = new int[height];
		uImg[i] = new int[height];
	}

	updateFromRoboImage();
}

YUVImage::~YUVImage() {
	for (int i = 0; i < width; i++) {
		delete yImg[i];
		delete vImg[i];
		delete uImg[i];
	}
	delete yImg;
	delete uImg;
	delete vImg;
}

void YUVImage::updateFromRoboImage() {
	const byte* data = roboImage->getImage();
	if (data == NULL)
		return ;
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
