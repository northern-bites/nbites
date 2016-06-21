#include "ColorReference.h"
#include "Tools/ImageProcessing/ColorModelConversions.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Util/snappy/include/snappy-c.h"

/**
 * A table that maps YCbCr color values to HSI color values.
 * The resolution of this table is the one used by the ColorReferenceBH class.
 */
static class ColorSpaceMapper
{
private:
  ImageBH::Pixel table[ColorReferenceBH::colorTableSize][ColorReferenceBH::colorTableSize][ColorReferenceBH::colorTableSize];

public:
  ColorSpaceMapper()
  {
    ImageBH::Pixel* p = &table[0][0][0];
    for(int y = 0; y < ColorReferenceBH::colorTableSize; ++y)
      for(int cb = 0; cb < ColorReferenceBH::colorTableSize; ++cb)
        for(int cr = 0; cr < ColorReferenceBH::colorTableSize; ++cr)
        {
          ColorModelConversions::fromYCbCrToHSI((unsigned char) (y << ColorReferenceBH::shiftFactor),
                                                (unsigned char) (cb << ColorReferenceBH::shiftFactor),
                                                (unsigned char) (cr << ColorReferenceBH::shiftFactor),
                                                p->h, p->s, p->i);
          ++p;
        }
  }

  const ImageBH::Pixel& fromYCbCrToHSI(const ImageBH::Pixel& yCbCr) const
  {
    return table[yCbCr.y >> ColorReferenceBH::shiftFactor]
                [yCbCr.cb >> ColorReferenceBH::shiftFactor]
                [yCbCr.cr >> ColorReferenceBH::shiftFactor];
  }
} colorSpaceMapper;

ColorReferenceBH::MultiColor ColorReferenceBH::getColorClasses(const ImageBH::Pixel* pixel) const
{
  return colorTable[pixel->y>>shiftFactor]
                   [pixel->cb>>shiftFactor]
                   [pixel->cr>>shiftFactor];
}

ColorReferenceBH::MultiColor ColorReferenceBH::getColorClassesFromHSI(const ImageBH::Pixel& pixel) const
{
  const ImageBH::Pixel& hsi = colorSpaceMapper.fromYCbCrToHSI(pixel);
  float h = hsi.h * pi2/ 255.f;
  float s = hsi.s / 255.f;
  float v = hsi.i / 255.f;

  MultiColor multiColor(0);

  if(thresholdGreen.hue.isInside(h) &&
     thresholdGreen.saturation.isInside(s) &&
     thresholdGreen.value.isInside(v))
  {
    multiColor.colors |= green;
  }

  if(thresholdOrange.hue.isInside(h) &&
     thresholdOrange.saturation.isInside(s) &&
     thresholdOrange.value.isInside(v))
  {
    multiColor.colors |= orange;
  }

  if(thresholdYellow.hue.isInside(h) &&
     thresholdYellow.saturation.isInside(s) &&
     thresholdYellow.value.isInside(v))
  {
    multiColor.colors |= yellow;
  }

  if(thresholdBlue.hue.isInside(h) &&
     thresholdBlue.saturation.isInside(s) &&
     thresholdBlue.value.isInside(v))
  {
    multiColor.colors |= blue;
  }

  if(thresholdRed.hue.isInside(h) &&
     thresholdRed.saturation.isInside(s) &&
     thresholdRed.value.isInside(v))
  {
    multiColor.colors |= red;
  }

  if(!multiColor.isGreen())
  {
    unsigned char r, g, b;
    ColorModelConversions::fromYCbCrToRGB(pixel.y, pixel.cb, pixel.cr, r, g, b);
    if(r >= thresholdWhite.first && b >= thresholdWhite.second && r + b >= thresholdWhite.third)
      multiColor.colors |= white;
  }

  if(pixel.cb == thresholdBlack.first && pixel.cr == thresholdBlack.second && pixel.y <= thresholdBlack.third)
    multiColor.colors |= black;

  return multiColor;
}

void ColorReferenceBH::update()
{
  ImageBH::Pixel p;

  for(int y = 0; y < colorTableSize; ++y)
  {
    for(int cb = 0; cb < colorTableSize; ++cb)
    {
      for(int cr = 0; cr < colorTableSize; ++cr)
      {
        p.y   = (unsigned char)(y<<shiftFactor);
        p.cb  = (unsigned char)(cb<<shiftFactor);
        p.cr  = (unsigned char)(cr<<shiftFactor);

        ColorReferenceBH::MultiColor colors = getColorClassesFromHSI(p);
        colorTable[y][cb][cr] = colors.colors;
      }
    }
  }
}

void ColorReferenceBH::serialize(In* in, Out* out)
{
  STREAM_REGISTER_BEGIN;
  size_t ctUncompressedSize = (int) pow(colorTableSize*sizeof(colorTable[0][0][0]), 3);
  size_t ctCompressedSize = 0;
  std::vector<char> ctCompressed;

  if(out != NULL)
  {
    // setup buffer
    ctCompressedSize = snappy_max_compressed_length(ctUncompressedSize);
    ctCompressed.resize(ctCompressedSize);

    // compress
    const snappy_status status =
      snappy_compress((char*) &colorTable[0][0][0], ctUncompressedSize, ctCompressed.data(), &ctCompressedSize);
    VERIFY(status == SNAPPY_OK);

    // stream
    out->write(&ctCompressedSize, sizeof(int));
    out->write(ctCompressed.data(), ctCompressedSize);
  }
  else if(in != NULL)
  {
    // setup buffer
    in->read(&ctCompressedSize, sizeof(int));
    ctCompressed.resize(ctCompressedSize);

    // stream
    in->read(ctCompressed.data(), ctCompressedSize);

    // uncompress
    const snappy_status status =
      snappy_uncompress(ctCompressed.data(), ctCompressedSize, (char*) &colorTable[0][0][0], &ctUncompressedSize);
    VERIFY(status == SNAPPY_OK);
  }
  else
    ASSERT(false);

  STREAM(thresholdGreen);
  STREAM(thresholdYellow);
  STREAM(thresholdOrange);
  STREAM(thresholdRed);
  STREAM(thresholdBlue);
  STREAM(thresholdWhite);
  STREAM(thresholdBlack);
  STREAM_REGISTER_FINISH;
}
