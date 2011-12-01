
#include "TestImage.h"

namespace qtool {
namespace image {

    TestImage::TestImage()

{ } 

void TestImage::updateBitmap() {
    bitmap= QImage(640, 480, QImage::Format_RGB32);
    for(int j=0; j<480; ++j)
	for (int i = 0; i < 640; ++i){
	    QRgb value = qRgb(255,0,0);
	    bitmap.setPixel(i,j,value);
	}
}

}
}
