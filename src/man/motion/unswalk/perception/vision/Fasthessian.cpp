

#include "Fasthessian.hpp"


using namespace std;

//-------------------------------------------------------


//! Constructor with image and y_coord vector (for 1D SURF)
FastHessian::FastHessian(float *img, std::vector<Ipoint> &ipts) 
                         : ipts(ipts),
                           img(img)
{

}

//-------------------------------------------------------


FastHessian::~FastHessian()
{
  for (unsigned int i = 0; i < responseMap.size(); ++i)
  {
    delete responseMap[i];
  }
}



//-------------------------------------------------------

//! Find the image features and write into vector of features
void FastHessian::getIpoints()
{
  // filter index map
  static const int filter_map [5][4] = {{0,1,2,3}, {1,3,4,5}, {3,5,6,7}, {5,7,8,9}, {7,9,10,11}};

  // Clear the vector of exisiting ipts
  ipts.clear();

  // Build the response map
	buildResponseMap();

  // Get the response layers
  ResponseLayer *b, *m, *t;
  for (int o = 0; o < OCTAVES; ++o) for (int i = 0; i <= 1; ++i)
  {
    b = responseMap.at(filter_map[o][i]);
    m = responseMap.at(filter_map[o][i+1]);
    t = responseMap.at(filter_map[o][i+2]);

    // loop over middle response layer at density of the most 
    // sparse layer (always top), to find maxima across scale and space
    for (int c = 0; c < t->width; ++c)
    {
      if (isExtremum(c, t, m, b))
      {
        saveExtremum(c, t, m, b);
      }
    }

  }
}

//-------------------------------------------------------

//! Build map of DoH responses
void FastHessian::buildResponseMap()
{
  // Calculate responses for the first 4 octaves:
  // Oct1: 9,  15, 21, 27
  // Oct2: 15, 27, 39, 51
  // Oct3: 27, 51, 75, 99
  // Oct4: 51, 99, 147,195
  // Oct5: 99, 195,291,387

  // Deallocate memory and clear any existing response layers
  for(unsigned int i = 0; i < responseMap.size(); ++i)  
    delete responseMap[i];
  responseMap.clear();

  // Get image attributes
  int w = (TOP_IMAGE_COLS / SURF_SUBSAMPLE / INIT_SAMPLE);
  int s = (INIT_SAMPLE);

  // Calculate approximated determinant of hessian values
  if (OCTAVES >= 1)
  {
    responseMap.push_back(new ResponseLayer(w,   s,   9));
    responseMap.push_back(new ResponseLayer(w,   s,   15));
    responseMap.push_back(new ResponseLayer(w,   s,   21));
    responseMap.push_back(new ResponseLayer(w,   s,   27));
  }
 
  if (OCTAVES >= 2)
  {
    responseMap.push_back(new ResponseLayer(w/2, s*2, 39));
    responseMap.push_back(new ResponseLayer(w/2, s*2, 51));
  }

  if (OCTAVES >= 3)
  {
    responseMap.push_back(new ResponseLayer(w/4, s*4, 75));
    responseMap.push_back(new ResponseLayer(w/4, s*4, 99));
  }

  if (OCTAVES >= 4)
  {
    responseMap.push_back(new ResponseLayer(w/8, s*8, 147));
    responseMap.push_back(new ResponseLayer(w/8, s*8, 195));
  }

  if (OCTAVES >= 5)
  {
    responseMap.push_back(new ResponseLayer(w/16, s*16, 291));
    responseMap.push_back(new ResponseLayer(w/16, s*16, 387));
  }

  // Extract responses from the image
  for (unsigned int i = 0; i < responseMap.size(); ++i)
  {
    buildHorizonResponseLayer(responseMap[i]);
  }
}

//-------------------------------------------------------


//! Calculate DoH responses for supplied hoirzlayer
void FastHessian::buildHorizonResponseLayer(ResponseLayer *rl)
{
  float *responses = rl->responses;        // response storage
  unsigned char *laplacian = rl->laplacian; // laplacian sign storage
  int step = rl->step;                      // step size for this filter
  int b = (rl->filter - 1)/2 + 1;         	// border for this filter
  int l = rl->filter / 3;                   // lobe for this filter (filter size / 3)
  int w = rl->filter;                       // filter size
  float inverse_area = 1.f/w;           		// normalisation factor
  float Dxx;

	int c, index = 0;

  for(int ac = 0; ac < rl->width; ++ac, index++) 
  {
    // get the image coordinates
    c = ac * step; 

    // Compute response components
		Dxx = BoxIntegral(img, c - b, w)
        - BoxIntegral(img, c - l / 2, l)*3;

    // Normalise the filter responses with respect to their size
    Dxx *= inverse_area;
   
    // Get the determinant of hessian response & laplacian sign
    responses[index] = (Dxx * Dxx);
    laplacian[index] = (Dxx >= 0 ? 1 : 0);

  }

}


//-------------------------------------------------------

//! Non Maximal Suppression function
inline int FastHessian::isExtremum(int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{

  // bounds check
  int layerBorder = (t->filter + 1) / (2 * t->step);
	if(c <= layerBorder || c >= t->width - layerBorder){
    	return 0;
  } 

  // check the candidate point in the middle layer is above thresh 
  float candidate = m->getResponse(c, t);
  if (candidate < THRESH){ 
    return 0; 
	}
	
	if (m->getResponse(c-1, t)  >= candidate) return 0;
	if (m->getResponse(c+1, t)  >= candidate) return 0;
  return 1;
}

//-------------------------------------------------------

//! Save scale-space extrema to form an image feature.   
inline void FastHessian::saveExtremum(int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b)
{
  // get the step distance between filters
  // check the middle filter is mid way between top and bottom
  assert( t->filter - m->filter == m->filter - b->filter);
 	
	Ipoint ipt;
   ipt.isRobot = 0;
	ipt.x = static_cast<float>( c * t->step);
	ipt.scale = static_cast<float>((0.1333f) * m->filter );
	ipt.laplacian = static_cast<int>(m->getLaplacian(c,t));
	ipts.push_back(ipt);
	
	return;
}

//-------------------------------------------------------


