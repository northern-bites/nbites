/* Bresenham Line Algorithm 
 *
 * Use for efficiently tracing lines across 2D buffers
 */

#include "types/Point.hpp"
#include <math.h>

template <typename T>
class BresenhamPtr
{
   public:
      /* Assume that data is stored as data[x][y]. Counter intuitively, this
       * means that the data is stored as
       *
       * y0x0 y1x0 ...
       * y0x1  .
       * y0x2     .
       *  |          .
       *
       *
       * @param data    base pointer to data
       * @param start   start 2d index of ptr
       * @param end     end 2d index of ptr
       * @param step    width of fixed parameter of array
       */
      BresenhamPtr(T *data, Point start, Point end, size_t step) : start(start)
      {

         int x, y;

         /* Reverse the assignments so we can think in standard graph coords */
         y = end.x() - start.x();
         x = end.y() - start.y();

         if (x > 0) {
            dx = x;
            incx = 1;
         } else {
            dx = -x;
            incx = -1;
         }

         if (y > 0) {
            dy = y;
            incy = step;
         } else {
            dy = -y;
            incy = -step;
         }

         if (dx >= dy) {
            dy <<= 1;
            balance = dy - dx;
            dx <<= 1;
         } else {
            dx <<= 1;
            balance = dx - dy;
            dy <<= 1;
         }

         start_ptr = data + start.x() * step + start.y();
         end_ptr = data + end.x() * step + end.y();
      }

      class iterator
      {
         public:
            iterator &operator ++()
            {
               if (dx >= dy) {
                  if (balance >= 0) {
                     ptr += incy;
                     balance -= dx;
                     if (incy >= 0) {
                        ++ p.x();
                     } else {
                        -- p.x();
                     }
                  }
                  ptr += incx;
                  balance += dy;
                  if (incx >= 0) {
                     ++ p.y();
                  } else {
                     -- p.y();
                  }
               } else {
                  if (balance >= 0) {
                     ptr += incx;
                     balance -= dy;
                     if (incx >= 0) {
                        ++ p.y();
                     } else {
                        -- p.y();
                     }
                  }
                  ptr += incy;
                  balance += dx;
                  if (incy >= 0) {
                     ++ p.x();
                  } else {
                     -- p.x();
                  }
               }
               return *this;
            }

            bool operator ==(const iterator &o)
            {
               return ptr == o.ptr;
            }

            bool operator ==(T *optr)
            {
               return ptr == optr;
            }

            bool operator !=(const iterator &o)
            {
               return ! (ptr == o.ptr);
            }

            bool operator !=(T *optr)
            {
               return ! (ptr == optr);
            }


            T operator *()
            {
               return *ptr;
            }

            T* operator ->()
            {
               return get();
            }

            T* get() 
            {
               return ptr;
            }

            Point point() const
            {
               return p;
            }

            iterator() {}

         private:
            iterator(T *ptr, Point p, int dx, int dy, int incx, int incy, int balance)
               : ptr(ptr), s(ptr), p(p), dx(dx), dy(dy),
                 incx(incx), incy(incy), balance(balance)
            {}

            T *ptr, *s;
            Point p;
            int dx, dy;
            int incx, incy;
            int balance;

            friend class BresenhamPtr;
      };

      iterator begin()
      {
         return iterator(start_ptr, start, dx, dy, incx, incy, balance);
      }

      /* This function is an optimization hack. Instead of constructing a new
       * iterator, simply return the pointer which is compared. The == and !=
       * methods of an iterator have been implemented to work with raw
       * pointers
       */
      T *end()
      {
         return end_ptr;
      }

   private:
      T *start_ptr, *end_ptr;
      Point start;
      int dx, dy;
      int incx, incy;
      int balance;
};
