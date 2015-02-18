#include "ImageAcquisition.h"
#include "VisionDef.h"

#include <tgmath.h> 


#define CPP_ACQUIRE 0
#define TRUE_ORANGE_X -0.68394  //-120
#define TRUE_ORANGE_Y 0.72954  //128

int ImageAcquisition::acquire_image(int rowCount,
										int colCount,
										int rowPitch,
										const uint8_t *yuv,
										uint8_t *out )
{
#if defined( __linux__) && !CPP_ACQUIRE
	_acquire_image (rowCount, colCount, rowPitch, yuv, out);

#else
	uint16_t *yOut = out;
	uint16_t *orangeOut = out + rowPitch*rowCount;

	// Y Averaging
	int outImage 
	for (int i=0; i < (rowCount >> 1); i++, yuv += rowPitch){
		for (int j=0; j < (rowHeight >> 1); j++, yuv += 4, yOut++, orangeOut++){
			*yOut = 
				yuv[YOFFSET1] + yuv[rowPitch + YOFFSET1] +
				yuv[YOFFSET2] + yuv[rowPitch + YOFFSET2];

			double u = ((yuv[UOFFSET] + yuv[rowPitch + UOFFSET]) / 2) - 128;
			double v = ((yuv[VOFFSET] + yuv[rowPitch + VOFFSET]) / 2) - 128;
			double magnitude = sqrt((u*u)+(v*v));
			u /= magnitude;
			v /= magnitude;
			double orangeU = TRUE_ORANGE_X;
			double orangeV = TRUE_ORANGE_Y;
			*oragneOut = ((u*orangeU + v*orangeV) + 1) * 128; 
		}
	}
#endif
	return 0;
}

// uint16_t *yOut = out;
// 	uint16_t *uvOut = out + height*width;
	
// 	// Y averaging (i->240, j->320)
// 	for (int i = 0; i < AVERAGED_IMAGE_HEIGHT; i++, yuv += NAO_IMAGE_ROW_OFFSET){
		
// 		for (int j =0; j < AVERAGED_IMAGE_WIDTH; j++, yuv+=4, yOut++, uvOut++) {

// 			//half avergae version YOFFSET1 = 0, YOFFSET2 = 2
// 			*yOut = yuv[YOFFSET1] + yuv[YOFFSET2];
// 			*uvOut = yuv[UOFFSET];
// 			*(uvOut+1) = yuv[VOFFSET];
// 		}
// 	}