/* A timer class
 **/

#pragma once

#include <sys/time.h>
#include <stdint.h>
#include <time.h>


//  timer  -------------------------------------------------------------------//

//  A timer object measures elapsed time.

//  It is recommended that implementations measure wall clock rather than CPU
//  time since the intended use is performance measurement on systems where
//  total elapsed time is more important than just process or CPU time.

//  Warnings: The maximum measurable elapsed time may well be only 596.5+ hours
//  due to implementation limitations.  The accuracy of timings depends on the
//  accuracy of timing information provided by the underlying platform, and
//  this varies a great deal from platform to platform.

class Timer {
   public:
      Timer() {
         // _start_time = std::clock();
         gettimeofday(&timeStamp, NULL);
      }  // postcondition: elapsed()==0
      void  restart() {
         // _start_time = std::clock();
         gettimeofday(&timeStamp, NULL);
      }  // post: elapsed()==0

      float sec(const timeval& t) {
         return t.tv_sec + t.tv_usec / 1000000.0;
      }
      float msec(const timeval& t) {
         return t.tv_sec * 1000 + t.tv_usec / 1000.0;
      }

      float usec(const timeval& t) {
         return t.tv_sec * 1000000 + t.tv_usec;
      }

      /* return elapsed time in seconds */
      float elapsed() {
         timeval tmp;
         gettimeofday(&tmp, NULL);
         return sec(tmp) -
                sec(timeStamp);
      }

      uint32_t elapsed_ms() {
         timeval tmp;
         gettimeofday(&tmp, NULL);
         return msec(tmp) - msec(timeStamp);
      }

      uint32_t elapsed_us() {
         timeval tmp;
         gettimeofday(&tmp, NULL);
         return usec(tmp) - usec(timeStamp);
      }

      /* return estimated maximum value for elapsed() */
      float elapsed_max() {
         /* Portability warning: elapsed_max() may return too high a value on systems
          * where std::clock_t overflows or resets at surprising values.
          */
         return elapsed();
      }

      /* return minimum value for elapsed() */
      float elapsed_min() {
         return elapsed();
      }

   private:
      timeval timeStamp;
};

