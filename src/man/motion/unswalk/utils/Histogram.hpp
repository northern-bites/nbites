#pragma once

#include <iostream>

template <typename T, int channels>
struct Histogram
{
   const int size;

   Histogram(int size);
   virtual ~Histogram();

   /**
    * Counts of entries into histogram
    */
   inline int counts(int i, int channel = 0) const
   {
      return _counts[i][channel];
   }

   /**
    * Windowed average of counts. Call applyWindowFilter to generate
    */
   inline int window(int i, int channel = 0) const
   {
      return _window[i][channel];
   }

   /**
    * List of peak indicies in window, terminated by -1
    */
   inline int peaks(int i, int channel = 0) const
   {
      return _peaks[i][channel];
   }

   T   (*_counts)[channels];
   T   (*_window)[channels];
   int (*_peaks )[channels];

   /**
    * Apply sliding window filter, peak indicies are outputted to peaks
    */
   void applyWindowFilter(
         const int windowSize,
         unsigned int channel_mask = 0xFFFFFFF);

   /**
    * Apply sliding window filter, peak indicies are outputted to peaks.
    * Ends of histogram are looped around
    */
   void applyWindowFilterLoop(
         const int windowSize,
         unsigned int channel_mask = 0xFFFFFFFF);

   /**
    * Find strongest peak
    */
   int maxPeak(int channel = 0) const;

   /**
    * Find strongest count
    */
   int maxCount(int channel = 0) const;

   template <typename _T, int _channels>
   friend std::ostream& operator << (
        std::ostream &out,
        const Histogram<_T, _channels> &h);
};

template <typename T, int channels>
std::ostream& operator <<(std::ostream &out, const Histogram<T, channels> &h);

#include "Histogram.tcc"

