#pragma once

#include "NNMC.hpp"

inline const Colour NNMC::classify(uint8_t y, uint8_t u, uint8_t v) const
{

   return (Colour) 
      nnmc.get()[((v & ~1) << (MAXY_POW + MAXU_POW - 1)) +
                 ((u & ~1) << (MAXY_POW            - 1)) + 
                  (y >> 1)
                ];
}

inline const Colour NNMC::classify(const uint8_t *const pixel) const
{
   if ((size_t)pixel & 0x2)
      return classifyYV_U(pixel);
   else
      return classifyYU_V(pixel);
}

// TODO(carl): write big endian version
inline const Colour NNMC::classifyYU_V(const uint8_t *const pixel) const
{
   return classify(pixel[0], pixel[1], pixel[3]);
}

// TODO(carl): write big endian version
inline const Colour NNMC::classify_UYV(const uint8_t *const pixel) const
{
   return classify(pixel[2], pixel[1], pixel[3]);
}

// TODO(carl): write big endian version
inline const Colour NNMC::classifyYV_U(const uint8_t *const pixel) const
{
   return classify(pixel[0], pixel[3], pixel[1]);
}

