// An abstract class that stores coordinates of a landmark

#ifndef ConcreteLandmark_h_defined__
#define ConcreteLandmark_h_defined__

#include <string>

namespace man {
namespace vision {

class ConcreteLandmark {
public:
    ConcreteLandmark(const float& _fieldX, const float& _fieldY);
    ConcreteLandmark(const ConcreteLandmark& other);
    virtual ~ConcreteLandmark();

    virtual const std::string toString() const = 0;

    float getFieldX()     const { return fieldX;     }
    float getFieldY()     const { return fieldY;     }

private:
    //  point <const float> fieldLocation;
    const float fieldX, fieldY;
};

}
}

#endif
