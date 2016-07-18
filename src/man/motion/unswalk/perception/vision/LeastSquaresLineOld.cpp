#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "LeastSquaresLine.hpp"

LeastSquaresLine::LeastSquaresLine ()
{
        _sumX2 = 0;
        _sumY2 = 0;
        _sumXY = 0;
        _sumX  = 0;
        _sumY  = 0;
        _sum1  = 0;

        _xIndependent = true;
}

LeastSquaresLine::~LeastSquaresLine ()
{
}

void LeastSquaresLine::addPoint (const Point& p)
{
        addPoint (p.x (), p.y ());
}

void LeastSquaresLine::addPoint (int x, int y)
{
        /*printf ("%p: adding pt (%d, %d)\n", this, x, y);
        if (_xIndependent) {
                printf ("%p: y = %fx + %f -> ", this, _c1, _c2);
        } else {
                printf ("%p: x = %fy + %f -> ", this, _c1, _c2);
        }*/

        _sumX2 += x * x;
        _sumY2 += y * y;
        _sumXY += x * y;
        _sumX  += x;
        _sumY  += y;
        _sum1  += 1;

        /**
         * Standard least squares. We are solving the equation
         *
         * |sum (x^2)   sum (x)| |c1|   |sum (y * x)|
         * |sum (x)     sum (1)| |c2| = |sum (y)    |
         */

        int det;
        if (_xIndependent) {
                det = _sumX2 * _sum1 - _sumX * _sumX;
        } else {
                det = _sumY2 * _sum1 - _sumY * _sumY;
        }
        
        /**
         * If the determinant is 0 all points lie on either a vertical
         * or horizontal line. To fix this we toggle the _xIndependent
         * flag and try again
         */

        if (det == 0) {
                _xIndependent = !_xIndependent;
                if (_xIndependent) {
                        det = _sumX2 * _sum1 - _sumX * _sumX;
                } else {
                        det = _sumY2 * _sum1 - _sumY * _sumY;
                }
        }

        /**
         * If the determinant is still 0 all points lie on both a vertical
         * and horizontal line, ie they are all on a single point.
         * In this case our model will be line through the point with
         * slope = 1
         */

        if (det == 0) {
                _xIndependent = true;
                _c1 = 1.0f;
                _c2 = (_sumY - (_sumX * _c1)) / _sum1;
                return;
        }

        /**
         * Otherwise we just solve the equation above
         */

        //printf ("y^2 * _sumX = %d x^2 * _sumY = %d INT_MAX = %d\n", _sumY2 * _sumX
        //                        , _sumX2 * _sumY, INT_MAX);
        if (_xIndependent) {
                _c1 = (_sum1 * _sumXY - _sumX * _sumY) / det;
                _c2 = (_sumX2 * _sumY - _sumX * _sumXY) / det;
        } else {
                _c1 = (_sum1 * _sumXY - _sumY * _sumX) / det;
                _c2 = (_sumY2 * _sumX - _sumY * _sumXY) / det;
        }

        /**
         * if the slope > 1 we have to flip the x and y
         */

        if (fabsf (_c1) > 1.0f) {
                _c2 = - (_c2 / _c1);
                _c1 = (1.0f / _c1);
                _xIndependent = !_xIndependent;
        }

        /*if (_xIndependent) {
                printf ("y = %fx + %f\n", _c1, _c2);
        } else {
                printf ("x = %fy + %f\n", _c1, _c2);
        }*/
}

void LeastSquaresLine::removePoint (const Point& p)
{
        removePoint (p.x (), p.y ());
}

void LeastSquaresLine::removePoint (int x, int y)
{
        _sumX2 -= x * x;
        _sumY2 -= y * y;
        _sumXY -= x * y;
        _sumX  -= x;
        _sumY  -= y;
        _sum1  -= 1;

        /**
         * Standard least squares. We are solving the equation
         *
         * |sum (x^2)   sum (x)| |c1|   |sum (y * x)|
         * |sum (x)     sum (1)| |c2| = |sum (y)    |
         */

        int det;
        if (_xIndependent) {
                det = _sumX2 * _sum1 - _sumX * _sumX;
        } else {
                det = _sumY2 * _sum1 - _sumY * _sumY;
        }
        
        /**
         * If the determinant is 0 all points lie on either a vertical
         * or horizontal line. To fix this we toggle the _xIndependent
         * flag and try again
         */

        if (det == 0) {
                _xIndependent = !_xIndependent;
                if (_xIndependent) {
                        det = _sumX2 * _sum1 - _sumX * _sumX;
                } else {
                        det = _sumY2 * _sum1 - _sumY * _sumY;
                }
        }

        /**
         * If the determinant is still 0 all points lie on both a vertical
         * and horizontal line, ie they are all on a single point.
         * In this case our model will be line through the point with
         * slope = 1
         */

        if (det == 0) {
                _xIndependent = true;
                _c1 = 1.0f;
                _c2 = (_sumY - (_sumX * _c1)) / _sum1;
                return;
        }

        /**
         * Otherwise we just solve the equation above
         */

        if (_xIndependent) {
                _c1 = (_sum1 * _sumXY - _sumX * _sumY) / det;
                _c2 = (_sumX2 * _sumY - _sumX * _sumXY) / det;
        } else {
                _c1 = (_sum1 * _sumXY - _sumY * _sumX) / det;
                _c2 = (_sumY2 * _sumX - _sumY * _sumXY) / det;
        }

        /**
         * if the slope > 1 we have to flip the x and y
         */

        if (fabsf (_c1) > 1.0f) {
                _c2 = - (_c2 / _c1);
                _c1 = (1.0f / _c1);
                _xIndependent = !_xIndependent;
        }
}

int LeastSquaresLine::predictX (int y) const
{
        if (_xIndependent) {
                /**
                 * If the slope == 0 then we cant predict an X, so we return
                 * INT_MAX and rely on the caller to realize that this is
                 * unreasonable
                 */
                if (_c1 == 0) {
                        return INT_MAX;
                } else {
                        return int ((float (y) - _c2) / _c1);
                }
        } else {
                return int (_c1 * y + _c2);
        }
}

int LeastSquaresLine::predictY (int x) const
{
        if (_xIndependent) {
                return int (_c1 * x + _c2);
        } else {
                /**
                 * If the slope == 0 then we cant predict an X, so we return
                 * INT_MAX and rely on the caller to realize that this is
                 * unreasonable
                 */
                if (_c1 == 0) {
                        return INT_MAX;
                } else {
                        return int ((float (x) - _c2) / _c1);
                }
        }
}

bool LeastSquaresLine::isAbsSlopeGT1 () const
{
        /**
         * note that if the slope is greater than 1, y becomes the independent
         */
        return ! _xIndependent;
}

bool LeastSquaresLine::isSlopeNegative () const
{
        return _c1 < 0; 
}

bool LeastSquaresLine::isPointWithin (int x, int y, int range) const
{
        /**
         * If abs (slope) < 1, then the vertical algebraic distance will be the
         * closest to the geometric (pythagorean) distance. Else wise the 
         * horizontal distance will be closest
         */

        if (_xIndependent) {
                return (abs (y - predictY (x)) <= range);
        } else {
                return (abs (x - predictX (y)) <= range);
        }
}

bool LeastSquaresLine::isPointWithin (const Point &p, int range) const
{
        return isPointWithin (p.x (), p.y (), range);
}

float LeastSquaresLine::dotProduct (const LeastSquaresLine &l)
{
        float i1, j1, i2, j2;

        if (_xIndependent) {
                i1 = sqrtf (1 / ((_c1 * _c1) + 1));
                j1 = sqrtf (1 - i1 * i1);
        } else {
                j1 = sqrtf (1 / ((_c1 * _c1) + 1));
                i1 = sqrtf (1 - j1 * j1);
        }

        if (l._xIndependent) {
                i2 = sqrtf (1 / ((l._c1 * l._c1) + 1));
                j2 = sqrtf (1 - i2 * i2);
        } else {
                j2 = sqrtf (1 / ((l._c1 * l._c1) + 1));
                i2 = sqrtf (1 - j2 * j2);
        }

        return i1 * i2 + j1 * j2;
}

void LeastSquaresLine::merge (const LeastSquaresLine &l)
{
        /*printf ("%p: adding pt (%d, %d)\n", this, x, y);
        if (_xIndependent) {
                printf ("%p: y = %fx + %f -> ", this, _c1, _c2);
        } else {
                printf ("%p: x = %fy + %f -> ", this, _c1, _c2);
        }*/

        _sumX2 += l._sumX2;
        _sumY2 += l._sumY2;
        _sumXY += l._sumXY;
        _sumX  += l._sumX;
        _sumY  += l._sumY;
        _sum1  += l._sum1;

        /**
         * Standard least squares. We are solving the equation
         *
         * |sum (x^2)   sum (x)| |c1|   |sum (y * x)|
         * |sum (x)     sum (1)| |c2| = |sum (y)    |
         */

        int det;
        if (_xIndependent) {
                det = _sumX2 * _sum1 - _sumX * _sumX;
        } else {
                det = _sumY2 * _sum1 - _sumY * _sumY;
        }
        
        /**
         * If the determinant is 0 all points lie on either a vertical
         * or horizontal line. To fix this we toggle the _xIndependent
         * flag and try again
         */

        if (det == 0) {
                _xIndependent = !_xIndependent;
                if (_xIndependent) {
                        det = _sumX2 * _sum1 - _sumX * _sumX;
                } else {
                        det = _sumY2 * _sum1 - _sumY * _sumY;
                }
        }

        /**
         * If the determinant is still 0 all points lie on both a vertical
         * and horizontal line, ie they are all on a single point.
         * In this case our model will be line through the point with
         * slope = 1
         */

        if (det == 0) {
                _xIndependent = true;
                _c1 = 1.0f;
                _c2 = (_sumY - (_sumX * _c1)) / _sum1;
                return;
        }

        /**
         * Otherwise we just solve the equation above
         */

        //printf ("y^2 * _sumX = %d x^2 * _sumY = %d INT_MAX = %d\n", _sumY2 * _sumX
        //                        , _sumX2 * _sumY, INT_MAX);
        if (_xIndependent) {
                _c1 = (_sum1 * _sumXY - _sumX * _sumY) / det;
                _c2 = (_sumX2 * _sumY - _sumX * _sumXY) / det;
        } else {
                _c1 = (_sum1 * _sumXY - _sumY * _sumX) / det;
                _c2 = (_sumY2 * _sumX - _sumY * _sumXY) / det;
        }

        /**
         * if the slope > 1 we have to flip the x and y
         */

        if (fabsf (_c1) > 1.0f) {
                _c2 = - (_c2 / _c1);
                _c1 = (1.0f / _c1);
                _xIndependent = !_xIndependent;
        }

        /*if (_xIndependent) {
                printf ("y = %fx + %f\n", _c1, _c2);
        } else {
                printf ("x = %fy + %f\n", _c1, _c2);
        }*/
}

