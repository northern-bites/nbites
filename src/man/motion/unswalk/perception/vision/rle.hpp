#pragma once

#include <stdint.h>
#include "perception/vision/VisionDefs.hpp"

/**
 * Performs run-length encoding/decoding on a saliency scan
 **/
namespace rle {
   /* Turns a saliency image into an RLE stream
    * @param output the buffer where the encoding is to be stored
    * @param saliency the saliency scan, sizes assumed from VisionDefs.hpp
    * @return number of bytes written to output buffer
    **/
   uint16_t encode(uint8_t *output, Colour *saliency);

   /* Turns a saliency image into an RLE stream
    * @param saliency the buffer where the saliency scan is to be stored
    * @param input the compressed stream
    * @param size the size of the compressed stream
    **/
   void decode(Colour *saliency, uint8_t *input, uint16_t size);
}

