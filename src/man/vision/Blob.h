#ifndef Blob_h_defined
#define Blob_h_defined

#include "Common.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include <stdlib.h>

using namespace std;

class Blob {
public:
    Blob();
    virtual ~Blob() {}

    // SETTERS
	void setLeftTop(point <int> lt)
		{leftTop.x = lt.x; leftTop.y = lt.y;}
	void setRightTop(point <int> rt)
		{rightTop.x = rt.x; rightTop.y = rt.y;}
	void setLeftBottom(point <int> lb)
		{leftBottom.x = lb.x; leftBottom.y = lb.y;}
	void setRightBottom(point <int> rb)
		{rightBottom.x = rb.x; rightBottom.y = rb.y;}
	void setLeftTopX(int x) {leftTop.x = x;}
	void setLeftTopY(int y) {leftTop.y = y;}
	void setRightTopX(int x) {rightTop.x = x;}
	void setRightTopY(int y) {rightTop.y = y;}
	void setLeftBottomX(int x) {leftBottom.x = x;}
	void setLeftBottomY(int y) {leftBottom.y = y;}
	void setRightBottomX(int x) {rightBottom.x = x;}
	void setRightBottomY(int y) {rightBottom.y = y;}
	void setArea(int a) {area = a;}
	void setPixels(int p) {pixels = p;}

	// GETTERS
	point<int> getLeftTop() {return leftTop;}
	int getLeftTopX() {return leftTop.x;}
	int getLeftTopY() {return leftTop.y;}
	point<int> getRightTop() {return rightTop;}
	int getRightTopX() {return rightTop.x;}
	int getRightTopY() {return rightTop.y;}
	point<int> getLeftBottom() {return leftBottom;}
	int getLeftBottomX() {return leftBottom.x;}
	int getLeftBottomY() {return leftBottom.y;}
	point<int> getRightBottom() {return rightBottom;}
	int getRightBottomX() {return rightBottom.x;}
	int getRightBottomY() {return rightBottom.y;}
	int getLeft() {return min(leftTop.x, leftBottom.x);}
	int getRight() {return max(rightTop.x, rightBottom.x);}
	int getTop() {return min(leftTop.y, rightTop.y);}
	int getBottom() {return max(leftBottom.y, rightBottom.y);}
	int width();
	int height();
	int getArea();
	int getPixels() {return pixels;}

    // blobbing
	void init();
	void merge(Blob other);
	void printBlob();

private:
    // bounding coordinates of the blob
    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;
    int pixels; // the total number of correctly colored pixels in our blob
    int area;
};

#endif // Blob_h_defined
