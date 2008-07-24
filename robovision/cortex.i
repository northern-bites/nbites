/* cortex.i */
%module cortex

%{
#include "CortexVision.h"
%}

%inline %{

class Image {
  private:
    byte img[IMAGE_BYTE_SIZE];

  public:
    Image() {
    }

    byte* getImage() {
      return &img[0];
    }

    void setImage(byte *bytes) {
      memcpy(&img[0], bytes, IMAGE_BYTE_SIZE);
    }

    PyObject* __str__() {
      return PyString_FromStringAndSize((const char*)&img[0], IMAGE_BYTE_SIZE);
    }

    void fromString(char* chars) {
      setImage((byte*) chars);
    }

    int width() {
      return IMAGE_WIDTH;
    }

    int height() {
      return IMAGE_HEIGHT;
    }
}; /* Image */

%} /* inline */

class Vision {
  public:
    Vision();
    ~Vision();
    
    void imageLoop();
    
    void setImage(byte *img);
    void setGlutImg(int **intImage);
    
    byte* getImage();
    byte* getCorrectedImage();
    byte* getUncorrectedImage();

    void setColorTablePath(char *c);
    char* getColorTablePath();
};
