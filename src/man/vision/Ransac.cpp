#include "Ransac.h"
//#include "utils/basic_maths.hpp"

#include <limits>

bool RANSAC::findLine(const std::vector<PointI>  &points,
                 std::vector<bool>        **cons,
                 RANSACLine                &result,
                 unsigned int               k,
                 float                      e,
                 unsigned int               n,
                 std::vector<bool>          cons_buf[2],
                 unsigned int              *seed
                )
{
    return RANSAC::findLineConstrained(points, cons, result, k, e, n, cons_buf, seed, -1.f);
}


bool RANSAC::findLineConstrained(const std::vector<PointI>  &points,
                      std::vector<bool>        **cons,
                      RANSACLine                &result,
                      unsigned int               k,
                      float                      e,
                      unsigned int               n,
                      std::vector<bool>          cons_buf[2],
                      unsigned int              *seed,
                      float                      slopeConstraint
                     )
{
	std::cout << "In ransac" << std::endl;
   if (points.size() < n || n < 2) {
      return false;
   }

   std::cout << "In find lines constrained" << std::endl;

   /* error of best line found so far */
   float minerr = std::numeric_limits<float>::max();

   std::vector<bool> *best_concensus, *this_concensus;
   best_concensus = &cons_buf[0];

   unsigned int i, j;

   /**
    * Randomly select 2 points and create a line
    */
   for (i = 0; i < k; ++ i) {
      unsigned int p1, p2;
      p1 = rand_r(seed) % points.size();
      do {
         p2 = rand_r(seed) % points.size();
      } while (p1 == p2);

      RANSACLine l(points[p1], points[p2]);
      float slope = -((float)l.t1) / ((float)l.t2);
      if(slopeConstraint > 0.f && (slope < (1/slopeConstraint) || slope > slopeConstraint)) continue; 

      /**
       * figure out the variance (sum of distances of points from the line)
       * could use dist() here, but since the denominator is consistent, we
       * save time and implement it again here.
       */
      const float denom = sqrt(l.t1*l.t1 + l.t2*l.t2);
      const float newe  = e*denom;

      /**
       * Choose the currently unused concensus buffer
       */
      if (best_concensus == &cons_buf[0]) {
         this_concensus = &cons_buf[1];
      } else {
         this_concensus = &cons_buf[0];
      }

      unsigned int n_concensus_points = 0;
      for (j = 0; j != points.size(); ++ j) {
         const PointI &p = points[j];
         float dist = (l.t1 * p.x() + l.t2 * p.y() + l.t3);

         if (dist < 0) {
            dist = -dist;
         }

         if (dist < newe) {
            l.var += dist;
            ++ n_concensus_points;
            (*this_concensus)[j] = true;
         } else {
            (*this_concensus)[j] = false;
         }
      }
      l.var /= denom;

      const float k = 0.2;
      l.var = (k * l.var) - n_concensus_points;
      if (l.var < minerr && n_concensus_points >= n) {
         minerr = l.var;
         l.var  = l.var / (points.size() * e);
         result = l;
         //std::cout << result.t1 << " " << result.t2 << " " << result.t3 << " " << result.var << std::endl;
         best_concensus = this_concensus;
      }
   }

   if (minerr < std::numeric_limits<float>::max()) {
      *cons = best_concensus;
      return true;
   } else {
      return false;
   }
}


bool RANSAC::findCircleOfRadius(
      const std::vector<PointI>  &points,
      float                      radius,
      std::vector<bool>        **cons,
      RANSACCircle              &result,
      unsigned int               k,
      float                      e,
      unsigned int               n,
      std::vector<bool>          cons_buf[2],
      unsigned int              *seed)
{
   if (points.size() < n || n < 2) {
      return false;
   }

   const int e2 = e * e;

   /* error of best circle found so far */
   float minerr = std::numeric_limits<float>::max();

   std::vector<bool> *best_concensus, *this_concensus;
   best_concensus = &cons_buf[0];

   unsigned int i, j;

   /**
    * Randomly select 2 points and create a circle
    */
   for (i = 0; i < k; ++ i) {
      unsigned int p1, p2;
      p1 = rand_r(seed) % points.size();
      do {
         p2 = rand_r(seed) % points.size();
      } while (p1 == p2);

      RANSACCircle c(points[p1], points[p2], radius);
      PointI centre = c.centre.cast<int>();

      /**
       * Choose the currently unused concensus buffer
       */
      if (best_concensus == &cons_buf[0]) {
         this_concensus = &cons_buf[1];
      } else {
         this_concensus = &cons_buf[0];
      }

      unsigned int n_concensus_points = 0;
      for (j = 0; j != points.size(); ++ j) {
         const PointI &p = points[j];
         /* TODO(carl) look into integer version of this */
         float dist = sqrt((centre - p).squaredNorm()) - radius;
         int dist2 = dist * dist;

         if (dist2 < e2) {
            c.var += dist2;
            ++ n_concensus_points;
            (*this_concensus)[j] = true;
         } else {
            (*this_concensus)[j] = false;
         }
      }

      const float k = 0.2;
      c.var = (k * c.var) - n_concensus_points;
      if (c.var < minerr && n_concensus_points >= n) {
         minerr = c.var;
         c.var  = c.var / (points.size() * e);
         result = c;
         best_concensus = this_concensus;
      }
   }

   if (minerr < std::numeric_limits<float>::max()) {
      *cons = best_concensus;
      return true;
   } else {
      return false;
   }
}

bool RANSAC::findCircleOfRadius3P(
      const std::vector<PointI>  &points,
      float                      radius,
      float                      radius_e,
      std::vector<bool>        **cons,
      RANSACCircle              &result,
      unsigned int               k,
      float                      e,
      unsigned int               n,
      std::vector<bool>          cons_buf[2],
      unsigned int              *seed)
{
   if (points.size() < n || n < 3) {
      return false;
   }

   const int e2 = e * e;

   /* error of best circle found so far */
   float minerr = std::numeric_limits<float>::max();

   std::vector<bool> *best_concensus, *this_concensus;
   best_concensus = &cons_buf[0];

   unsigned int i, j;

   float pos_var[4], neg_var[4];

   RANSACCircle c;
   for (i = 0; i < k; ++ i) {
      /**
       * Randomly select 3 points and create a circle
       */
      unsigned int p1, p2, p3;
      int radius_fails = 0;
      while (radius_fails < 20) {
         p1 = rand_r(seed) % points.size();
         do {
            p2 = rand_r(seed) % points.size();
         } while (p1 == p2);
         do {
            p3 = rand_r(seed) % points.size();
         } while (p1 == p3 || p2 == p3);

         RANSACCircle tmp(points[p1], points[p2], points[p3]);

         if (! isnan(tmp.radius) && fabsf(tmp.radius - radius) <= radius_e) {
            c = tmp;
            break;
         }

         ++ radius_fails;

      }
      if (radius_fails == 20) {
         return false;
      }

      /**
       * Choose the currently unused concensus buffer
       */
      if (best_concensus == &cons_buf[0]) {
         this_concensus = &cons_buf[1];
      } else {
         this_concensus = &cons_buf[0];
      }

      for (j = 0; j < 4; ++ j) {
         pos_var[j] = neg_var[j] = 0;
      }

      unsigned int n_concensus_points = 0;
      for (j = 0; j != points.size(); ++ j) {
         const PointF &p = points[j].cast<float>();
         const PointF &d = c.centre - p;
         /* TODO(carl) look into integer version of this */
         float dist = d.norm() - c.radius;
         float dist2 = dist * dist;

         if (dist2 < e2) {
            int quadrant = 0;
            if (d.x() > 0) {
               if (d.y() > 0) {
                  quadrant = 0;
               } else {
                  quadrant = 3;
               }
            } else {
               if (d.y() > 0) {
                  quadrant = 1;
               } else {
                  quadrant = 2;
               }
            }

            if (dist > 0) {
               pos_var[quadrant] += dist2;
            } else {
               neg_var[quadrant] += dist2;
            }

            ++ n_concensus_points;
            (*this_concensus)[j] = true;
         } else {
            (*this_concensus)[j] = false;
         }
      }

      const float k = 0.2;

      c.var = 0;
      for (j = 0; j < 4; ++ j) {
         float diff_var = pos_var[j] - neg_var[j];
         c.var += pos_var[j] + neg_var[j] + diff_var * diff_var;
      }

      c.var = (k * c.var) - n_concensus_points;
      if (c.var < minerr && n_concensus_points >= n) {
         minerr = c.var;
         c.var  = c.var / (points.size() * e);
         result = c;
         best_concensus = this_concensus;
      }
   }

   if (minerr < std::numeric_limits<float>::max()) {
      *cons = best_concensus;
      return true;
   } else {
      return false;
   }
}

bool RANSAC::findLinesAndCircles(const std::vector<PointI>  &points,
                      float                      radius,
                      std::vector<bool>        **cons,
                      RANSACLine                 &resultLine,
                      RANSACCircle               &resultCircle,
                      unsigned int               k,
                      float                      e,
                      unsigned int               n,
                      std::vector<bool>          cons_buf[2],
                      unsigned int              *seed
                     )
{
   if (points.size() < n || n < 2) {
      return false;
   }

   /* error of best line found so far */
   float minerr = std::numeric_limits<float>::max();
   const int e2 = e * e;

   std::vector<bool> *best_concensus, *this_concensus;
   best_concensus = &cons_buf[0];

   unsigned int i, j;

   /**
    * Randomly select 2 points and create a line
    */
   for (i = 0; i < k; ++ i) {
      unsigned int p1, p2;
      p1 = rand_r(seed) % points.size();
      p2 = p1;
/*
      int count = 0;
      while ((p1 == p2) || 
            (DISTANCE_SQR(points[p1].x(), points[p1].y(),
                          points[p2].x(), points[p2].y()) < (100*100))) {
         p2 = rand_r(seed) % points.size();
         count++;
         if (count > 40) return false;
      }
*/

      while (p1 == p2) { 
         p2 = rand_r(seed) % points.size();
      }

      RANSACLine l(points[p1], points[p2]);

      /**
       * figure out the variance (sum of distances of points from the line)
       * could use dist() here, but since the denominator is consistent, we
       * save time and implement it again here.
       */
      const float denom = sqrt(l.t1*l.t1 + l.t2*l.t2);
      const float newe  = e*denom;

      /**
       * Choose the currently unused concensus buffer
       */
      if (best_concensus == &cons_buf[0]) {
         this_concensus = &cons_buf[1];
      } else {
         this_concensus = &cons_buf[0];
      }
      float distsum = 0;
      unsigned int n_concensus_points = 0;
      for (j = 0; j != points.size(); ++ j) {
         const PointI &p = points[j];
         float dist = (l.t1 * p.x() + l.t2 * p.y() + l.t3);

         if (dist < 0) {
            dist = -dist;
         }

         if (dist < newe) {
            l.var += (dist*dist);
            ++ n_concensus_points;
            (*this_concensus)[j] = true;
         } else {
            (*this_concensus)[j] = false;
         }
      }
      l.var /= (denom * denom);
      distsum = l.var;
      //const float k = 0.2;
      //l.var = (k * l.var) - n_concensus_points;
      //l.var /= (n_concensus_points/2);
      //l.var -= (n_concensus_points/points.size());
      l.var = n_concensus_points;
      l.var *= -1;
      //std::cout << "l.var1 = " << l.var << std::endl;
      l.var += distsum / 100;
      //std::cout << "l.var2 = " << l.var << std::endl;
      if (l.var < minerr && n_concensus_points >= n) {
         minerr = l.var;
         resultLine = l;
         //std::cout << "line var = " << l.var 
         //          << " and distsum = " << distsum << std::endl;
         //std::cout << result.t1 << " " << result.t2 << " " << result.t3 << " " << result.var << std::endl;
         best_concensus = this_concensus;
      }

//      int count = 0;
//      while ((p1 == p2) || 
//            (DISTANCE_SQR(points[p1].x(), points[p1].y(),
//                          points[p2].x(), points[p2].y()) < (200*200))) {
//         p2 = rand_r(seed) % points.size();
//         count++;
//         if (count > 40) return false;
//      }

      // Also try and find a circle through those two points
      RANSACCircle c(points[p1], points[p2], radius);
      if (isnan(c.radius)) {
         continue;
      }
      PointI centre = c.centre.cast<int>();

      //
      // Choose the currently unused concensus buffer
      //
      if (best_concensus == &cons_buf[0]) {
         this_concensus = &cons_buf[1];
      } else {
         this_concensus = &cons_buf[0];
      }
   
      n_concensus_points = 0;
/*
      float point_dist = sqrt((points[p1] - points[p2]).squaredNorm());
      std::cout << "p1 = " << p1 << " p2 = " << p2 << std::endl;
      std::cout << "p1 = ( " << points[p1].x() << ", " << points[p1].y()
                << ")" << std::endl;
      std::cout << "p2 = ( " << points[p2].x() << ", " << points[p2].y()
                << ")" << std::endl;
      std::cout << "points dist = " << point_dist << std::endl;
      std::cout << "centre = ( " << centre.x() << ", " << centre.y()
                << ")" << std::endl;
*/
      for (j = 0; j != points.size(); ++ j) {
         const PointI &p = points[j];
         // TODO(carl) look into integer version of this
         float dist = sqrt((centre - p).squaredNorm()) - radius;
         float dist2 = dist*dist;
//         std::cout << "j = " << j << " and dist = " << dist << std::endl;

         if (dist2 < e2) {
            c.var += dist2;
            ++ n_concensus_points;
            (*this_concensus)[j] = true;
         } else {
            (*this_concensus)[j] = false;
         }
      }
      distsum = c.var;
      //c.var = (k * c.var) - n_concensus_points;
      //c.var /= (n_concensus_points * n_concensus_points);

      if (n_concensus_points == 0) {
         //std::cout << "no points in ransac circle :(" << std::endl;
         continue;
      }

      //c.var /= (n_concensus_points/2);
      //c.var -= (n_concensus_points/points.size());
      c.var = n_concensus_points;
      c.var *= -1;
      c.var += 8 + distsum / 100;
      if (c.var < minerr && n_concensus_points >= n) {
         minerr = c.var;
         resultCircle = c;
         best_concensus = this_concensus;
         //std::cout << "circle var = " << c.var 
         //          << " and distsum = " << distsum << std::endl;
      }

   }

   if (minerr < std::numeric_limits<float>::max()) {
      *cons = best_concensus;
      return true;
   } else {
      return false;
   }
}
/**
 * RANSACLine Acceptor
 */
RANSAC::Acceptor<RANSACLine>::Acceptor(RANSACLine &line, float error)
   : line (line),
     denom(line.t1 * line.t1 + line.t2 * line.t2),
     e2   ((error * error) * denom)
{
}

bool RANSAC::Acceptor<RANSACLine>::accept(PointI p)
{
   float dist = (line.t1 * p.x() + line.t2 * p.y() + line.t3);
   float d2   = dist * dist;
   if (d2 < e2) {
      line.var += d2;
      return true;
   } else {
      return false;
   }
}

void RANSAC::Acceptor<RANSACLine>::finalise()
{
   /* Normalise the line variance between [0, 1] * n_consensus_point */
   line.var = line.var / e2;
}

/**
 * RANSACLine Generator
 */

bool RANSAC::Generator<RANSACLine>::operator() (
            RANSACLine &line,
            const std::vector<PointI> &points,
            unsigned int *seed) const
{

   if (points.size() < 2) {
      return false;
   }

   unsigned int p1, p2;
   p1 = rand_r(seed) % points.size();
   do {
      p2 = rand_r(seed) % points.size();
   } while (p1 == p2);
   line = RANSACLine(points[p1], points[p2]);

   return true;
}
