#include "Parameters.hpp"

template<> template<>
Parameters<float> Parameters<fadbad::F<float> >::cast<float>()
{
   Parameters<float> casted;

   casted.cameraPitchTop    = cameraPitchTop.x();
   casted.cameraYawTop      = cameraYawTop.x();
   casted.cameraRollTop     = cameraRollTop.x();

   casted.cameraYawBottom   = cameraYawBottom.x();
   casted.cameraPitchBottom = cameraPitchBottom.x();
   casted.cameraRollBottom  = cameraRollBottom.x();

   casted.bodyPitch         = bodyPitch.x();

   return casted;
}

