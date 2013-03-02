#include "Images.h"
#include <stdio.h>

// *******************************
// *                             *
// *  Image Abstract Base Class  *
// *                             *
// *******************************

namespace messages {

void Image::makeMeWindowOf(const Image& img, int& x0, int& y0, int wd, int ht)
{
  // Top and left intersection
  if (x0 < 0)
  {
    wd += x0;
    x0 = 0;
  }
  if (y0 < 0)
  {
    ht += y0;
    y0 = 0;
  }

  // Bottom and right intersection
  int excess = x0 + wd - img.width();
  if (excess > 0)
    wd -= excess;
  excess = y0 + ht - img.height();
  if (excess > 0)
    ht -= excess;

  // If result is null, make sure both width and height are 0
  if (wd <= 0 || ht <= 0)
    wd = ht = 0;

  // Now we can set our size.
  setSize(wd, ht);
}

// *******************
// *                 *
// *  Pixel Buffers  *
// *                 *
// *******************

#ifdef TRACE_BUFFERS
void PixelBuffer::printTrace(bool releasing) const
{
  printf("%s %08X count = %d\n", releasing ? "Releasing" : "Grabbing ", this, referenceCount_);
}
#endif

// *****************************
// *                           *
// *  Memory Image Base Class  *
// *                           *
// *****************************

void MemoryImageBase::shareBuffer(const MemoryImageBase& img)
{
  // In case this image is already sharing a buffer with img, must getAddress before
  // release.
  img.buffer_->getAddress();
  if (buffer_)
    buffer_->release();
  buffer_ = img.buffer_;
}

MemoryImageBase::MemoryImageBase(const MemoryImageBase& img)
  : buffer_(0)
{
  // The copy constructor just uses the assignment operator, after insuring that this
  // is a valid object (bases are default constructed, buffer is set null)
  *this = img;
}

MemoryImageBase& MemoryImageBase::operator=(const MemoryImageBase& img)
{
  Image::operator=(img);          // assign base classes
  rowPitch_   = img.rowPitch_;    // assign members from this class
  pixelPitch_ = img.pixelPitch_;
  shareBuffer(img);               // share pixels
  return *this;
}

MemoryImageBase::~MemoryImageBase()
{
  if (buffer_)
    buffer_->release();
}

void* MemoryImageBase::makeMeOnHeap(int wd, int ht, int pixelSize)
{
  setSize(wd, ht);

  // Compute quadword alignment
  int alignment = 8 / pixelSize;
  if (alignment * pixelSize != 8)
    alignment = 1;

  rowPitch_   = (wd + alignment - 1) & ~(alignment - 1);
  buffer_     = new HeapPixelBuffer(rowPitch_ * ht * pixelSize);
  pixelPitch_ = 1;

  return buffer_->getAddress();
}

void* MemoryImageBase::makeMeInMemory(void* pixels, int wd, int ht, int rowPitch, int pixelPitch)
{
  setSize(wd, ht);
  rowPitch_   = rowPitch;
  pixelPitch_ = pixelPitch;
  buffer_     = new VideoPixelBuffer(pixels);

  return buffer_->getAddress();
}

void MemoryImageBase::makeMeWindowOf(const MemoryImageBase& img, int& x0, int& y0, int wd, int ht)
{
  Image::makeMeWindowOf(img, x0, y0, wd, ht);
  rowPitch_   = img.rowPitch_;
  pixelPitch_ = img.pixelPitch_;
  shareBuffer(img);
}

void MemoryImageBase::makeMeSubsampleOf(const MemoryImageBase& img, int& x0, int& y0, int xSample, int ySample)
{
  // If x0 or y0 are off the image, adjust them and keep the same pixel phase
  if (x0 < 0)
    x0 += (-x0 + xSample - 1) / xSample * xSample;
  if (y0 < 0)
    y0 += (-y0 + ySample - 1) / ySample * ySample;

  // Clip width and height as necessary
  int wd = (img.width () - x0 + xSample - 1) / xSample;
  int ht = (img.height() - y0 + ySample - 1) / ySample;

  // Handle null result
  if (wd <= 0 || ht <= 0)
    wd = ht = 0;

  // Now we can do the main work
  setSize(wd, ht);
  rowPitch_   = img.rowPitch_   * ySample;
  pixelPitch_ = img.pixelPitch_ * xSample;

  shareBuffer(img);
}

// ***************
// *             *
// *  YUV Image  *
// *             *
// ***************

MemoryImage8 YUVImage::yImage() const
{
  return crudeSubsample(0, 0, 2, 1);
}

MemoryImage8 YUVImage::uImage() const
{
  return crudeSubsample(1, 0, 4, 1);
}

MemoryImage8 YUVImage::vImage() const
{
  return crudeSubsample(3, 0, 4, 1);
}

YUVImage YUVImage::window(int x0, int y0, int wd, int ht) const
{
  int dx = (4 - (x0 & 3)) & 3;
  x0 += dx;
  wd -= dx;
  wd &= ~3;
  return PackedImage8::window(x0, y0, wd, ht);
}

YUVImage& YUVImage::makeMeCopyOf(const Image& img, CopyOptions co)
{
  int wd = img.width() & ~3;
  int ht = img.height();
  makeCopyOptions(co, wd, ht);

  PackedImage8::makeMeCopyOf(img, PackedImage8::JustPixels);
  return *this;
}

YUVImage& YUVImage::makeMeCopyOf(const MemoryImage8& img, CopyOptions co)
{
  PackedImage8::makeMeCopyOf(img.window(0, 0, img.width() & ~3, img.height()), co);
  return *this;
}
}
