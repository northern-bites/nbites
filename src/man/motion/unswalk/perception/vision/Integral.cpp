

#include "Integral.hpp"


// Convert horizon of image to single channel 32F
void getGrayHorizon(float * result, const VisionFrame *frame_p, int left_horizon, int right_horizon)
{

	// This is Bresenham's line algorithm
  int x0 = 0;
  int y0 = left_horizon;
	int x1 = TOP_IMAGE_COLS-1;
  int y1 = right_horizon;

  int dx = abs(x1-x0);
  int dy = abs(y1-y0); 

  int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
  int err = dx-dy; 

  int vsum;

  while (1){

		// Get pixel sum
    if(x0 % SURF_SUBSAMPLE == 0){		
      result[x0/SURF_SUBSAMPLE] = 0.0;
      vsum = 0; 
		  for (int j=-(SURF_HORIZON_WIDTH/2); j<=SURF_HORIZON_WIDTH/2; j+=2){		
			  if( (y0 + j) >=0 && (y0 + j) < TOP_IMAGE_ROWS){
          vsum +=	result[x0/SURF_SUBSAMPLE] += ((const_cast<uint8_t *>(frame_p->topImage)) + ((y0+j)  
                * TOP_IMAGE_COLS + x0 / 2 * 2) * 2)[2 * (x0 & 1)];	
				  }
		  }
      result[x0/SURF_SUBSAMPLE] = static_cast<float>(vsum);
    }
    if (x0 == x1 && y0 == y1) return;
    int e2 = 2*err;
    if (e2 > -dy){ 
      err -= dy;
      x0 += sx;
		}
    if (e2 < dx){ 
    	err += dx;
       y0 += sy; 
    }
  }
}



//! Computes the 1d integral image of the specified horizon line in 32-bit grey float
void Integral(float * data, const VisionFrame * source, int left_horizon, int right_horizon)
{
  // convert the image to single channel 32f
  getGrayHorizon(data, source, left_horizon, right_horizon);

  // one row only
  float rs = 0.0f;
  for(int j=0; j<TOP_IMAGE_COLS / SURF_SUBSAMPLE; j++) 
  {
    rs += data[j]; 
    data[j] = rs;
  }

  return;
}




