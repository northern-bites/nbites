/* Vision.i */

%module AiboVision
%{
#include "CortexVision.h"
%}

%inline %{

class Image {
  private:
    int intImage[IMAGE_HEIGHT][IMAGE_WIDTH*3];
    int pointers[IMAGE_HEIGHT];
    byte img[IMAGE_HEIGHT*IMAGE_WIDTH*3];

  public:
    Image() {
      for (int i=0; i<IMAGE_HEIGHT; i++)
        pointers[i] = (int) &intImage[i];
    }

    void setImage(char *chars) { setImage((byte *) chars); }
    void setImage(byte *bytes) {
      memcpy(&img[0], bytes, IMAGE_HEIGHT*IMAGE_WIDTH*3);

      for (int y = 0; y < IMAGE_HEIGHT; y++) { 
        for (int x = 0, j = 0; x < IMAGE_WIDTH; x++, j+=3) {
          intImage[y][j] = img[y*IMAGE_WIDTH*3 + x];
          intImage[y][j + 1] = img[y*IMAGE_WIDTH*3 + x+IMAGE_WIDTH];
          intImage[y][j + 2] = img[y*IMAGE_WIDTH*3 + x+(IMAGE_WIDTH*2)];
        }
      }
    }

    byte* getImage() {
      return &img[0];
    }

    char* toString() {
      return (char*) &img[0];
    }
    void fromString(char* chars) {
      setImage((byte*) chars);
    }

    int getWidth() {
      return IMAGE_WIDTH;
    }

    int getHeight() {
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
