#ifndef __LEAST_SQUARES_LINE_HPP
#define __LEAST_SQUARES_LINE_HPP

#include "types/Point.hpp"

class LeastSquaresLine
{
public:
        LeastSquaresLine ();
        virtual ~LeastSquaresLine ();

        /**
         * Add a point to the regressed line and recalculate the parameters
         *
         * @param x             x co-ord of point to add to line
         * @param y             y co-ord of point to add to line
         */
        void addPoint (int x, int y);

        /**
         * Add a point to the regressed line and recalculate the parameters
         *
         * @param p             Point to be added to the line
         */
        void addPoint (const Point &p);

        /**
         * Remove a point from the regressed line and recalculate the parameters
         * This does not actually require the point was added before hand
         *
         * @param x             x co-ord of point to remove to line
         * @param y             y co-ord of point to remove to line
         */
        void removePoint (int x, int y);

        /**
         * Remove a point from the regressed line and recalculate the parameters
         * This does not actually require the point was added before hand
         *
         * @param p             Point to be removeed to the line
         */
        void removePoint (const Point &p);

        /**
         * Given a y co-ord, predict x according to the regressed line
         *
         * @param y             y co-ordanate of point to predict
         * @return              Predicted x co-ord according to the regressed
         *                      line
         *                      If slope = 0, then X cannot be predicted, and
         *                      INT_MAX is returned. It is up to the caller to
         *                      realize the return is nonsensical
         */
        int predictX (int y) const;

        /**
         * Given a x co-ord, predict y according to the regressed line
         *
         * @param x             x co-ordanate of point to predict
         * @return              Predicted y co-ord according to the regressed
         *                      line
         *                      If slope = inf, then y cannot be predicted, and
         *                      INT_MAX is returned. It is up to the caller to
         *                      realize the return is nonsensical
         */
        int predictY (int x) const;

        /**
         * Is absolute value of the slope greater than 1
         *
         * @return               True if abs (slope) > 1
         *                       False if abs (slope) <= 1
         */
        bool isAbsSlopeGT1 () const;

        /**
         * Is the slope negative
         *
         * @return               True if slope < 0
         *                       False if slope >= 0
         */
        bool isSlopeNegative () const;

        /**
         * Is Point within range of the regressed line. The distance used is
         * The algebraic distance, not the pythagorean distance. However, if
         * abs (slope) > 1 then the horizontal (x) algebraic distance is used,
         * otherwise the vertical (y) algebraic distance is used. This ensures
         * that the difference between algebraic and pythagorean distance is
         * as minimal as possible.
         *
         * @param x             x co-ord of point to test
         * @param y             y co-ord of point to test
         * @return              True if algebraic distance to point <= range
         *                      False otherwise
         */
        bool isPointWithin (int x, int y, int range) const;

        /**
         * Is Point within range of the regressed line. The distance used is
         * The algebraic distance, not the pythagorean distance. However, if
         * abs (slope) > 1 then the horizontal (x) algebraic distance is used,
         * otherwise the vertical (y) algebraic distance is used. This ensures
         * that the difference between algebraic and pythagorean distance is
         * as minimal as possible.
         *
         * @param p             Point to test
         * @return              True if algebraic distance to point <= range
         *                      False otherwise
         */
        bool isPointWithin (const Point &p, int range) const;

        /**
         * Returns the normalized dot product of l and itself. This is a good
         * measure of how parallel two lines are.
         *
         * The result is always positive, as the lines have no direction
         */
        float dotProduct (const LeastSquaresLine &l);

        /**
         * Merges two lines together
         */
        void merge (const LeastSquaresLine &l);
private:
        /**
         * ints will overflow
         */

        float _sumX2;
        float _sumX;

        float _sumY2;
        float _sumY;

        float _sumXY;

        float _sum1; /* aka how many points */

        /**
         * to maintain accuracy of very steep slopes, whenever abs (slope) > 1
         * y becomes the independant variable
         */

        bool _xIndependent;

        /**
         * y = _c1 * x + _c2
         */

        float _c1;
        float _c2;
};

#endif

