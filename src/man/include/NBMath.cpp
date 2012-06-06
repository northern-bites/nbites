#include "NBMath.h"
using namespace NBMath;

const int NBMath::ROUND(float x) {
    if ((x-static_cast<float>(
             static_cast<int>(x))) >= 0.5) return (static_cast<int>(x)+1);
    if ((x-static_cast<float>(
             static_cast<int>(x))) <= -0.5) return (static_cast<int>(x)-1);
    else return (int)x;
}

const float NBMath::clip(const float value, const float minValue,
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

const float NBMath::clip(const float value, const float minMax){
    return clip(value,-minMax,minMax);
}

/**
 * Given a float return its sign
 *
 * @param f the number to examine the sign of
 * @return -1.0f if f is less than 0.0f, 1.0f if greater than, 0.0f otherwise
 */
const float NBMath::sign(const float f)
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
 * Given a float return its sign
 *
 * @param f the number to examine the sign of
 * @return -1.0f if f is less than 0.0f, 1.0f otherwise
 */
const float NBMath::signNoZero(const float f)
{
    if (f < 0.0f) {
        return -1.0f;
    } else {
        return 1.0f;
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
const float NBMath::subPIAngle(float theta)
{
    theta = std::fmod(theta, 2.0f*M_PI_FLOAT);
    if( theta > M_PI) {
        theta -= 2.0f*M_PI_FLOAT;
    }

    if( theta < -M_PI) {
        theta += 2.0f*M_PI_FLOAT;
    }
    return theta;
}

const float NBMath::safe_asin(const float input){
    return std::asin(clip(input,1.0f));
}
const float NBMath::safe_acos(const float input){
    return std::acos(clip(input,1.0f));
}

// takes in two sides of a triangle, returns hypotenuse
float getHypotenuse(float x, float y) {
    return std::sqrt(x*x + y*y);
}

const float  NBMath::cycloidx(const float theta){
    return theta - std::sin(theta);
}

const float  NBMath::cycloidy(const float theta){
    return 1.0f - std::cos(theta);
}

const float NBMath::safe_atan2(const float y, const float x)
{
    if (x == 0.0f) {
        if ( y > 0.0f) {
            return M_PI_FLOAT / 2.0f;
        } else {
            return -M_PI_FLOAT / 2.0f;
        }
    }
    return static_cast<float>(atan2(y,x));
}
