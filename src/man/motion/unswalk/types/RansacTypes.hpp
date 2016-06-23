#pragma once

#include <math.h>
#include <boost/serialization/version.hpp>
#include "types/Point.hpp"

struct RANSACLine
{
   Point p1, p2;
   /**
    * Line defined in terms of
    * t1 * x + t2 * y + t3 = 0
    */
   int t1, t2, t3;
   float var;

   RANSACLine(Point p1, Point p2, float var = 0) : p1(p1), p2(p2), var(var)
   {
      t1 = p2.y() - p1.y();
      t2 = p1.x() - p2.x();
      t3 = p1.y() * (p2.x() - p1.x()) - p1.x() * (p2.y() - p1.y());
   }

   RANSACLine() {};
   
   RANSACLine(const RANSACLine &other) {
      this->t1 = other.t1;
      this->t2 = other.t2;
      this->t3 = other.t3;
      this->var = other.var;
      
      this->p1(0, 0) = other.p1(0, 0);
      this->p1(1, 0) = other.p1(1, 0);
      this->p2(0, 0) = other.p2(0, 0);
      this->p2(1, 0) = other.p2(1, 0);
   }

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version)
   {
      ar & t1 & t2 & t3;
      ar & var;
      if (file_version >= 1) {
        ar & p1 & p2;
      } else {
        p1 = Point (0, 0);
        p2 = Point (0, 0);
      }
   }
};

inline std::ostream& operator<<(std::ostream& os, const RANSACLine& line) {
   os << line.p1;
   os << line.p2;
   os.write((char*) &(line.t1), sizeof(int));
   os.write((char*) &(line.t2), sizeof(int));
   os.write((char*) &(line.t3), sizeof(int));
   os.write((char*) &(line.var), sizeof(float));
   
   return os;
}

inline std::istream& operator>>(std::istream& is, RANSACLine& line) {
   is >> line.p1;
   is >> line.p2;
   is.read((char*) &(line.t1), sizeof(int));
   is.read((char*) &(line.t2), sizeof(int));
   is.read((char*) &(line.t3), sizeof(int));
   is.read((char*) &(line.var), sizeof(float));
   
   return is;
}

BOOST_CLASS_VERSION(RANSACLine, 1);

struct RANSACCircle
{
   PointF centre;
   float radius;
   float var;

   RANSACCircle(PointF centre, float radius, float var = 0)
      : centre(centre), radius(radius), var(var)
   {
   }

   RANSACCircle(const Point p1, const Point p2, const Point p3, float var = 0)
      : var(var)
   {
      float bx = p1.x(); float by = p1.y();
      float cx = p2.x(); float cy = p2.y();
      float dx = p3.x(); float dy = p3.y();

      float temp = cx*cx+cy*cy;
      float bc   = (bx*bx + by*by - temp)/2.0;
      float cd   = (temp - dx*dx - dy*dy)/2.0;
      float det  = (bx-cx)*(cy-dy)-(cx-dx)*(by-cy);

      centre = PointF();
      radius = 0;

      if (fabs(det) < 1.0e-6) {
         this->radius = std::numeric_limits<float>::quiet_NaN();
         return;
      }

      det = 1 / det;
      centre.x() = (bc*(cy-dy)-cd*(by-cy))*det;
      centre.y() = ((bx-cx)*cd-(cx-dx)*bc)*det;
      cx = centre.x(); cy = centre.y();
      radius = sqrt((cx-bx)*(cx-bx)+(cy-by)*(cy-by));
   }

   RANSACCircle(const Point p1, const Point p2, float radius, float var = 0)
      : radius(radius), var(var)
   {
		/* TODO(carl) Find a faster way to do this */
/*
      PointF v = (p2 - p1).cast<float>();
      PointF n = PointF(v.y(), -v.x()).normalized();
      PointF m = (p2 + p1).cast<float>() / 2;
      float dist2 = ((p1.x()- p2.x()) * (p1.x()- p2.x())) +
         ((p1.y()- p2.y()) * (p1.y()- p2.y()));
      float d = sqrt(radius*radius - (dist2 / 4));
      PointF newN = PointF(n.x() * d, n.y() *d);
      centre = (m + (newN));
*/

      centre = PointF();
      if (p1 == p2) {
         this->radius = std::numeric_limits<float>::quiet_NaN();
         return;
      };

      const float x1 = p1.x(), x2 = p2.x(), y1 = p1.y(), y2 = p2.y();
      if (y1 != y2) {
         const float k1 = (x2 - x1) / (y1 - y2);
         const float k2 = (x1*x1 + y1*y1 - x2*x2 - y2*y2) / (2*(y1 - y2));

         const float a = 1 + k1*k1;
         const float b = -2*x1 + 2*(k1*(k2 - y1));
         const float c = x1*x1 + (k2 - y1)*(k2 - y1) - radius*radius;
         const float d = b*b - 4*a*c;

         if (d < 0) {
            this->radius = std::numeric_limits<float>::quiet_NaN();
            return;
         }

         const float x = (-b + (y1 > y2 ? sqrtf(d) : -sqrtf(d))) / (2*a);
         const float y = k1*x + k2;
         centre.x() = x;
         centre.y() = y;

      } else {
         const float k1 = (y2 - y1) / (x1 - x2);
         const float k2 = (y1*y1 + x1*x1 - y2*y2 - x2*x2) / (2*(x1 - x2));

         const float a = 1 + k1*k1;
         const float b = -2*y1 + 2*(k1*(k2 - x1));
         const float c = y1*y1 + (k2 - x1)*(k2 - x1) - radius*radius;
         const float d = b*b - 4*a*c;

         if (d < 0) {
            this->radius = std::numeric_limits<float>::quiet_NaN();
            return;
         }

         const float x = (-b + (x1 > x2 ? sqrtf(d) : -sqrtf(d))) / (2*a);
         const float y = k1*x + k2;
         centre.x() = x;
         centre.y() = y;
      }
   }

   RANSACCircle() {};

   template<class Archive>
   void serialize(Archive &ar, const unsigned int file_version)
   {
      ar & centre;
      ar & radius;
      ar & var;
   }

};

BOOST_CLASS_VERSION(RANSACCircle, 0);
