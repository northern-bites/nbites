/**
 * \file  common.h
 * \brief Describes some common information shared between color
 * classification and ground truth detection
 *
 * \author  Piyush Khandelwal (piyushk), piyushk@cs.utexas.edu
 * Copyright (C) 2011, The University of Texas at Austin, Piyush Khandelwal
 *
 * License: Modified BSD License
 *
 * $ Id: 08/10/2011 03:07:07 PM piyushk $
 */

#ifndef COMMON_29E2EKDP
#define COMMON_29E2EKDP

#include <stdint.h>

namespace color_table {

  struct Rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  struct Yuv {
    uint8_t y;
    uint8_t u;
    uint8_t v;
  };

  typedef uint8_t ColorTable[128][128][128];

  enum Color {
    UNDEFINED,
    ORANGE,
    PINK,
    BLUE,
    GREEN,
    WHITE,
    YELLOW,
    NUM_COLORS
  };

  const unsigned int IMAGE_HEIGHT = 480;
  const unsigned int IMAGE_WIDTH = 640;
}

#endif /* end of include guard: COMMON_29E2EKDP */
