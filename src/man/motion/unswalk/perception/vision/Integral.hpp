
#pragma once

#include <algorithm>  // req'd for std::min/max
#include "perception/vision/VisionDefs.hpp"
#include "perception/vision/VisionConstants.hpp"
#include <perception/vision/VisionFrame.hpp>

//! Computes the 1d integral image of the specified horizon line 
//! in image img.  Assumes source image to be a 32-bit floating point.  Returns IplImage in 32-bit float form.
void Integral(float* result, const VisionFrame *source, int left_horizon, int right_horizon);

// Convert horizon of image to single channel 32F
void getGrayHorizon(float* data, const VisionFrame *frame_p, int left_horizon, int right_horizon);

//! Computes the sum of pixels within the row specified by the left start
//! co-ordinate and width
inline float BoxIntegral(float *data, int col, int cols) 
{

  // The subtraction by one for col because col is inclusive.
  int c1 = std::min(col,          TOP_IMAGE_COLS/SURF_SUBSAMPLE)  - 1;
  int c2 = std::min(col + cols,   TOP_IMAGE_COLS/SURF_SUBSAMPLE)  - 1;

  float A(0.0f), B(0.0f);
  if (c2 >= 0) A = data[c2];
  if (c1 >= 0) B = data[c1];

  return std::max(0.f, A - B);
}

