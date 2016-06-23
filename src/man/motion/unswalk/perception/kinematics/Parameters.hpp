#pragma once
#include "FADBAD++/fadiff.h"
template <class T>
class Parameters {
   public:
      T cameraPitchTop;
      T cameraYawTop;
      T cameraRollTop;

      T cameraYawBottom;
      T cameraPitchBottom;
      T cameraRollBottom;

      T bodyPitch;

      /* Used only to convert fadbad::F<float> to float.
       * Needed by KinematicsCalibrationSkill
       */
      template<typename T_>
      Parameters<T_> cast();

      template<class Archive>
      void serialize(Archive &ar, const unsigned int file_version) {
         ar & cameraPitchTop;
         ar & cameraYawTop;
         ar & cameraRollTop;
         ar & cameraYawBottom;
         ar & cameraPitchBottom;
         ar & cameraRollBottom;
         ar & bodyPitch;
      }

};

