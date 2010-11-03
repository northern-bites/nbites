#ifndef _HoughLine_h_DEFINED
#define _HoughLine_h_DEFINED

#include <ostream>


/**
 * A line defined in polar coordinates, also storing information from the
 * Hough Space in which it was found.
 */
class HoughLine
{
public:
   HoughLine(int _rIndex, int _tIndex,
              float _r, float _t, int _score);
    virtual ~HoughLine() { };

    float getRadius() const { return r;      }
    float getAngle()  const { return t;      }

    int getRIndex()   const { return rIndex; }
    int getTIndex()   const { return tIndex; }
    int getScore()    const { return score;  }

    static bool intersect(int x0, int y0,
                          const HoughLine& a, const HoughLine& b);
    friend std::ostream& operator<< (std::ostream &o,
                                     const HoughLine &l){
        return o << "Line: rIndex: " << l.rIndex << " tIndex: " << l.tIndex <<
            "\n\tr: " << l.r << " t:" << l.t << " score: " << l.score;
    }

private:
    float r, t;                 // Radius and angle of line in polar coords
    int rIndex,                 // Radius index in HoughSpace
        tIndex,                 // Theta index in HoughSpace
        score;                  // Hough accumulator count
};

#endif /* _HoughLine_h_DEFINED */
