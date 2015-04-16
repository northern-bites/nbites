#ifndef _IMAGE_ACQUISITION_H_
#define _IMAGE_ACQUISITION_H_

#include <stdint.h>
#include <iostream>

#define S16_15(n) ((short)((((n) << 15) + ((n) >= 0 ? 50 : -50)) / 100))

class ColorParams;

// Stuct to store parameters for max and min values, widths, and Y coefficients used in U and V comparions
// All U and V vales are s16.1 (signed 16 bit integers with one bit to the right of the binary point)
// All coefficients are s16.15 (signed 16 bit integers with 15 bits to the right of the binary point)
struct ColorClassificationValues {
    short whiteWidth;   // Width of fuzzy thresholds for all whiteness U and V comparisons 
    short whiteU;       // Begining of fuzzy threshold for U values. Increase to make more tolerant
    short whiteV;       // Begining of fuzzy threshold for V values. Increase to make more tolerant
    short whiteCoef;    // Degree to which Y values impact U and V thresholds (s16.15 between 0 and 1)

    short orangeWidth;  // Width of fuzzy thresholds for U and V comparisons for orange calculations
    short orangeMaxU;   // Begining of fuzzy threshold for U value
    short orangeMinV;   // Begining of fuzzy threshold for V value
    short orangeCoefU;  // Degree to which Y values impact the max U value
    short orangeCoefV;  // Degree to which Y values impact the min V value

    short greenWidth;   // Width of fuzzy thresholds for U and V comparisons for green calculations
    short greenMaxU;    // Begining of fuzzy threshold for U value
    short greenMaxV;    // Begining of fuzzy threshold for V value
    short greenCoef;    // Degree to which Y values impact U and V values

    ColorClassificationValues() {
        whiteWidth = 20;
        whiteU = -10 << 1;
        whiteV = -10 << 1;    
        whiteCoef = S16_15(20);  // .2

        orangeWidth = 30;
        orangeMaxU = -20 << 1;
        orangeMinV = 22 << 1;
        orangeCoefU = S16_15(10); 
        orangeCoefV = S16_15(10);

        greenWidth = 30;
        greenMaxU = 18 << 1;
        greenMaxV = 18 << 1;
        greenCoef = S16_15(-12); // -.12
    }
};

extern "C" int _acquire_image(int rowCount,
                              int colCount,
                              int rowPitch,
                              const uint8_t *yuv,
                              uint8_t *out,
                              ColorClassificationValues *colors );

extern "C" int _copy_image(const uint8_t *camera, uint8_t *local);

namespace ImageAcquisition {
    int acquire_image(int rowCount, int colCount,int rowPitch,
                            const uint8_t *yuv, uint8_t *out, ColorClassificationValues *colors );
};

#endif
