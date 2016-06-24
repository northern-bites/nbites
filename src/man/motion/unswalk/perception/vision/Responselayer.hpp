

#include <memory.h>


class ResponseLayer
{
public:

  int width, step, filter;
  float *responses;
  unsigned char *laplacian;

  ResponseLayer(int width, int step, int filter)
  {
    assert(width > 0 );
    
    this->width = width;
    this->step = step;
    this->filter = filter;

    responses = new float[width];
    laplacian = new unsigned char[width];

    memset(responses,0,sizeof(float)*width);
    memset(laplacian,0,sizeof(unsigned char)*width);
  }

  ~ResponseLayer()
  {
    if (responses) delete [] responses;
    if (laplacian) delete [] laplacian;
  }

  inline unsigned char getLaplacian(unsigned int column)
  {
    return laplacian[column];
  }

  inline unsigned char getLaplacian(unsigned int column, ResponseLayer *src)
  {
    int scale = this->width / src->width;
    return laplacian[scale * column];
  }

  inline float getResponse(unsigned int column)
  {
    return responses[column];
  }

  inline float getResponse(unsigned int column, ResponseLayer *src)
  {
    int scale = this->width / src->width;
    return responses[scale * column];
  }

};
