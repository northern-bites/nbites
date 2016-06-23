#include <math.h>

#include <limits>
#include <iomanip>
#include <typeinfo>

template <typename T, int channels>
Histogram<T, channels>::Histogram(int size)
   : size(size)
{
   _counts = new T  [size][channels];
   _window = new T  [size][channels];
   _peaks  = new int[size][channels];

   int c;
   for (c = 0; c < channels; ++ c) {
      _peaks [0][c] = -1;
   }
}

template <typename T, int channels>
Histogram<T, channels>::~Histogram()
{
   delete[] _counts;
   delete[] _window;
   delete[] _peaks;
}

template <typename T, int channels>
void Histogram<T, channels>::applyWindowFilter(
      const int _windowSize,
      unsigned int channel_mask)
{
   if (_windowSize >= size) {
      return;
   }

   const int lag  = _windowSize / 2;
   const int lead = _windowSize - lag;

   int  i, channel;
   int  peak;
   int  last_was_peak;
   bool last_was_half_peak;
   T    cumm, cumm_prev;

   for (channel = 0; channel < channels; ++ channel) {
      if (! (channel_mask & 1 << channel)) {
         continue;
      }
      cumm = 0;
      peak = 0;
      last_was_peak = 0;
      /* Initialising last_was_half_peak to 1 causes half peak locations
       * to be rounded up.
       * If a peak is 2 wide, then the peak will fall on the 2nd pixel.
       * If a peak is 3 wide, then the peak will also fall on the 2nd pixel.
       * If a peak is 4 wide, then the peak will fall on the 3rd pixel.
       * and so on
       */
      last_was_half_peak = 1;

      for (i = 0; i < _windowSize - lead; ++ i) {
         _window[i][channel] = 0;
      }

      for (i = 0; i < _windowSize; ++ i) {
         cumm += _counts[i][channel];
      }
      cumm_prev = _window[i - lead][channel] = cumm;

      do {
         cumm += _counts[i][channel];
         cumm -= _counts[i - _windowSize][channel];

         ++ i;

         _window[i - lead][channel] = cumm;

         if (cumm_prev < cumm) {
            _peaks[peak][channel] = i - lead;
            last_was_peak = 1;
            last_was_half_peak = true;
         } else if (cumm_prev == cumm) {
            if (last_was_half_peak) {
               _peaks[peak][channel] = i - lead;
            }
            last_was_half_peak = ! last_was_half_peak;
            /* Note that at this point, it may seem logical that
             * we should also set
             * last_was_peak = 1;
             * However, in order to reach this branch we must 
             * first also have entered the '<' branch, and therefore
             * last_was_peak is already set. The one exception is
             * when the entire histogram is uniform. In this case,
             * last_was_peak will not be set. However, if the entire
             * array is uniform, then it reasons that there are no
             * _peaks
             */
         } else {
            peak += last_was_peak;
            last_was_peak = 0;
         }
         cumm_prev = cumm;

      } while (i < size);

      for (i = size - lead + 1; i < size; ++ i) {
         _window[i][channel] = 0;
      }

      _peaks[peak][channel] = -1;
   }
}

template <typename T, int channels>
void Histogram<T, channels>::applyWindowFilterLoop(
      const int _windowSize,
      unsigned int channel_mask)
{
   if (_windowSize >= size) {
      return;
   }

   const int lag  = _windowSize / 2;
   const int lead = _windowSize - lag;

   int  i, channel;
   int  peak, peak_tmp;
   int  last_was_peak;
   bool last_was_half_peak;
   T    cumm, cumm_prev;

   for (channel = 0; channel < channels; ++ channel) {
      if (! (channel_mask & 1 << channel)) {
         continue;
      }
      cumm = 0;
      peak = 0;
      for (i = size - lag; i < size; ++ i) {
         cumm += _counts[i][channel];
      }
      for (i = 0; i < lead; ++ i) {
         cumm += _counts[i][channel];
      }
      _window[0][channel] = cumm; /* _window[i - lead] */

      /* We must initialize the peak search sequence here. This requires
       * calculating the value of the previous _window
       */
      last_was_peak = 0;
      cumm_prev  = cumm;
      cumm_prev += _counts[size - lag - 1][channel];
      cumm_prev -= _counts[lead - 1][channel];

      if (cumm_prev < cumm) {
         _peaks[peak][channel] = 0;
         last_was_peak = 1;
      }

      last_was_half_peak = true;
      
      cumm_prev = cumm;

      while (i < _windowSize) {
         cumm += _counts[i][channel];
         cumm -= _counts[i + size - _windowSize][channel];

         ++ i;

         _window[i - lead][channel] = cumm;

         if (cumm_prev < cumm) {
            _peaks[peak][channel] = i - lead;
            last_was_peak = 1;
            last_was_half_peak = true;
         } else if (cumm_prev == cumm) {
            if (last_was_half_peak) {
               _peaks[peak][channel] = i - lead;
            }
            last_was_half_peak = ! last_was_half_peak;
         } else {
            peak += last_was_peak;
            last_was_peak = 0;
         }

         cumm_prev = cumm;
      } 

      while (i < size) {
         cumm += _counts[i][channel];
         cumm -= _counts[i - _windowSize][channel];

         ++ i;

         _window[i - lead][channel] = cumm;

         if (cumm_prev < cumm) {
            _peaks[peak][channel] = i - lead;
            last_was_peak = 1;
            last_was_half_peak = true;
         } else if (cumm_prev == cumm) {
            if (last_was_half_peak) {
               _peaks[peak][channel] = i - lead;
            }
            last_was_half_peak = ! last_was_half_peak;
         } else {
            peak += last_was_peak;
            last_was_peak = 0;
         }

         cumm_prev = cumm;
      }

      while (i < size + lead - 1) {
         cumm += _counts[i - size][channel];
         cumm -= _counts[i - _windowSize][channel];

         ++ i;

         _window[i - lead][channel] = cumm;

         if (cumm_prev < cumm) {
            _peaks[peak][channel] = i - lead;
            last_was_peak = 1;
            last_was_half_peak = true;
         } else if (cumm_prev == cumm) {
            if (last_was_half_peak) {
               ++ _peaks[peak][channel];
            }
            last_was_half_peak = ! last_was_half_peak;
         } else {
            peak += last_was_peak;
            last_was_peak = 0;
         }

         cumm_prev = cumm;
      }

      /* Test final element for peak */
      cumm = _window[size - 1][channel];
      if (last_was_peak) {
         if (_window[0][channel] < cumm) {
            _peaks[peak ++][channel] = size - 1;
         } else if (_window[0][channel] == cumm) {
            /* start from the beginning again */
            i = 0;
            for (i = 0; _window[i][channel] == cumm; ++ i) {
               if (last_was_half_peak) {
                  ++ _peaks[peak][channel];
               }
               last_was_half_peak = ! last_was_half_peak;
            }
            /* Only increment if we actually found a peak */
            if (cumm > _window[i][channel]) {
               /* To keep everything sorted, move the final peak
                * to the beginning of the array
                */
               if (_peaks[peak][channel] >= size) {
                  peak_tmp = _peaks[peak][channel];
                  for (i = 0; i < peak; ++ i) {
                     _peaks[i + 1][channel] = _peaks[i][channel];
                  }
                  _peaks[0][channel] = peak_tmp - size;
               }
               ++ peak;
            }
         }
      }
      _peaks[peak][channel] = -1;
   }
}

template <typename T, int channels>
int Histogram<T, channels>::maxPeak(int channel) const
{
   int ret = -1;
   T best = std::numeric_limits<T>::min();

   int i;
   for (i = 0; _peaks[i][channel] != -1 ; ++ i) {
      int val = _window[_peaks[i][channel]][channel];
      if (best < val) {
         ret = _peaks[i][channel];
         best = val;
      }
   }

   return ret;
}

template <typename T, int channels>
int Histogram<T, channels>::maxCount(int channel) const
{
   int ret = -1;
   T best = std::numeric_limits<T>::min();

   int i;
   for (i = 0; i < size; ++ i) {
      if (best < _counts[i][channel]) {
         ret = i;
         best = _counts[i][channel];
      }
   }

   return ret;
}

template <typename T, int channels>
std::ostream& operator <<(std::ostream &out, const Histogram<T, channels> &h)
{
   std::ios_base::fmtflags save_flags = out.flags();
   int                     save_fill  = out.fill ();

   int i, c;
   int spacing;

   /* Yes, this breaks for other floating point types. Fix it if
    * you ever need it (carl)
    */
   if (typeid(T) == typeid(float) || typeid(T) == typeid(double)) {
      spacing = 6;
   } else {
      int globalmax = 0;
      for (c = 0; c < channels; ++ c) {
         for (i = 0; i < h.size; ++ i) {
            if (h._counts[i][c] > globalmax) {
               globalmax = h._counts[i][c];
            }
            if (h._window[i][c] > globalmax) {
               globalmax = h._counts[i][c];
            }
         }
      }
      if (globalmax < 2) {
         spacing = 3;
      } else {
         spacing = int(log(globalmax) / log(10)) + 2;
      }
   }

   for (c = 0; c < channels; ++ c) {
      bool has_val = false;
      for (i = 0; i < h.size && ! has_val; ++ i) {
         has_val = h._counts[i][c] != 0;
      }
      if (! has_val) {
         continue;
      }

      /* Print channel info */
      out << "channel ";
      out << c << std::setw(0) << ":" << std::endl;

      /* Print counts */
      out << "counts:";

      for (i = 0; i < h.size; ++ i) {
         out << std::setw(spacing) << h._counts[i][c];
      }
      out << std::endl;

      /* Print window */
      out << "window:";
      for (i = 0; i < h.size; ++ i) {
         out << std::setw(spacing) << h._window[i][c];
      }
      out << std::endl;

      /* Print peaks */
      out << "peaks :";
      for (i = 0; h._peaks[i][c] != -1; ++ i) {
         out << std::setw(spacing) << h._peaks[i][c];
      }
      out << std::endl;
   }

   out.flags(save_flags);
   out.fill (save_fill );

   return out;
}

