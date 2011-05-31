#include "YUVImage.h"

using namespace std;

YUVImage::YUVImage(const RoboImage* _roboImage) :
		roboImage(_roboImage){

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

void YUVImage::read(QString filename) {
	QFile file(filename);
	QTextStream out(stdout);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		out << "The file would not open properly" << "\n";
		return;
	}
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; x += 2) {
			QByteArray temp;
			temp = file.read(1);
			yImg[x][y] = temp[0];
			while (yImg[x][y] < 0) {
				yImg[x][y] = 256 + yImg[x][y];
			}
			yImg[x][y] = yImg[x][y] % 256;
			temp = file.read(1);
			uImg[x][y] = temp[0];
			while (uImg[x][y] < 0) {
				uImg[x][y] = 256 + uImg[x][y];
			}
			uImg[x][y] = uImg[x][y] % 256;
			uImg[x + 1][y] = uImg[x][y];
			temp = file.read(1);
			yImg[x + 1][y] = temp[0];
			temp = file.read(1);
			while (yImg[x + 1][y] < 0) {
				yImg[x + 1][y] = 256 + yImg[x + 1][y];
			}
			yImg[x + 1][y] = yImg[x + 1][y] % 256;
			vImg[x][y] = temp[0];
			while (vImg[x][y] < 0) {
				vImg[x][y] = 256 + vImg[x][y];
			}
			vImg[x][y] = vImg[x][y] % 256;
			vImg[x + 1][y] = vImg[x][y];
		}
	}
	file.close();
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

QImage YUVImage::getBitmap(BitmapType type) {
	QImage img(width, height, QImage::Format_RGB32);
	img.fill(0);
	ColorSpace c;

	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i) {
			c.setYuv(yImg[i][j], uImg[i][j], vImg[i][j]);
			int r, g, b;
			switch (type) {
			case YUV:
				r = c.getRb();
				g = c.getGb();
				b = c.getBb();
				break;

			case Y:
				r = g = b = yImg[i][j];
				break;

			case U:
				r = g = b = uImg[i][j];
				break;

			case V:
				r = g = b = vImg[i][j];
				break;

			case Red:
				r = g = b = c.getRb();
				break;

			case Green:
				r = g = b = c.getGb();
				break;

			case Blue:
				r = g = b = c.getBb();
				break;

			case Hue:
				if (c.getS() >= 0.25f && c.getY() >= 0.2f) {
					ColorSpace h = ColorSpace();
					h.setHsz(c.getH(), c.getS(), 0.875f);
					r = h.getRb();
					g = h.getGb();
					b = h.getBb();
				} else
					r = g = b = 0;
				break;

			case Saturation:
				r = g = b = c.getSb();
				break;

			case Value:
				r = g = b = c.getZb();
				break;

			default:
				r = g = b = 0;
				break;
			}
			QRgb value = qRgb(r, g, b);
			img.setPixel(i, j, value);

		}

	return img;
}
