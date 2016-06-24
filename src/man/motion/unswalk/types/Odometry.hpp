#pragma once

#include <iostream>

struct Odometry {
   float forward;
   float left;
   float turn;

   Odometry(float f = 0.0f, float l = 0.0f, float t = 0.0f)
      : forward(f), left(l), turn(t) {}

   inline void clear() {
      forward = left = turn = 0.0f;
   }

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version)
   {
      ar & forward & left & turn;
   }
};

inline std::ostream& operator<<(std::ostream& os, const Odometry& odometry) {
   os.write((char*) &(odometry.forward), sizeof(float));
   os.write((char*) &(odometry.left), sizeof(float));
   os.write((char*) &(odometry.turn), sizeof(float));

   return os;
}

inline std::istream& operator>>(std::istream& is, Odometry& odometry) {
   is.read((char*) &(odometry.forward), sizeof(float));
   is.read((char*) &(odometry.left), sizeof(float));
   is.read((char*) &(odometry.turn), sizeof(float));

   return is;
}

#ifndef SWIG
   static inline Odometry operator+(const Odometry& a, const Odometry& b) {
      Odometry c;
      c.forward = a.forward + b.forward;
      c.left = a.left + b.left;
      c.turn = a.turn + b.turn;
      return c;
   }

   static inline Odometry operator-(const Odometry& a, const Odometry& b) {
      Odometry c;
      c.forward = a.forward - b.forward;
      c.left = a.left - b.left;
      c.turn = a.turn - b.turn;
      return c;
   }
#endif
