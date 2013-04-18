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

#define isnan(x)                                                        \
    (   sizeof (x) == sizeof(float )    ?   __inline_isnanf((float)(x)) \
        :   sizeof (x) == sizeof(double)    ?   __inline_isnand((double)(x)) \
        :   __inline_isnan ((long double)(x)))
#define isinf(x)                                                        \
    (   sizeof (x) == sizeof(float )    ?   __inline_isinff((float)(x)) \
        :   sizeof (x) == sizeof(double)    ?   __inline_isinfd((double)(x)) \
        :   __inline_isinf ((long double)(x)))

#endif

#ifdef WIN32
#define M_PI 3.14159265358979323846
inline static void sincosf(float _x, float * _sinx, float * _cosx) {
    *_sinx = std::sin(_x);
    *_cosx = std::cos(_x);
}

#define isnan(x) ((x)!=(x))

#define isinf(x)                                                        \
    (   sizeof (x) == sizeof(float )    ?   __inline_isinff((float)(x)) \
        :   sizeof (x) == sizeof(double)    ?   __inline_isinfd((double)(x)) \
        :   __inline_isinf ((long double)(x)))

#endif


namespace NBMath {

    const int ROUND(float x);

    const float clip(const float value, const float minValue,
                     const float maxValue);
    const float clip(const float value, const float minMax);

    /**
     * Given a float return its sign
     *
     * @param f the number to examine the sign of
     * @return -1.0f if f is less than 0.0f, 1.0f if positive, 0.0f otherwise
     */
    const float sign(const float f);
    /**
     * Given a float return its sign
     *
     * @param f the number to examine the sign of
     * @return -1.0f if f is less than 0.0f, 1.0f otherwise
     */
    const float signNoZero(const float f);

    /**
     * Returns an equivalent angle to the one passed in with value between
     * positive and negative pi.
     *
     * @param theta The angle to be simplified
     *
     * @return The equivalent angle between -pi and pi.
     */
    const  float subPIAngle(float theta);

    const float safe_asin(const float input);
    const float safe_acos(const float input);
    const float safe_atan2(const float y,
                           const float x);


    const float  cycloidx(const float theta);

    const float  cycloidy(const float theta);

    float getHypotenuse(float x, float y);

    float getLargestMagRoot(float a, float b, float c);

    template <class T>
    T square(T x) { return x*x; }

}
#endif //NBMath_h
