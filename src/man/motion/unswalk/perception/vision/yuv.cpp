#include "yuv.hpp"

uint8_t* getpixelpair(const uint8_t* yuv, int row, int col, int num_cols) {
   return const_cast<uint8_t *>(yuv)
      + (row * num_cols + col / 2 * 2) * 2;
}

uint8_t gety(const uint8_t* yuv, int row, int col, int num_cols) {
   return getpixelpair(yuv, row, col, num_cols)[2 * (col & 1)];
}

uint8_t getu(const uint8_t* yuv, int row, int col, int num_cols) {
   return getpixelpair(yuv, row, col, num_cols)[1];
}

uint8_t getv(const uint8_t* yuv, int row, int col, int num_cols) {
   return getpixelpair(yuv, row, col, num_cols)[3];
}
