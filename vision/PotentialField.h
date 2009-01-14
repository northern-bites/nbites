/* A potential field allows one to easily and flexibly specify how a dog
   should position itself on the field.  This is accomplished through that
   placement of charges, which either attract or repell the dog to or from the
   location at which they are placed.  One can specify both the strength of
   this attractive/repulsive force and the how the force should decay for
   points increasingly distant from the charge.  When there are multiple
   charges in the potential field, the net force over all of the charges is
   used to determine how a dog should move.  This implementation was inspired
   by potential field work done by the NuBots and particularly by 2006 the
   thesis of Florian Knorn. */

#ifndef POTENTIAL_FIELD
#define POTENTIAL_FIELD

#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>

#include "FieldConstants.h"

using namespace std;

// mathematical functions (as in f(x))
inline static const float dPoint(const float x_p, const float y_p,
                                 const float x, const float y) {
    return sqrt(pow((x - x_p),2) + pow((y - y_p),2));
}

inline static const float dPoint2(const float x_p, const float y_p,
                                  const float x, const float y) {
    return pow((x - x_p),2) + pow((y - y_p),2);
}

inline static const float pointHeight(const float attr, const float spr2,
                                      const float x_p, const float y_p,
                                      const float x, const float y) {
    return attr * pow((double)M_E, (double)-dPoint2(x_p, y_p, x, y) / spr2);
}

inline static const float point_xGrad(const float attr, const float spr2,
                                      const float x_p, const float y_p,
                                      const float x, const float y) {
    return (-2 * attr / spr2) * (x - x_p) *
        pow((double)M_E, (double)(-dPoint2(x_p, y_p, x, y) / spr2));
}

inline static const float point_yGrad(const float attr, const float spr2,
                                      const float x_p, const float y_p,
                                      const float x, const float y) {
    return (-2 * attr / spr2) * (y - y_p) *
        pow((double)M_E, (double)(-dPoint2(x_p, y_p, x, y) / spr2));
}

inline static const float dLine(const float a_l, const float b_l,
                                const float c_l, const float x, const float y) {
    return (a_l*x + b_l*y + c_l) / sqrt(a_l*a_l + b_l*b_l);
}

inline static const float dLine2(const float a_l, const float b_l,
                                 const float c_l,
                                 const float x, const float y) {
    return (a_l*x + b_l*y + c_l)*(a_l*x + b_l*y + c_l) / (a_l*a_l + b_l*b_l);
}

inline static const float lineHeight(const float attr, const float spr2,
                                     const float a_l, const float b_l,
                                     const float c_l, const float x,
                                     const float y) {
    return attr * pow((double)M_E, (double)(-dLine2(a_l, b_l, c_l, x, y) /
                                            spr2));
}

inline static const float line_xGrad(const float attr, const float spr2,
                                     const float a_l, const float b_l,
                                     const float c_l, const float x,
                                     const float y) {
    float tmp1 = a_l*a_l + b_l*b_l;
    float tmp2 = a_l*x + b_l*y + c_l;
    return ((-2.0f * attr / spr2) * (a_l / tmp1) * tmp2 *
            pow((double)M_E, (double)(-tmp2*tmp2 / tmp1 / spr2)));
}

inline static const float line_yGrad(const float attr, const float spr2,
                                     const float a_l, const float b_l,
                                     const float c_l,
                                     const float x, const float y) {
    float tmp1 = a_l*a_l + b_l*b_l;
    float tmp2 = a_l*x + b_l*y + c_l;
    return ((-2.0f * attr / spr2) * (b_l / tmp1) * tmp2 *
            pow((double)M_E,(double)(-tmp2*tmp2 / tmp1 / spr2)));
}

static const char POINT[] = "point";
static const char LINE[] = "line";
static const char SEGMENT[] = "segment";

/* Spread constants
   We define the constants in terms of cm, but then multiply them by a
   scaling factor S to avoid floating-point accuracy problems in the
   gradient functions */
static const float DISTANCE_NEUTRAL = FIELD_HEIGHT;
static const float WHOLE_FIELD = FIELD_HEIGHT;
static const float HALF_FIELD = WHOLE_FIELD / 2.0f;
static const float REGION = WHOLE_FIELD / 5.0f;
static const float SMALL_REGION = WHOLE_FIELD / 12.0f;
static const float VERY_SMALL_REGION = WHOLE_FIELD / 25.0f;
static const float UTTERLY_SMALL_REGION = WHOLE_FIELD / 40.0f;
static const float LINE_SPREAD = VERY_SMALL_REGION;
static const float POINT_SPREAD = 5.0f;

// Attraction constants
static const float ATTRACTION = -1.0f;
static const float REPULSION = 1.0f;
static const float VERY_STRONG = 16.0f;
static const float STRONG = 8.0f;
static const float MEDIUM_ = 4.0f;
static const float WEAK = 3.0f;
static const float VERY_WEAK = 2.0f;

// Standard field setup
static const float SIDELINE_REPULSION = VERY_STRONG * REPULSION;
static const float SIDELINE_SPREAD = VERY_SMALL_REGION;
static const float MY_GOALBOX_REPULSION = VERY_STRONG * REPULSION;
static const float MY_GOALBOX_POINT_SPREAD = WHOLE_FIELD / 25.0f;
static const float MY_GOALBOX_LINE_SPREAD = WHOLE_FIELD / 45.0f;

/* Tends to make the dogs that have gone slightly out of bounds come back
   towards the center of the field, instead of moving farther out of bounds.
   Distance in cm. */
static const float SIDELINE_OFFSET = 5.0f;

/* Distance in cm that is considered sufficiently close to true
   equilibriums in the potential field that the dog need not move */
static const float EQUILIBRIUM_RANGE = 4.0f;

struct Vector2D {
    float x;
    float y;
};

class Charge {
    /* Wraps the attributes that define a charge: its type, attraction, spread,
       and x and y locations on the field.
       Also responsible for computing heights and gradients with respect
       to specified positions on the field. */
protected:
    const char *type;
    bool is_standard, affects_goalbox, line; // it's either a line or a segment
    float attraction, spread, spread2, x1, y1, x2, y2, a_l, b_l, c_l;

    bool isPoint() const { return strcmp(type, POINT) == 0; }
    bool isLine() const { return strcmp(type, LINE) == 0; }
    bool isSegment() const { return strcmp(type, SEGMENT) == 0; }
    bool isStandard() const { return is_standard; }

    void recalcLine();
    const float callForPoint(const float at_x, const float at_y) const;
    const float callForLine(const float at_x, const float at_y) const;
    const bool actsAsPoint(const float at_x, const float at_y) const;
    Vector2D relevantPoint(const float at_x, const float at_y) const;

public:
    Charge(const char *type, float attraction, float spread,
           bool affects_goalbox, float x1, float y1);
    Charge(const char *type, float attraction, float spread,
           bool affects_goalbox, float x1, float y1, float x2, float y2);
    virtual ~Charge();

    void description(char *string);
    void setX1(const float x);
    void setY1(const float y);
    void setX2(const float x);
    void setY2(const float y);

    virtual const float heightAt(const float at_x, const float at_y) const;
    virtual const float xGradAt(const float at_x, const float at_y) const;
    virtual const float yGradAt(const float at_x, const float at_y) const;
};

class Point : public Charge {
public:
    Point(float attraction, float spread, bool affects_goalbox,
          float x1, float y1);
    virtual ~Point();

    virtual const float heightAt(const float x, const float y) const;
    virtual const float xGradAt(const float x, const float y) const;
    virtual const float yGradAt(const float x, const float y) const;
};

class Line : public Charge {
public:
    Line(float attraction, float spread, bool affects_goalbox,
	     float x1, float y1, float x2, float y2);
    virtual ~Line();

    virtual const float heightAt(const float x, const float y) const;
    virtual const float xGradAt(const float x, const float y) const;
    virtual const float yGradAt(const float x, const float y) const;
};

class Segment : public Charge {
public:
    Segment(float attraction, float spread, bool affects_goalbox,
            float x1, float y1, float x2, float y2);
    ~Segment();

    virtual const float heightAt(const float x, const float y) const;
    virtual const float xGradAt(const float x, const float y) const;
    virtual const float yGradAt(const float x, const float y) const;
};

class PotentialField {
private:
    vector <Charge*> charges;

public:
    PotentialField();
    virtual ~PotentialField();

    const unsigned int addPointCharge(const float attraction,
                                      const float spread,
                                      const float x, const float y,
                                      const bool affects_goalbox = true);
    const unsigned int addLineCharge(const float attraction, const float spread,
                                     const float x1, const float y1,
                                     const float x2, const float y2,
                                     const bool affects_goalbox = true);
    const unsigned int addSegmentCharge(const float attraction,
                                        const float spread,
                                        const float x1, const float y1,
                                        const float x2, const float y2,
                                        const bool affects_goalbox = true);
    void movePointCharge(const unsigned int point_charge_id,
                         const float x, const float y);
    void moveLineCharge(const unsigned int point_charge_id,
                        const float new_x1, const float new_y1,
                        const float new_x2, const float new_y2);
    void moveSegmentCharge(const unsigned int point_charge_id,
                           const float new_x1, const float new_y1,
                           const float new_x2, const float new_y2);
    void addFieldCharges();
    void removeCharge(const unsigned int charge_id);
    void movementVectorAt(Vector2D *vector, bool *x_eq, bool *y_eq,
                          const float at_x, const float at_y) const;
    // chargesDescription
    // nonStandardChargesDescription
    // descriptionForCharges

    const float heightAt(const float at_x, const float at_y) const;

protected:
    const float sumHeights(const float at_x, const float at_y) const;
    const float sumXGrads(const float at_x, const float at_y) const;
    const float sumYGrads(const float at_x, const float at_y) const;
    const unsigned int addChargeGetId(Charge *charge);
};

#endif
