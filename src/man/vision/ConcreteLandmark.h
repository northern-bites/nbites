#ifndef ConcreteLandmark_h_defined__
#define ConcreteLandmark_h_defined__

#include <string>

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

#endif
