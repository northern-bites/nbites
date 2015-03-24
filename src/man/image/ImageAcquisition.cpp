#include "ImageAcquisition.h"
#include "VisionDef.h"

#include <tgmath.h> 
#include <algorithm>

#define CPP_ACQUIRE 0

#define MAX_ORANGE_U_MIN 100
#define MAX_ORANGE_U_MAX 130

#define MIN_ORANGE_V_MIN 110
#define MIN_ORANGE_V_MAX 150

#define MAX_GREEN_U_MIN 90
#define MAX_GREEN_U_MAX 120

#define MAX_GREEN_V_MIN 140
#define MAX_GREEN_V_MAX 150

/*
TODO before sending to bill
change outs to 8 bit (oragnge and white)
uncomment out pointer adds
delete #if #elses
change line 40 orange++ to +2. same with white
make changes for 244 mode
delete * 2 on 70
 */

int ImageAcquisition::acquire_image(int rowCount,
									int colCount,
									int rowPitch,
									const uint8_t *yuv,
									uint8_t *out )
{
#if 0 
//	_acquire_image (rowCount, colCount, rowPitch, yuv, out);
#else

	uint16_t *yOut = (uint16_t*)out;
	uint16_t *whiteOut  = (uint16_t*)out /*+ rowPitch*rowCount*2 */;
	uint16_t *orangeOut = (uint16_t*)out /*+ rowPitch*rowCount */ ;
	uint16_t *greenOut  = (uint16_t*)out /*+rowPitch*rowCount */  ;
	

	int y;
	bool newRow = false;

	for (int i=0; i < rowCount; i ++, yuv += rowPitch*4){
		if (newRow)
			newRow = false;
		else newRow = true;

		for (int j=0; j < colCount; j++, yuv += 4, yOut++, orangeOut++, whiteOut++){
			// Y Averaging
			if (newRow) {
				*yOut = *(yOut + rowPitch) = y = yuv[YOFFSET1] + yuv[rowPitch*4 + YOFFSET1] +
		    	yuv[YOFFSET2] + yuv[rowPitch*4 + YOFFSET2];
		    }
#if 0
			//  Orange and Green Calcs
			double u = yuv[UOFFSET];
			double v = yuv[VOFFSET];

			double orangeU_width = MAX_ORANGE_U_MAX - MAX_ORANGE_U_MIN;
			double orangeV_width = MIN_ORANGE_V_MAX - MIN_ORANGE_V_MIN;
			double greenU_width = MAX_GREEN_U_MAX - MAX_GREEN_U_MIN;
			double greenV_width = MAX_GREEN_V_MAX - MAX_GREEN_V_MIN;

			double orange = std::min(std::min(std::max(MAX_ORANGE_U_MAX - u, 0.0), orangeU_width) / orangeU_width, std::min(std::max(v - MIN_ORANGE_V_MIN, 0.0), orangeV_width) / orangeV_width) * 256 * 2;
			

			double first = std::min(std::max(MAX_ORANGE_U_MAX - u, 0.0), orangeU_width) / orangeU_width;
			double second = std::min(std::max(v - MIN_ORANGE_V_MIN, 0.0), orangeV_width) / orangeV_width;

			if (first > 0 && second > 0)
				std::cout << "u: " << u << " v: " << v << " orange out: " << orange << " first: " << first << " second: " << second << std::endl;
			
			*orangeOut = orange;

#endif


#if 0
			// White Calc

#endif
		}
	}
#endif
	return 0;
}
