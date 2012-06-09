#ifndef _HoughLine_h_DEFINED
#define _HoughLine_h_DEFINED

#include <iostream>
#include <math.h>
#include "FieldLines/Gradient.h"

/**
 * A line defined in polar coordinates, also storing information from the
 * Hough Space in which it was found.
 */
class HoughLine
{
public:
    HoughLine(int _rIndex, int _tIndex, int _score);

    HoughLine();
    virtual ~HoughLine() { };

    inline float getRadius() const { return r;      }
    inline float getAngle()  const { return t;      }

    inline int getRIndex()   const { return rIndex; }
    inline int getTIndex()   const { return tIndex; }
    inline int getScore()    const { return score;  }

    inline float getSinT() const {
        if (!didSin){
            didSin = true;
            sinT = sinf(t);
        }
        return sinT;
    }

    inline float getCosT() const {
        if (!didCos){
            didCos = true;
            cosT = cosf(t);
        }
        return cosT;
    }

    inline bool isOnLine(const AnglePeak& a) const {
        if (abs(a.angle - tIndex) < acceptable_angle_diff){

            float rTemp = r;
            float cs = getCosT();
            float sn = getSinT();
            if (rTemp < 0){
                rTemp = -rTemp;
                cs = -cs;
                sn = -sn;
            }

            // Find dot product of (cosT, sinT) and (pt.x, pt.y)
            float dot = (cs * static_cast<float>(a.x) +
                         sn * static_cast<float>(a.y));

            // Difference from r is its magnitude from the line
            float diff = std::abs(dot - rTemp);
            return diff < acceptable_xy_diff;
        }

        return false;
    }

    static bool intersect(int x0, int y0,
                          const HoughLine& a, const HoughLine& b);
    friend std::ostream& operator<< (std::ostream &o,
                                     const HoughLine &l) {
        return o << "Line: rIndex: " << l.rIndex << " tIndex: " << l.tIndex <<
            "\n\tr: " << l.r << " t:" << l.t << " score: " << l.score;
    }

    static void findLineImageIntersects(const HoughLine& line,
                                        double& u1, double& u2);

    bool operator==(const HoughLine &other) const;
    bool operator!=(const HoughLine &other) const;

private:
    int rIndex, tIndex;    // Radius, angle indices in HoughSpace table
    float r, t;            // Radius and angle of line in polar coords
    int score;             // Hough accumulator count

    mutable float sinT, cosT;   // These get computed on the fly, if needed
    mutable bool didSin, didCos;

    enum {
        acceptable_angle_diff = 5,
        acceptable_xy_diff = 5,
    };
};

#endif /* _HoughLine_h_DEFINED */
