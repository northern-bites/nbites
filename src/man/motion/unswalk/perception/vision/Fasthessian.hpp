
#pragma once


#include "types/Ipoint.hpp"
#include "perception/vision/Integral.hpp"
#include "perception/vision/Responselayer.hpp"
#include "perception/vision/VisionConstants.hpp"
#include <vector>


//-------------------------------------------------------


class ResponseLayer;


class FastHessian {
  
  public:

		//! Constructor with image and y_coords for 1D
    FastHessian(float *img, 
                std::vector<Ipoint> &ipts);

    //! Destructor
    ~FastHessian();

    //! Find the image features and write into vector of features
    void getIpoints();
    
  private:

    //---------------- Private Functions -----------------//

    //! Build map of DoH responses
    void buildResponseMap();

		//! Calculate DoH responses for supplied Horizon layer
    void buildHorizonResponseLayer(ResponseLayer *r);

    //! 3x3 Extrema test
    inline int isExtremum(int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);    
    
    //! Save function
    inline void saveExtremum(int c, ResponseLayer *t, ResponseLayer *m, ResponseLayer *b);
    

    //---------------- Private Variables -----------------//

		//! Reference to vector of features passed from outside 
    std::vector<Ipoint> &ipts;

    //! Pointer to the integral Image, and its attributes 
    float *img;

    //! Response stack of determinant of hessian values
    std::vector<ResponseLayer *> responseMap;

};

