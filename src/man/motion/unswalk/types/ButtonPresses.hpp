#pragma once

#include <stdint.h>

class ButtonPresses {
   public:
      ButtonPresses() : mask(0) {}
      bool pop(int n) {
         bool ret = peek(n);
         mask &= ~(1 << (n - 1));
         return ret;
      }

      bool peek(int n) {
         return mask & (1 << (n - 1));
      }

      void push(int n) {
         mask |= 1 << (n - 1);
      }

      void operator|=(const ButtonPresses& b) {
         mask |= b.mask;
      }

      void clear() {
         mask = 0;
      }

   private:
      uint8_t mask;
};
