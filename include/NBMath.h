/*
 * Northern Bites file with math expressions needed in many modules/threads
 */

#ifndef NBMath_h_DEFINED
#define NBMath_h_DEFINED
#include <cmath>

static const float M_TO_CM  = 100.0f;
static const float CM_TO_M  = 0.01f;
static const float CM_TO_MM = 10.0f;
static const float MM_TO_CM = 0.1f;

static const double PI = M_PI;
static const double DEG_OVER_RAD = 180.0 / M_PI;
static const double RAD_OVER_DEG = M_PI / 180.0;
static const float M_PI_FLOAT = static_cast<float>(M_PI);
static const float TO_DEG = 180.0f/M_PI_FLOAT;
#ifndef TO_RAD
static const float TO_RAD = M_PI_FLOAT/180.0f;
#endif
static const float QUART_CIRC_RAD = M_PI_FLOAT / 2.0f;

#ifdef __APPLE__
    inline static void sincosf(float _x, float * _sinx, float * _cosx) {
        *_sinx = std::sin(_x);
		*_cosx = std::cos(_x);
    }

	#define isnan(x)	\
		(	sizeof (x) == sizeof(float )	?	__inline_isnanf((float)(x))	\
		:	sizeof (x) == sizeof(double)	?	__inline_isnand((double)(x))	\
											:	__inline_isnan ((long double)(x)))

#endif

namespace NBMath {

    inline static int ROUND(float x) {
        if ((x-static_cast<float>(
                 static_cast<int>(x))) >= 0.5) return (static_cast<int>(x)+1);
        if ((x-static_cast<float>(
                 static_cast<int>(x))) <= -0.5) return (static_cast<int>(x)-1);
        else return (int)x;
    }

    static const float clip(const float value, const float minValue,
                            const float maxValue) {
        if (value > maxValue)
            return maxValue;
        else if (value < minValue)
            return minValue;
        else if(isnan(value))
            return 0.0f;
        else
            return value;
    }

    static const float clip(const float value, const float minMax){
        return clip(value,-minMax,minMax);
    }

    /**
     * Given a float return its sign
     *
     * @param f the number to examine the sign of
     * @return -1.0f if f is less than 0.0f, 1.0f otherwise
     */
    static float sign(const float f)
    {
        if (f < 0.0f) {
            return -1.0f;
        } else if (f > 0.0f) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    }

    /**
     * Returns an equivalent angle to the one passed in with value between positive
     * and negative pi.
     *
     * @param theta The angle to be simplified
     *
     * @return The equivalent angle between -pi and pi.
     */
    inline static float subPIAngle(float theta)
    {
        theta = fmod(theta, 2.0f*M_PI_FLOAT);
        if( theta > M_PI) {
            theta -= 2.0f*M_PI_FLOAT;
        }

        if( theta < -M_PI) {
            theta += 2.0f*M_PI_FLOAT;
        }
        return theta;
    }
}
#endif //NBMath_h
