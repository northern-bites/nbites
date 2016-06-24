#pragma once

#include <Eigen/Eigen>

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive &ar, Eigen::Vector3f &vec, const unsigned int version) {
         ar & vec[0];
         ar & vec[1];
         ar & vec[2];
      }

      template<class Archive>
      void serialize(Archive &ar, Eigen::Matrix<float, 3, 3> &var, const unsigned int version) {
         ar & var(0, 0); ar & var(0, 1); ar & var(0, 2);
         ar & var(1, 0); ar & var(1, 1); ar & var(1, 2);
         ar & var(2, 0); ar & var(2, 1); ar & var(2, 2);
      }
      
      template<class Archive>
      void serialize(Archive &ar, Eigen::Matrix<float, 7, 1> &vec, const unsigned int version) {
         for (int i = 0; i < 7; i++) {
            ar & vec[i];
         }
      }

      template<class Archive>
      void serialize(Archive &ar, Eigen::Matrix<float, 7, 7> &var, const unsigned int version) {
         for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 7; col++) {
               ar & var(row, col);
            }
         }
      }
   }
}
