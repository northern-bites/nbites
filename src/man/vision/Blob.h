#ifndef Blob_h_defined
#define Blob_h_defined

#include "Common.h"
#include "VisionStructs.h"
#include "VisionHelpers.h"
#include <stdlib.h>

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
    void setBlob(point <int> tl, point <int> tr, point <int> bl,
                 point <int> br) {setLeftTop(tl); setRightTop(tr);
        setLeftBottom(bl); setRightBottom(br);}
	void setLeftTopX(int x) {leftTop.x = x;}
	void setLeftTopY(int y) {leftTop.y = y;}
	void setRightTopX(int x) {rightTop.x = x;}
	void setRightTopY(int y) {rightTop.y = y;}
	void setLeftBottomX(int x) {leftBottom.x = x;}
	void setLeftBottomY(int y) {leftBottom.y = y;}
	void setRightBottomX(int x) {rightBottom.x = x;}
	void setRightBottomY(int y) {rightBottom.y = y;}
    void setLeft(int x) {setLeftTopX(x); setLeftBottomX(x);}
    void setRight(int x) {setRightTopX(x); setRightBottomX(x);}
    void setTop(int y) {setLeftTopY(y); setRightTopY(y);}
    void setBottom(int y) {setLeftBottomY(y); setRightBottomY(y);}
	void setArea(int a) {area = a;}
	void setPixels(int p) {pixels = p;}
	void shift(int deltax, int deltay);

	// GETTERS
	point<int> getLeftTop() const {return leftTop;}
	int getLeftTopX() const {return leftTop.x;}
	int getLeftTopY() const {return leftTop.y;}
	point<int> getRightTop() const {return rightTop;}
	int getRightTopX() const {return rightTop.x;}
	int getRightTopY() const {return rightTop.y;}
	point<int> getLeftBottom() const {return leftBottom;}
	int getLeftBottomX() const {return leftBottom.x;}
	int getLeftBottomY() const {return leftBottom.y;}
	point<int> getRightBottom() const {return rightBottom;}
	int getRightBottomX() const {return rightBottom.x;}
	int getRightBottomY() const {return rightBottom.y;}
	int getLeft() const {return std::min(leftTop.x, leftBottom.x);}
	int getRight() const {return std::max(rightTop.x, rightBottom.x);}
	int getTop() const {return std::min(leftTop.y, rightTop.y);}
	int getBottom() const {return std::max(leftBottom.y, rightBottom.y);}
    int getMidBottomX() const {return (leftBottom.x + rightBottom.x) / 2;}
    int getMidBottomY() const {return (leftBottom.y + rightBottom.y) / 2;}
	int width() const;
	int height() const;
	int getArea() const;
	int getPixels() const {return pixels;}

    // blobbing
	void init();
	void merge(Blob other);
	void printBlob() const;
    bool isAligned(Blob b);

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
