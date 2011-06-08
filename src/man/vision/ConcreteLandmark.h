#ifndef ConcreteLandmark_h_defined__
#define ConcreteLandmark_h_defined__

#include <string>

class ConcreteLandmark {
public:
    ConcreteLandmark(const float& _fieldX, const float& _fieldY);
    ConcreteLandmark(const float& _fieldX,
                     const float& _fieldY,
                     const float& _fieldAngle);
    ConcreteLandmark(const ConcreteLandmark& other);
    virtual ~ConcreteLandmark();

    virtual const std::string toString() const = 0;

    const float getFieldX()     const { return fieldX;     }
    const float getFieldY()     const { return fieldY;     }
    const float getFieldAngle() const { return fieldAngle; }

private:
    //  point <const float> fieldLocation;
    const float fieldX, fieldY, fieldAngle;
};

#endif
