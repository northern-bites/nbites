#include "UNSWCamera.hpp"

class NullCamera : public UNSWCamera
{
   public:
      NullCamera() {}
      virtual ~NullCamera() {}

      /**
       * Get the dummy frame
       * @return the dummy frame
       */
      virtual const uint8_t *get(const int colourSpace = AL::kYUVColorSpace)
      {
         return frame;
      }
      

      /**
       * Set the dummy frame returned by get
       * @param frame the dummy frame
       */
      void set(const uint8_t *frame)
      {
         this->frame = frame;
      }

      virtual bool setCamera(WhichCamera whichCamera)
      {
         this->whichCamera = whichCamera;
         return true;
      }

      virtual WhichCamera getCamera()
      {
         return this->whichCamera;
      }

      virtual bool setControl(const uint32_t id, const int32_t value)
      {
         return false; 
      }

   private:
      /* The frame returned by get */
      WhichCamera whichCamera;
      const uint8_t *frame;

};

