#include "perception/vision/rle.hpp"

namespace rle {
   uint16_t encode(uint8_t *output, Colour *saliency) {
      Colour lastcolour = saliency[0];
      output[0] = (uint8_t)lastcolour;
      uint16_t outpos = 0;
      uint8_t run = 1;
      for (uint16_t i = 0;
            // File isn't used, so changed SALIENCY_DENSITY
            // to BOT_SALIENCY_DENSITY to make it compile :)
            i < (IMAGE_COLS/BOT_SALIENCY_DENSITY)*
                (IMAGE_ROWS/BOT_SALIENCY_DENSITY);
            ++i) {
         if (lastcolour != saliency[i] || run == 255) {
            // different colour, or number too big, start a new run
            output[outpos++] = (uint8_t)lastcolour;
            output[outpos++] = run;
            lastcolour = saliency[i];
            run = 1;
         } else {
            // the run continues
            ++run;
         }
      }
      output[outpos++] = (uint8_t)lastcolour;
      output[outpos++] = run;
      return outpos;
   }

   void decode(Colour *saliency, uint8_t *input, uint16_t size) {
      uint16_t inpos = 0;
      uint16_t saliencypos = 0;
      while (inpos < size) {
         Colour c = (Colour)input[inpos++];
         uint8_t run = input[inpos++];
         for (uint8_t i = 0; i < run; ++i) {
            saliency[saliencypos++] = c;
         }
      }
   }
}

