#pragma once

#include "perception/vision/Vision.hpp"
#include "perception/vision/VisionDefs.hpp"

// Just uses top image for now
inline Colour Vision::getColour(uint16_t row, uint16_t col) {
   uint8_t const* pixelpair = topFrame +
      ((row * IMAGE_COLS + ((col >> 1) << 1)) << 1);
   return nnmc.classify(pixelpair[(col & 1) << 1], pixelpair[1], pixelpair[3]);
}

inline PixelValues Vision::getPixelValues(uint16_t row, uint16_t col) {
   uint8_t const* pixelPair = topFrame +
      ((row * IMAGE_COLS + ((col >> 1) << 1)) << 1);
   PixelValues values;
   values.y = pixelPair[(col & 1) << 1];
   values.u = pixelPair[1];
   values.v = pixelPair[3];
   return values;
}
