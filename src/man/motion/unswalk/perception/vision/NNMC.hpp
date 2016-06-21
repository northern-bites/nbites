#pragma once

#include <stdint.h>
#include <boost/shared_array.hpp>

#include "VisionDefs.hpp"

class NNMC
{

   public:
      NNMC();
      /**
       * Classifies a pixel of a YUV422 image
       * given a colour calibration file
       * @param pixel address of Y component of pixel. 
       *              Assumes pixel is 0-byte offset
       * @param nnmc the classification table
       * @return the classified colour of the yuv values
       **/
      inline const Colour classifyYU_V(const uint8_t *const pixel) const;

      /**
       * Classifies a pixel of a YUV422 image
       * given a colour calibration file
       * @param pixel address of Y component of previous pixel.
       *              Assumes pixel is 2-byte offset
       * @param nnmc the classification table
       * @return the classified colour of the yuv values
       **/
      inline const Colour classify_UYV(const uint8_t *const pixel) const;

      /**
       * Classifies a pixel of a YUV422 image
       * given a colour calibration file
       * @param pixel address of Y component of pixel.
       *              Assumes pixel is 0-byte offset.
       * @param nnmc the classification table
       * @return the classified colour of the yuv values
       **/
      inline const Colour classifyYV_U(const uint8_t *const pixel) const;

      /**
       * Classifies a pixel of a YUV422 image
       * given a colour calibration file. Automatically chooses classification
       * function based on pixel alignment. If pixel alignment is known, then
       * it is faster to explicitly call classify_UYV or classifyYV_U
       * @param pixel address of Y component of pixel.  assumes image is 4-byte
       * @param nnmc the classification table
       * @return the classified colour of the yuv values
       **/
      inline const Colour classify(const uint8_t *const pixel) const;

      /**
       * Classifies a pixel of a YUV422 image
       * given a colour calibration file. This method of classifying a pixel
       * requires a significant number of arithmetic operations. 
       * @param y    y value of pixel
       * @param u    u value of pixel
       * @param v    v value of pixel
       * @return the classified colour of the yuv values
       **/
      inline const Colour classify(uint8_t y, uint8_t u, uint8_t v) const;

      /**
       * Loads the specified calibration file. Or the default
       * @param filename file to be loaded
       **/
      void load(const char *filename);

      /**
       * Unload the calibration and free up memory
       */
      void unload();

      /**
       * Returns true if the calibration is loaded
       */
      bool isLoaded() const;


   private:
      /**
       * Calibration table optimized for saliency scan
       */
      boost::shared_array<uint8_t> nnmc;

   friend class CalibrationTab;
};

#include "NNMCinline.hpp"

