#ifndef _Images_
#define _Images_

// ********************
// *                  *
// *  RoboCup Images  *
// *                  *
// ********************
#include <assert.h>
#include <string>
#include <inttypes.h>

// A primary challenge in designing image classes is that the need to support very efficient
// pixel operations is at odds with a desire to have an elegant, modular, polymorphic set of
// classes. The class hierarchy herein achieves a reasonable tradeoff. 
//
// At the base of the hierarchy, the abstract base class Image defines an interface to any
// image, where an image is a two-dimensional zero-based array of pixels whose size is determined
// at run time. Pixels are integer quantities that can fit in 32 bits. Pixels can be read and
// written using relatively inefficient virtual functions, and a rectangular subset ("window") of
// the image can be made in small constant time. An image can be null, having no pixels. Attempts
// to read or write outside the bounds of an image results in an assertion failure in debug mode
// and are undefined in release mode. 
//
// The pixels of an image may reside in memory or may be computed from their (x, y) coordinates
// by any suitable formula. Computed images are read-only. Other, more exotic images can be
// derived from Image. 
//
// Code that takes an image as input and is not speed-critical should be written to operate on
// Image, for maximum flexibility. 
//
// The derived template class MemoryImage<T> is the concrete base class for any image whose
// pixels, of type T, reside in memory. T is expected to be an integer type. Pixels may reside on
// the heap or in a fixed video memory. Heap pixels are managed by these classes and are
// automatically deleted when all images sharing those pixels are destroyed. 
//
// A MemoryImage supports efficient pixel operations using native C++ pointers, and supports an
// efficient window operation where the window image shares pixels with the source image and so
// can be made without copying pixels. A client cannot tell whether or not a MemoryImage is a
// window of another image, so any image processing operation on a MemoryImage will work on
// full iamges and windows alike. 
//
// A MemoryImage supports an efficient subsample operation, which makes a new MemoryImage that
// consists of every xSample pixel in x and every ySample pixel in y. The new image is made in
// small constant time without copying pixels. This is not a good way to do subsampling for use
// as an image processing operation (e.g. for resolution pyramids); instead its main purpose is
// to obtain a Y, U, or V image that is part of a YUV composite image. 
//
// Code that takes an image as input and is speed-critical should generally operate on a
// MemoryImage. This allows the code to operate on every memory image type, including the Y, U,
// and V components of a YUV image as separate images without pixel copying. 
//
// Sometimes code assumes that pixels are sequential in memory, which is called a "packed" image.
// The Y, U, and V components of a YUV image are not packed, but most other images are. The
// template class PackedImage<T> is derived from MemoryImage<T> and guarantees that the pixels
// are packed. While MemoryImage is preferred, legacy or assembly language code may assume a
// packed image and can be written to operate on PackedImage. By doing so, attempts to pass a
// potentially not packed image to one of these routines can be detected at compile time,
// avoiding the need to figure out how to handle such an error at run time. 
//
// A YUVImage is a packed image of unsigned 8-bit pixels in the YUV composite format. The Y, U,
// and V components can be extracted as MemoryImages in small constant time. The width of a YUV
// image is guaranteed to be a multiple of 4. YUVImage has special versions of window and other
// member functions that insure that windows and copies of YUV images are valid YUV images (width
// is multiple of 4, pixels in YUYV order). 

// *******************************
// *                             *
// *  Image Abstract Base Class  *
// *                             *
// *******************************

namespace messages {

class Image
{
  int width_, height_;
  // Sizes are >= 0. Both are > 0, or both are == 0.

  void check() const { assert(width_ >= 0 && height_ >= 0 && !((width_ == 0) ^ (height_ == 0)));}
  // effect   Assertion failure if width and height are in an invalid state.

protected:
  // These protected functions provide utilities and encapsulation for derived classes.

  Image() { setSize(0, 0);}
  // effect   Default construct a null image

  Image(int wd, int ht) { setSize(wd, ht);}
  // effect   Construct with specified size
  // requires Sizes are >= 0. Both are > 0, or both are == 0.
  // note     An Image object cannot be constructed because it is an abstract class; these
  //          constructors are used by derived classes to initialize this base class

  Image(const Image& img) { setSize(img.width_, img.height_);}
  // effect   Copy constructor.
  // note     This does essentially the same thing that the default copy constructor would do
  //          (copy all data members), except that it's protected (the compiler-generated
  //          default copy constructor is public). I could have let the compiler generate the
  //          default copy constructor, because the public can't construct Image anyway because
  //          it's abstract. The call to check() in setSize should be unnecessary because the
  //          argument img has already been constructed and therefore has been checked and is
  //          valid, but no harm in checking again. 

  Image& operator=(const Image& img) { setSize(img.width_, img.height_); return *this;}
  // returns  This object as l-value, the standard behavior of assignment operators
  // effect   Assignment operator.
  // note     Like the above copy constructor, the compiler will generate a default assignment
  //          operator if you don't provide one, so the programmer must consider for each class
  //          how assignment and copy construction should be handled. Like the copy constructor,
  //          we do essentially what the default would do, but we make it protected so the public
  //          can't do an assignment to this base class of a concrete derived class. 

  void setSize(int wd, int ht) { width_ = wd; height_ = ht; check();}
  // effect   Set the size of the image
  // requires Sizes are >= 0. Both are > 0, or both are == 0.

  void makeMeWindowOf(const Image&, int& x0, int& y0, int wd, int ht);
  // effect   Compute the intersection of the specified rectangle with me, update x0, y0
  //          and set my size accordingly.
  // notes    The intersection can be null, in which case the updated values of x0 and y0
  //          are undefined.
  //          This is a helper function for derived classes, which must complete the work
  //          of making a window.

  void check(int x, int y) const { assert(pixelExists(x, y));}
  // effect   Assertion failure if the specified (x, y) are beyond the bounds of this image.
  // note     For a null image, every (x, y) is beyond the bounds.

public:
  virtual ~Image() {}
  // note     A class with virtual functions should have a virtual destructor even if it
  //          does nothing, so that destroying an object via a pointer or reference to this
  //          base class invokes the correct overriding destructor.

  int width () const { return width_ ;}
  int height() const { return height_;}
  // returns  Size of this image

  bool isNull() const { return width_ == 0;}
  // returns  true if this image is null
  // note     A null image has no pixels; width() and height() are 0.

  bool pixelExists(int x, int y) const { return 0 <= x && x < width_ && 0 <= y && y < height_;}
  // returns  true if the specified pixel exists
  // note     For a null image, always returns false.

  virtual int getPixel(int x, int y) const = 0;
  // returns  The pixel value at the specified (x, y)
  // requires pixelExists(x, y) is true
  // note     Pure virtual function, must be implemented by some derived class

  virtual int putPixel(int x, int y, int z) = 0;
  // returns  z
  // effect   Set the pixel value at the specified (x, y) to z
  // requires pixelExists(x, y) is true
  // notes    Pure virtual function, must be implemented by some derived class.
  //          If z doesn't fit in the pixel, the results are specified by the derived
  //          class that implements the function.

  virtual Image* newWindow(int x0, int y0, int wd, int ht) const = 0;
  // returns  A new image that is the specified window of this image.
  // notes    The returned image may be null (but the pointer returned won't be)
  //          The returned image is made with the new operator and must be deleted with the
  //          delete operator
  //          Like all windows, the returned image shares pixels with this image.
  //          Executes in small constant time.
};

// *******************
// *                 *
// *  Pixel Buffers  *
// *                 *
// *******************

// The pixel buffer classes manage the memory that holds pixels for images whose pixels reside
// in memory. The goal is to allow images to share pixels efficiently for a variety of purposes,
// and for a buffer holding the pixels to be deleted automatically when the last image using that
// buffer is destroyed. The variety of purposes includes: 
//
//    * Making in small constant time a window (rectangular subset of some image) that shares
//      pixels with the image 
//    * Making in small constant time a subsample (an every nth pixel subset of some image) that
//      shares pixels with the image, primarily to support extracting Y, U, and V images from a
//      YUV composite
//    * Making in small constant time a copy of an image, using copy constructors or assignment
//      operators, that shares pixels with the image. This allows very efficient construction
//      and destruction of copies, which is very convenient for functions that return images,
//      and to take advantage of the full power of C++ temporary objects in expressions, which
//      have to be constructed and destroyed behind the scenes.
//
// Typically a pixel buffer resides on the heap, where these classes manage its lifetime
// automatically. A pixel buffer may also reside in memory outside the control of these classes,
// the primary example being video memory into which images are acquired from a camera. Video
// memory is managed by the acquisition system. Any memory managed by other code, video or
// otherwise, can be used as a pixel buffer as needed.
//
// The abstract base class Pixelbuffer implements functionality common to heap and video buffers
// (and any other types that one wishes to derive). The derived classes HeapPixelBuffer and
// VdeoPixelBuffer handle the two concrete cases. 

// **************************************
// *                                    *
// *  Pixel Buffer Abstract Base Class  *
// *                                    *
// **************************************

//#define TRACE_BUFFERS

class PixelBuffer
{
  int referenceCount_;
  // number of images using this buffer

  PixelBuffer(const PixelBuffer&);
  PixelBuffer& operator=(const PixelBuffer&);
  // disallow copy construction and assignment

#ifdef TRACE_BUFFERS
  void printTrace(bool releasing) const;
  // effect   Print a diagnostic message when a buffer is shared or released.
#else
  void printTrace(bool) {}
#endif

protected:
  PixelBuffer() : referenceCount_(0) {}
  // effect   Default constructor
  // note     The public is not allowed to construct this.

  virtual ~PixelBuffer() {}
  // note     See above note on virtual destructors. The public is not allowed to destroy
  //          a pixel buffer, it happens automatically.

  virtual void* address() = 0;
  // returns  The address of the first pixel in the buffer
  // note     The public can only get the address using getAddress(), which updates the
  //          reference count.

public:
  void* getAddress() { ++referenceCount_; printTrace(false); return address();}
  // returns  The address of the first pixel in the buffer.
  // effect   Share this buffer (by updating the reference count).
  //          Print disgnostic trace if enabled.
  // note     Cast the returned void* to whatever pixel type you want.
  
  void release() { --referenceCount_; printTrace(true); if (referenceCount_ == 0) delete this;}
  // effect   Stop sharing this buffer . If this was the last share, delete the buffer.
  // note     The concrete pixel buffer objects are always on the heap. They are deleted by using
  //          the delete operator on this object itself, which first invokes the virtual
  //          destructor that allows the derived class to delete the (separate) buffer holding
  //          the pixels. Doing a "delete this" is potentially dangerous, because the function
  //          will continue to execute after the object itself is gone. But since the "delete
  //          this" is the last thing the function does, all it needs to do after the object is
  //          gone is return, which doesn't need any info from the object. But the truth of this
  //          argument is not specified by the language rules, it is based on my knowledge of the
  //          behavior of any reasonable compiler generating code for any reasonable computer
  //          architecture. If you Google "delete this" you'll find lots of commentary on the
  //          subject, ranging from it's practical for reference counting (which is what I'm
  //          doing) to why would you ever want to do it, to it means you're doing something
  //          wrong or don't understand C++. In our case, the problem is that no object owns a
  //          pixel buffer, they are shared, so there is no clear responsibility for deleting
  //          one. To avoid using "delete this" I could have release() return a boolean that
  //          directs the caller to delete the pixel buffer, but that seems to me to be a
  //          violation of modularity and encapsulation that is worse. But it's a judgment call. 
};

// **************************
// *                        *
// *  Pixel Buffer On Heap  *
// *                        *
// **************************

class HeapPixelBuffer : public PixelBuffer
{
  char* pixels_;
  // Buffers on the heap are arrays of bytes regardless of pixel type.

protected:
  virtual ~HeapPixelBuffer() { delete [] pixels_;}
  // effect   The destroctor just deletes the buffer

  virtual void* address() { return pixels_;}
  // returns  The address of the first pixel

public:
  HeapPixelBuffer(int size) { pixels_ = new char[size];}
  // effect   Construct a new pixel buffer of the specified size
  // requires You must make this with the new operator.
  //          You must call getAddress() next, to indicate that you are using this buffer,
  //          otherwise it won't be managed properly. But you need to call getAddress() to
  //          get the address, so you're not likely to forget.
  // note     The buffer is not yet shared by anyone, even the client who just constructed
  //          this. That happens when getAddress() is called. 
};

// **********************************
// *                                *
// *  Pixel Buffer in Video Memory  *
// *                                *
// **********************************

class VideoPixelBuffer : public PixelBuffer
{
  void* pixels_;
  // Just an address, don't care what it points to.

protected:
  virtual ~VideoPixelBuffer() { /* What should go here? */ }
  // effect   Let the owner know that the buffer is no longer needed.
  // note     Not yet clear what to do here.

  virtual void* address() { return pixels_;}
  // returns  The address of the first pixel

public:
  VideoPixelBuffer(void* pixels) { pixels_ = pixels;}
  // effect   Construct a video pixel buffer at the specified address
  // note     See above requirements and note for HeapPixelBuffer's constructor.
};

// *******************
// *                 *
// *  Memory Images  *
// *                 *
// *******************

// *****************************
// *                           *
// *  Memory Image Base Class  *
// *                           *
// *****************************
//
// This is the abstract base class for all images that reside in memory. It is abstract because
// it doesn't implement the pure virtual functions of its base class Image. This class implements
// all of the memory image mechanism that is not dependent on the type of pixels, including all
// interface to pixel buffers. The concrete derived template class MemoryImage<T> adds just the
// mechanism that depends on the pixel type T. It is good programming style in C++ to restrict a
// template class as much as is practical to just those things that depend on the template
// parameters, putting all common mechanism in a base class. This can significantly reduce the
// amount of code generated, and provides a clearer and perhaps easier to maintain
// implementation, at a cost of some increased time up front in figuring out and implementing the
// division between the base class and the template class. Note that reducing the amount of code
// generated may not be a big deal on contemporary machines with gigabytes of memory, but it can
// also reduce instruction cache misses which can be a big deal in time-critical systems. 
//
// It is possible in principal for a sufficiently smart compiler to split a template class
// automatically, avoiding the need for the programmer to figure out how to do it. I don't know
// if any contemporary compilers are that smart. I'll always do it manually because it forces me
// to think carefully about the design, and the result is more elegant. 

class MemoryImageBase : public Image
{
  // The pitch values are in units of pixels, not bytes.
  int           rowPitch_;    // The offset from a pixel to its neighbor in the following row (y)
  int           pixelPitch_;  // The offset from a pixel to its neighbor in the following column (x)
  PixelBuffer*  buffer_;      // The pixel buffer in which the pixels are held. Can be a null
                              // pointer if this image doesn't yet have pixels, which is different
                              // from an image being null. A null image might or might not have
                              // pixels, in the sense that it might be sharing a pixel buffer even
                              // if its size is 0.

  void shareBuffer(const MemoryImageBase&);
  // effect   Cause this memory image to share a pixel buffer with the specified memory image.
  // note     If this image was sharing another pixel buffer, that one is released.
  //          The buffer to be shared can be the same one as this image is currently sharing. This
  //          is handled correctly.

protected:
  // Almost all of this class is for the benefit of the MemoryImage<T>, so most member functions are
  // protected.

  MemoryImageBase() : buffer_(0) {}
  // effect   Default construct a null image with no pixel buffer

  MemoryImageBase(const MemoryImageBase&);
  // effect   Copy construct from specified memory image, share the pixels.
  // note     Executes in small constant time

  MemoryImageBase& operator=(const MemoryImageBase&);
  // returns  This object as l-value, standard for assignment operator
  // effect   Set this image to be a copy of the specified memory image, sharing the pixels, and
  //          releasing any pixels currently being shared.
  // note     Executes in small constant time

  virtual ~MemoryImageBase();
  // effect   Destroy this image

  void* makeMeOnHeap(int wd, int ht, int pixelSize);
  // returns  Address of first pixel, for derived class use
  // effect   Make me a new memory image of specified size on the heap, releasing any currently shared
  //          pixels.
  // notes    After this call, this image is the only one currently sharing the pixels.
  //          rowPitch is guaranteed to be a multiple of 8 bytes. So if the pixel size is 2
  //          bytes, rowPitch is a multiple of 4. Maintaining the quadword alignment of pixels in
  //          a column can sometimes be used to enhance efficiency.
  //          pixelPitch will be 1.

  void* makeMeInMemory(void* pixels, int wd, int ht, int rowPitch, int pixelPitch = 1);
  // returns  Address of first pixel, for derived class use
  // effect   Make me a new memory image of specified size at the specified address and pitch
  //          parameters, releasing any currently shared pixels. 
  // note     After this call, this image is the only one currently sharing the pixels.

  void makeMeWindowOf(const MemoryImageBase&, int& x0, int& y0, int wd, int ht);
  // effect   Make me a window of the specified memory image, sharing its pixels and releasing
  //          any currently shared pixels. Update x0 and y0 and the size of this image to
  //          be the intersection of this image with the specified window.
  // note     The pixels to be shared can be the same ones as this image is currently sharing.

  void makeMeSubsampleOf(const MemoryImageBase&, int& x0, int& y0, int xSample, int ySample);
  // effect   Make me a subsample of the specified memory image, sharing its pixels and releasing
  //          any currently shared pixels. Update x0 and y0 and the size of this image to
  //          be the intersection of this image with the specified subsample.
  // note     The pixels to be shared can be the same ones as this image is currently sharing.

public:
  int rowPitch  () const { return rowPitch_  ;}
  int pixelPitch() const { return pixelPitch_;}
  // returns  The pitch values.
};

// ******************
// *                *
// *  Memory Image  *
// *                *
// ******************

// Here is the template class that adds functionality dependent on the particular pixel type. The classes
// generated from this template are the first concrete image classes so far.
//
// Pixels must be of native integral type.

template<class T>
class MemoryImage : public MemoryImageBase
{
  T* pixels_;

public:
  MemoryImage() {}
  // effect   Default construct null image

  MemoryImage(int wd, int ht);
  // effect   Construct new (not yet shared) image on heap of specified size.

  MemoryImage(T* pixels, int wd, int ht, int rowPitch, int pixelPitch = 1);
  // effect   Construct new image at specified address in memory, of specified size and pitch.

  MemoryImage(const MemoryImage&);
  // effect   Copy construct from spacified image, sharing pixels
  // note     Executes in small constant time

  MemoryImage& operator=(const MemoryImage&);
  // returns  This object as l-value, standard for assignment operator
  // effect   Set this image to be a copy of the specified memory image, sharing the pixels, and
  //          releasing any pixels currently being shared.
  // note     Executes in small constant time

  T* pixelAddress(int x, int y) const { return pixels_ + y * rowPitch() + x * pixelPitch();}
  // returns  A pointer to the specified pixel.
  // note     You are welcome to get a pointer to a non-existent pixel, just be careful about how
  //          you use it.

  virtual int getPixel(int x, int y       ) const { check(x, y); return *pixelAddress(x, y)       ;}
  virtual int putPixel(int x, int y, int z)       { check(x, y); return *pixelAddress(x, y) = (T)z;}
  // note     Implement the virtual pixel get/set functions of the Image base class.

  MemoryImage window(int x0, int y0, int wd, int ht) const;
  // returns  An image that is the specified window of this image, sharing pixels.
  // notes    Executes in small constant time.
  //          Takes advantage effecient copy construction of images.
  //          The returned image is not on the heap and must not be deleted. It can be assigned, used for
  //          construction, used in an expression, etc. The compiler will handle destroying it when it
  //          goes out of scope. Of course the pixels may be on the heap in a pixel buffer, but those
  //          buffers are separate from instances of image classes.

  virtual Image* newWindow(int x0, int y0, int wd, int ht) const { return new MemoryImage(window(x0, y0, wd, ht));}
  // note     Implement the virtual window function of the Image base class.

  MemoryImage crudeSubsample(int x0, int y0, int xSample, int ySample) const;
  // returns  An image that is the specified subsample of this image, sharing pixels.
  // notes    Executes in small constant time.
  //          Takes advantage effecient copy construction of images.
  //          The returned image is not on the heap and must not be deleted. It can be assigned,
  //          used for construction, used in an expression, etc. The compiler will handle
  //          destroying it when it goes out of scope. Of course the pixels may be on the heap
  //          in a pixel buffer, but those buffers are separate from instances of image classes. 
  //          This is not the proper way to do subsampling for image analysis purposes (e.g.
  //          resolution pyramids). For that you need low-pass filtering. The primary purpose
  //          here is to extract Y, U, and V components from a YUV composite image. 
};

//
// *** Template Functions ***
//

template<class T>
MemoryImage<T>::MemoryImage(const MemoryImage& img)
{
  // The copy constructor uses the assignment operator, and relies on the default construction of
  // MemoryImageBase to give operator= a valid l-value to assign to.
  *this = img;
}

template<class T>
MemoryImage<T>& MemoryImage<T>::operator=(const MemoryImage& img)
{
  // Mostly defer to the assignment operator of the base class. Nice!
  MemoryImageBase::operator=(img);
  pixels_ = img.pixels_;
  return *this;
}

template<class T>
MemoryImage<T>::MemoryImage(int wd, int ht)
{
  // Base class does all the work.
  pixels_ = (T*)makeMeOnHeap(wd, ht, sizeof(T));
}

template<class T>
MemoryImage<T>::MemoryImage(T* pixels, int wd, int ht, int rowPitch, int pixelPitch)
{
  // Base class does all the work.
  pixels_ = (T*)makeMeInMemory(pixels, wd, ht, rowPitch, pixelPitch);
}

template<class T>
MemoryImage<T> MemoryImage<T>::window(int x0, int y0, int wd, int ht) const
{
  MemoryImage<T> img;                         // Start with null image, no pixels.
  img.makeMeWindowOf(*this, x0, y0, wd, ht);  // Base class does the work
  img.pixels_ = pixelAddress(x0, y0);         // Just need to get the pixel address.
  return img;                                 // Temp return value is constructed, efficiently.
}

template<class T>
MemoryImage<T> MemoryImage<T>::crudeSubsample(int x0, int y0, int xSample, int ySample) const
{
  MemoryImage<T> img;
  img.makeMeSubsampleOf(*this, x0, y0, xSample, ySample);
  img.pixels_ = pixelAddress(x0, y0);
  return img;
}

// ******************
// *                *
// *  Packed Image  *
// *                *
// ******************
//
// A PackedImage<T> is a MemoryImage<T> where pixelPitch is guaranteed to be 1. It adds no new information to
// the base, just the compile-time guarantee.
template<class T>
class PackedImage : public MemoryImage<T>
{
public:
  // A non-packed image can be made packed by copying the pixels, which is not efficient but sometimes can't
  // be avoided. Here are options for doing the copy when the size of this image might not match the size of
  // the source image.
  enum CopyOptions
  {
    JustPixels, // Copy just those pixels that fit in this image. If this image is null, no pixels are copied.
    Grow,       // All source pixels are copied. If this image is smaller than the source, make a new pixel
                // buffer to hold all of the pixels.
    ExactSize   // All source pixels are copied. If the size of this image is different than the source, make
                // a new pixel buffer to hold all of the pixels. 
  };

private:
  PackedImage(const MemoryImage<T>& img) : MemoryImage<T>(img) { assert(img.pixelPitch() == 1);}
  // effect   Construct a copy of a MemoryImage<T>
  // requires img.pixelPitch() == 1
  // note     A helper function for this class, the public is not allowed to use it.

protected:
  void makeCopyOptions(CopyOptions, int& wd, int& ht);
  // effect   Handle the copy options given the specified size of the source image, and update the size
  //          to be the dimensions that are actually to be copied.

public:
  PackedImage() {}
  // effect   Default construct null image

  PackedImage(int wd, int ht) : MemoryImage<T>(wd, ht) {}
  // effect   Construct new (not yet shared) image on heap of specified size.

  PackedImage(T* pixels, int wd, int ht, int rowPitch) : MemoryImage<T>(pixels, wd, ht, rowPitch) {}
  // effect   Construct new image at specified address in memory, of specified size and pitch.
  // note     Like the constructor in MemoryImage<T>, except that you can't specify pixelPitch, it
  //          must be 1.

  // Default copy constructor and assignment operator allowed; both rely on the base class to do
  // the work.

  PackedImage& makeMeCopyOf(const Image&         , CopyOptions);
  PackedImage& makeMeCopyOf(const MemoryImage<T>&, CopyOptions);
  // returns  This image as l-value, like assignment operators
  // effect   Make me a copy of the specified image by copying pixels. Depending on CopyOptions and
  //          the image sizes, my size might change and pixels currently being shared might be
  //          released.
  // note     The Image source version is not particularly efficient due to the use of virtual
  //          functions to fetch the pixels. The MemoryImage<T> source version is about as
  //          efficient as a C++ copy routine could be.
  //          The implementation of these functions below provides good examples of efficient
  //          pixel loops using memory images.

  PackedImage window(int x0, int y0, int wd, int ht) const { return MemoryImage<T>::window(x0, y0, wd, ht);}
  // returns  An image that is the specified window of this image, sharing pixels.
  // notes    This is effectively identical to the window function of the MemoryImage<T> base class,
  //          just returns a PackedImage<T>. Any window of a packed image is also packed. Providing
  //          this function preserves the compile-time guarantee of pixelPitch() == 1.
};

//
// *** Template Functions ***
//

template<class T>
void PackedImage<T>::makeCopyOptions(CopyOptions co, int& wd, int& ht)
{
  switch (co)
  {
  case JustPixels:
    if (this->width() < wd)
      wd = this->width();
    if (this->height() < ht)
      ht = this->height();
    break;

  case Grow:
    if (wd <= this->width() && ht <= this->height())
      break;

  case ExactSize:
    if (wd != this->width() || ht != this->height())
      *this = PackedImage(wd, ht);
    break;
  }
}

template<class T>
PackedImage<T>& PackedImage<T>::makeMeCopyOf(const Image& img, CopyOptions co)
{
  int wd = img.width();
  int ht = img.height();
  makeCopyOptions(co, wd, ht);

  // Here is a good example of a memory image pixel loop.
  for (int y = 0; y < ht; ++y)
  {
    T* p = this->pixelAddress(0, y);
    for (int x = 0; x < wd; ++x)
      *p++ = (T)img.getPixel(x, y);
  }

  return *this;
}

template<class T>
PackedImage<T>& PackedImage<T>::makeMeCopyOf(const MemoryImage<T>& img, CopyOptions co)
{
  int wd = img.width();
  int ht = img.height();
  makeCopyOptions(co, wd, ht);

   // Here is a good example of an efficient memory image pixel loop.
 for (int y = 0; y < ht; ++y)
  {
    T* src = img.pixelAddress(0, y);
    T* dst =     this->pixelAddress(0, y);
    for (int x = 0; x < wd; ++x, src += img.pixelPitch())
      *dst++ = *src;
  }

  return *this;
}

// Some useful aliases for memory images
typedef MemoryImage<unsigned char> MemoryImage8;
typedef PackedImage<unsigned char> PackedImage8;
typedef MemoryImage<uint16_t> MemoryImage16;
typedef PackedImage<uint16_t> PackedImage16;

// ***************
// *             *
// *  YUV Image  *
// *             *
// ***************

// A YUVImage is a packed image of unsigned 8-bit pixels in the YUV composite format. The Y, U,
// and V components can be extracted as MemoryImages in small constant time. The width of a YUV
// image is guaranteed to be a multiple of 4.

class YUVImage : public PackedImage8
{
  YUVImage(const PackedImage8& img) : PackedImage8(img) {}
  // effect   Construct a copy of a PackedImage8
  // requires width is a multiple of 4
  // note     A helper function for this class, the public is not allowed to use it.

public:
  YUVImage() {}
  // effect   Default construct null image

  YUVImage(int wd, int ht) : PackedImage8(wd & ~3, ht) {}
  // effect   Construct new (not yet shared) image on heap of specified size. The width is forced
  //          to be a multiple of 4 by truncation.

  YUVImage(unsigned char* pixels, int wd, int ht, int rowPitch) : PackedImage8(pixels, wd & ~3, ht, rowPitch) {}
  // effect   Construct new image at specified address in memory, of specified size and pitch.The
  //          width is forced to be a multiple of 4 by truncation. 

  MemoryImage8 yImage() const;
  MemoryImage8 uImage() const;
  MemoryImage8 vImage() const;
  // returns  An image that is the Y, U, or V component of this image, by sharing pixels.
  // notes    The returned image is not packed.
  //          Executes in small constant time.
  //          Takes advantage effecient copy construction of images.
  //          The returned image is not on the heap is must not be deleted. It can be assigned,
  //          used for construction, used in an expression, etc. The compiler will handle
  //          destroying it when it goes out of scope. Of course the pixels may be on the heap in
  //          a pixel buffer, but those buffers are separate from instances of image classes. 

  YUVImage& makeMeCopyOf(const Image&       , CopyOptions);
  YUVImage& makeMeCopyOf(const MemoryImage8&, CopyOptions);
  // note     Just like the ones in PackedImage<T>, but guarantee that the result is a
  //          proper YUVImage (width is a multiple of 4).

  YUVImage window(int x0, int y0, int wd, int ht) const;
  // note     Just like the one in PackedImage<T>, but guarantee that the result is a
  //          proper YUVImage (width is a multiple of 4 and x0 is adjusted to start
  //          on a YUYV boundary).

  // HACK: makes YUVImage look like a proto to keep RoboGrams happy
  void Clear() { return; }
  std::string DebugString() const { return "Not implemented."; }
};

// ***********************
// *                     *
// *  Threshold Image    *
// *                     *
// ***********************

// A ThresholdImage is a packed image of unsigned 16-bit pixels. Every pixel can be one of seven colors.
// Color segmentation is done in acquire_image_fast in src/man/image. ThresholdImage provides helper functions 
// to identify pixels via bitwise operations.

class ThresholdImage : public PackedImage16
{
  ThresholdImage(const PackedImage16& img) : PackedImage16(img) {}
  // effect   Construct a copy of a PackedImage16
  // note     A helper function for this class, the public is not allowed to use it.

public:
  ThresholdImage() {}
  // effect   Default construct null image

  ThresholdImage(int wd, int ht) : PackedImage16(wd, ht) {}
  // effect   Construct new (not yet shared) image on heap of specified size.

  ThresholdImage(uint16_t* pixels, int wd, int ht, int rowPitch) : PackedImage16(pixels, wd, ht, rowPitch) {}
  // effect   Construct new image at specified address in memory, of specified size and pitch.

  bool isGreen(unsigned char threshColor) const { return threshColor & GreenBit; }
  // returns  True if pixel passed in is green, false otherwise

  bool isWhite(unsigned char threshColor) const { return threshColor & WhiteBit; }
  // returns  True if pixel passed in is white, false otherwise

  bool isBlue(unsigned char threshColor) const { return threshColor & BlueBit; }
  // returns  True if pixel passed in is blue, false otherwise

  bool isYellow(unsigned char threshColor) const { return threshColor & YellowBit; }
  // returns  True if pixel passed in is yellow, false otherwise

  bool isOrange(unsigned char threshColor) const { return threshColor & OrangeBit; }
  // returns  True if pixel passed in is orange, false otherwise

  bool isNavy(unsigned char threshColor) const { return threshColor & NavyBit; }
  // returns  True if pixel passed in is navy, false otherwise

  bool isRed(unsigned char threshColor) const { return threshColor & RedBit; }
  // returns  True if pixel passed in is red, false otherwise

  bool isUndefined(unsigned char threshColor) const { return threshColor == UndefinedBit; }
  // returns  True if pixel passed in is undefined, false otherwise

  bool colorsEqual(unsigned char x, unsigned char y) const { return !((x & y) == 0x00); }
  // returns  True if pixels passed in are the same, false otherwise

  enum Colors {
    WhiteBit = 0x01,
    GreenBit = 0x02,
    BlueBit = 0x04,
    YellowBit = 0x08,
    OrangeBit = 0x10,
    RedBit = 0x20,
    NavyBit = 0x40,
    DrawingBit = 0x80, // not sure what this is for, do we need it?
    UndefinedBit = 0x00
  };

  // HACK: makes ThresholdImage look like a proto to keep RoboGrams happy
  void Clear() { return; }
  std::string DebugString() const { return "Not implemented."; }
};
}

#endif
