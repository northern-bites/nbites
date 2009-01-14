#include "PotentialField.h"

// Geometric helper methods

inline bool inOwnGoalbox(const float x, const float y) {
    return x > MY_GOALBOX_LEFT_X &&
        x < MY_GOALBOX_RIGHT_X &&
        y > MY_GOALBOX_BOTTOM_Y &&
        y < MY_GOALBOX_TOP_Y;
}

/* Returns the a_l, b_l, and c_l in a float array such that line between
   the given points is equal to a_l*x + b_l*y + c_l = 0 */
void lineAbcForPoints(float result[], float x1, float y1, float x2, float y2)
{
    // See section 2.6.2 in Knorn's thesis
    float a_l, b_l, c_l;

    float x_s = x2 - x1;
    float y_s = y2 - y1;
    if (x_s == 0) {
        a_l = 1;
        b_l = 0;
    }
    else if (y_s == 0) {
        a_l = 0;
        b_l = 1;
    }
    else {
        a_l = -y_s / x_s;
        b_l = 1;
    }
    c_l = -a_l*x1 - b_l*y1;
    result[0] = a_l;
    result[1] = b_l;
    result[2] = c_l;
}

Vector2D normalize(const Vector2D p) {
    // Normalizes a 2-dimensional vector
    float norm = sqrt(p.x*p.x + p.y*p.y);
    if (norm == 0) {
        Vector2D result;
        result.x = result.y = 0;
        return result;
    }
    else {
        Vector2D result;
        result.x = p.x / norm;
        result.y = p.y / norm;
        return result;
    }
}

float dotProduct(const Vector2D a, const Vector2D b) {
    // Returns the dot product of two 2-dimensional vectors, represented as
    // points
    return a.x*b.x + a.y*b.y;
}

enum Possibility {
    P1,
    P2,
    A_LINE
};

Possibility determineRegionForSegment(const float x, const float y,
                                      const float x1, const float y1,
                                      const float x2, const float y2) {
    /* Determines whether the height and gradient created at a given point
       (x,y) should be calculated on the basis of the charge at point (x1, y1)
       => returns "p1", the charge at point (x2, y2) => returns "p2", or the
       line segment joining them => returns "line".  See figure 2.7 in Knorn's
       thesis. */
    float p1_a = x1 - x;
    float p1_b = y1 - y;
    float p2_a = x - x2;
    float p2_b = y - y2;
    float s_a = x2 - x1;
    float s_b = y2 - y1;
    float s_dot_p2 = (s_a*p2_a + s_b*p2_b);
    float s_dot_p1 = (s_a*p1_a + s_b*p1_b);
    bool a_or_b = s_dot_p2 < 0;
    bool b_or_c = s_dot_p1 < 0;

    if ((!b_or_c && a_or_b) || (s_dot_p1 == 0))
        return P1;

    else if ((!a_or_b && b_or_c) || (s_dot_p2 == 0))
        return P2;

    else return A_LINE;
}

Charge::Charge(const char *_type, float _attraction, float _spread,
               bool _affects_goalbox, float _x1, float _y1) {
    /* Create a charge with the given attraction and spread at the specified
       location on the field.
       init(Charge.POINT, x1, y1) => A point charge at (x,y)
       init(Charge.LINE, x1, y1, x2, y2) => A line charge extending through
       (x1, y1) and (x2, y2)
       init(Charge.SEGMENT, x1, y1, x2, y2, True) => A segment charge between
       (x1, y1) and (x2, y2) */
    type = _type;
    is_standard = false;
    affects_goalbox = _affects_goalbox;

    // point, line, or segment
    attraction = _attraction;
    spread = _spread;
    spread2 = spread*spread;
    line = false;
    x1 = _x1;
    y1 = _y1;
}

Charge::Charge(const char *_type, float _attraction, float _spread,
               bool _affects_goalbox, float _x1, float _y1,
               float _x2, float _y2)
{
    type = _type;
    is_standard = false;
    affects_goalbox = _affects_goalbox;

    // point, line, or segment
    attraction = _attraction;
    spread = _spread;
    spread2 = spread*spread;

    line = true;
    x1 = _x1;
    y1 = _y1;
    x2 = _x2;
    y2 = _y2;

    float result[3];
    lineAbcForPoints(result, x1, y1, x2, y2);
    a_l = result[0];
    b_l = result[1];
    c_l = result[2];
}

Charge::~Charge()
{
}

void Charge::setX1(const float x) {
    if (x1 != x) {
        x1 = x;
        recalcLine();
    }
}

void Charge::setY1(const float y) {
    if (y1 != y) {
        y1 = y;
        recalcLine();
    }
}

void Charge::setX2(const float x) {
    if (x2 != x) {
        x2 = x;
        recalcLine();
    }
}

void Charge::setY2(const float y) {
    if (y2 != y) {
        y2 = y;
        recalcLine();
    }
}

void Charge::recalcLine() {
    /* Recalculates the line coefficeints, i.e. after the line
       coordinates have been changed. */
    if (isLine() || isSegment()) {
        float result[3];
        lineAbcForPoints(result, x1, y1, x2, y2);
    }
}

const float Charge::heightAt(const float at_x, const float at_y) const {
    if (!affects_goalbox && inOwnGoalbox(at_x, at_y))
        return 0.0f;
    if (actsAsPoint(at_x, at_y)) {
        Vector2D p = relevantPoint(at_x, at_y);
        return point_yGrad(attraction, spread2, p.x, p.y, at_x, at_y);
    }
    else {
        return line_yGrad(attraction, spread2, a_l, b_l, c_l, at_x, at_y);
    }
}

const float Charge::xGradAt(const float at_x, const float at_y) const {
    if (!affects_goalbox && inOwnGoalbox(at_x, at_y))
        return 0.0f;
    if (actsAsPoint(at_x, at_y)) {
        Vector2D p = relevantPoint(at_x, at_y);
        return point_xGrad(attraction, spread2, p.x, p.y, at_x, at_y);
    }
    else {
        return line_xGrad(attraction, spread2, a_l, b_l, c_l, at_x, at_y);
    }
}

const float Charge::yGradAt(const float at_x, const float at_y) const {
    if (!affects_goalbox && inOwnGoalbox(at_x, at_y))
        return 0.0f;
    if (actsAsPoint(at_x, at_y)) {
        Vector2D p = relevantPoint(at_x, at_y);
        return point_yGrad(attraction, spread2, p.x, p.y, at_x, at_y);
    }
    else {
        return line_yGrad(attraction, spread2, a_l, b_l, c_l, at_x, at_y);
    }
}

const bool Charge::actsAsPoint(const float at_x, const float at_y) const {
    /* A charge as a point charge with respect to a
       given location if the
       charge is a point type or if the charge is a segment type and the
       location is not witin the perpendicular region of the segment. */
    if (isPoint()) return true;
    else if (isLine()) return false;
    else return determineRegionForSegment(at_x, at_y, x1, y1, x2, y2) != A_LINE;
}

Vector2D Charge::relevantPoint(const float at_x, const float at_y) const {
    /* For a point charge, the relevant point is
       the location of the charge.
       For a line segment charge for which the point (at_x, at_y) lies outside
       of the line segment, the relevant point is the one who's corresponding
       (at_x, at_y) is in.  See figure 2.7 in the thesis. */
    if (isPoint() ||
        determineRegionForSegment(at_x, at_y, x1, y1, x2, y2) == P1) {
        Vector2D p;
        p.x = x1;
        p.y = y1;
        return p;
    }
    else {
        Vector2D p;
        p.x = x2;
        p.y = y2;
        return p;
    }
}

void Charge::description(char *string)
{
    /* Returns a string describing this charge, for use e.g. in logging and
       debugging.
       Point: (P, a, s, x1, y1)
       Line: (L, a, s, x1, y1, x2, y2)
       Segment: (S, a, s, x1, y1, x2, y2)
       Where P, L, and S are string literals representing points, lines, and
       segments; a and s are the attraction and spread, respectively; and
       xn and yn are coordinates. */

    //if (isPoint()) {
    //  sprintf(string, "%s
}

Point::Point(float _attraction, float _spread,
             bool _affects_goalbox, float _x1, float _y1) :
    Charge(POINT, _attraction, _spread, _affects_goalbox, _x1, _y1) {

}

Point::~Point()
{
}

const float Point::heightAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    return pointHeight(attraction, spread2, x1, y1, x, y);
}

const float Point::xGradAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    return point_xGrad(attraction, spread2, x1, y1, x, y);
}

const float Point::yGradAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    return point_yGrad(attraction, spread2, x1, y1, x, y);
}

Line::Line(float _attraction, float _spread,
           bool _affects_goalbox, float _x1, float _y1,
           float _x2, float _y2) :
    Charge(LINE, _attraction, _spread, _affects_goalbox, _x1, _y1, _x2, _y2) {

}

Line::~Line()
{
}

const float Line::heightAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    return lineHeight(attraction, spread2, a_l, b_l, c_l, x, y);
}

const float Line::xGradAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    return line_xGrad(attraction, spread2, a_l, b_l, c_l, x, y);
}

const float Line::yGradAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    return line_yGrad(attraction, spread2, a_l, b_l, c_l, x, y);
}

Segment::Segment(float _attraction, float _spread,
                 bool _affects_goalbox, float _x1, float _y1,
                 float _x2, float _y2) :
    Charge(SEGMENT, _attraction, _spread, _affects_goalbox, _x1, _y1, _x2, _y2) {

}

Segment::~Segment()
{
}

const float Segment::heightAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    Possibility region = determineRegionForSegment(x, y, x1, y1, x2, y2);

    if (region == A_LINE) {
        return lineHeight(attraction, spread2, a_l, b_l, c_l, x, y);
    }
    else if (region == P1) {
        return pointHeight(attraction, spread2, x1, y1, x, y);
    }
    else {
        return pointHeight(attraction, spread2, x2, y2, x, y);
    }
}

const float Segment::xGradAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    Possibility region = determineRegionForSegment(x, y, x1, y1, x2, y2);

    if (region == A_LINE) {
        return line_xGrad(attraction, spread2, a_l, b_l, c_l, x, y);
    }
    else if (region == P1) {
        return point_xGrad(attraction, spread2, x1, y1, x, y);
    }
    else {
        return point_xGrad(attraction, spread2, x2, y2, x, y);
    }
}

const float Segment::yGradAt(const float x, const float y) const {
    if (!affects_goalbox && inOwnGoalbox(x, y))
        return 0.0f;
    Possibility region = determineRegionForSegment(x, y, x1, y1, x2, y2);

    if (region == A_LINE) {
        return line_yGrad(attraction, spread2, a_l, b_l, c_l, x, y);
    }
    else if (region == P1) {
        return point_yGrad(attraction, spread2, x1, y1, x, y);
    }
    else {
        return point_yGrad(attraction, spread2, x2, y2, x, y);
    }
}

PotentialField::PotentialField()
{
}

PotentialField::~PotentialField()
{
    while (charges.size() > 0) {
        if (charges[charges.size()-1]) {
            delete charges[charges.size()-1];
            charges.pop_back();
        }
    }
}

const unsigned int PotentialField::
addPointCharge(const float attraction, const float spread,
               const float x, const float y,
               const bool affects_goalbox) {
    /* Add to the potential field a single point charge with the specified
       attraction, spread, and x and y playing field coordinates.
       As with the other add*Charge methods, returns an interer charge id,
       which you will need to pass as a paramater to removeCharge() if and
       when you wish to remove this charge from the potential field.
       If no_effect_in_goalbox is True, the charge will not contribute to the
       potential hieght or gradient of objects in your own goalbox. */
    return addChargeGetId(new Point(attraction, spread, affects_goalbox, x, y));
}

const unsigned int PotentialField::
addLineCharge(const float attraction, const float spread,
              const float x1, const float y1,
              const float x2, const float y2,
              const bool affects_goalbox) {
    /* Add to the potential field a single line charge with the specified
       attraction and spread and which passes through the two given points,
       but extends infitely. */
    return addChargeGetId(new Line(attraction, spread, affects_goalbox,
                                   x1, y1, x2, y2));
}

const unsigned int PotentialField::
addSegmentCharge(const float attraction, const float spread,
                 const float x1, const float y1,
                 const float x2, const float y2,
                 const bool affects_goalbox) {
    /* Add to the potential field a single line charge with the specified
       attraction and spread and which lies between and terminates at the
       two given points. */
    return addChargeGetId(new Segment(attraction, spread, affects_goalbox,
                                      x1, y1, x2, y2));
}

void PotentialField::movePointCharge(const unsigned int point_charge_id,
                                     const float x, const float y) {
    /* Move a pre-existing point charge to a new location, keeping its
       previous attraction and spread. */
    Charge *p_ch = charges[point_charge_id];
    p_ch->setX1(x);
    p_ch->setY1(y);
}

void PotentialField::moveLineCharge(const unsigned int point_charge_id,
                                    const float new_x1, const float new_y1,
                                    const float new_x2, const float new_y2) {
    /* Move a pre-existing line charge to a new location, keeping its
       previous attraction and spread. */
    Charge *l_ch = charges[point_charge_id];
    l_ch->setX1(new_x1);
    l_ch->setY1(new_y1);
    l_ch->setX2(new_x2);
    l_ch->setY2(new_y2);
}

void PotentialField::moveSegmentCharge(const unsigned int point_charge_id,
                                       const float new_x1, const float new_y1,
                                       const float new_x2, const float new_y2) {
    /* Move a pre-existing segment charge to a new location, keeping its
       previous attraction and spread. */
    Charge *l_ch = charges[point_charge_id];
    l_ch->setX1(new_x1);
    l_ch->setY1(new_y1);
    l_ch->setX2(new_x2);
    l_ch->setY2(new_y2);
}

void PotentialField::addFieldCharges() {
    /* Add to the potential field a set of pre-defined charges that correspond
       to the basic boundries of the playing field.  The charges are designed
       to keep the dog in bounds and prevent it from entering its own goal box
       The method returns a tuple of charge ids, which you will need to pass
       as a paramater to removeCharge() if and when you wish to remove this
       set of charges from the potential field. */
    // unsigned int left_sideline_charge =
    //     addLineCharge(SIDELINE_REPULSION, SIDELINE_SPREAD,
    //                   FIELD_GREEN_LEFT_SIDELINE_X - SIDELINE_OFFSET,
    //                   FIELD_GREEN_BOTTOM_SIDELINE_Y,
    //                   FIELD_GREEN_LEFT_SIDELINE_X - SIDELINE_OFFSET,
    //                   FIELD_GREEN_TOP_SIDELINE_Y);

    // unsigned int right_sideline_charge =
    //     addLineCharge(SIDELINE_REPULSION, SIDELINE_SPREAD,
    //                   FIELD_GREEN_RIGHT_SIDELINE_X + SIDELINE_OFFSET,
    //                   FIELD_GREEN_BOTTOM_SIDELINE_Y,
    //                   FIELD_GREEN_RIGHT_SIDELINE_X + SIDELINE_OFFSET,
    //                   FIELD_GREEN_TOP_SIDELINE_Y);

    // unsigned int opponent_goalline_charge =
    //     addLineCharge(SIDELINE_REPULSION, SIDELINE_SPREAD,
    //                   FIELD_GREEN_LEFT_SIDELINE_X,
    //                   FIELD_GREEN_TOP_SIDELINE_Y + SIDELINE_OFFSET,
    //                   FIELD_GREEN_RIGHT_SIDELINE_X,
    //                   FIELD_GREEN_TOP_SIDELINE_Y + SIDELINE_OFFSET);

    // unsigned int my_goalline_charge =
    //     addLineCharge(SIDELINE_REPULSION, SIDELINE_SPREAD,
    //                   FIELD_GREEN_LEFT_SIDELINE_X,
    //                   FIELD_GREEN_BOTTOM_SIDELINE_Y - SIDELINE_OFFSET,
    //                   FIELD_GREEN_RIGHT_SIDELINE_X,
    //                   FIELD_GREEN_BOTTOM_SIDELINE_Y - SIDELINE_OFFSET);

    // float g_off = 15.0f;

    // unsigned int my_goalbox_bottom_charge =
    //     addSegmentCharge(MY_GOALBOX_REPULSION, MY_GOALBOX_LINE_SPREAD,
    //                      MY_GOALBOX_LEFT_X,
    //                      MY_GOALBOX_BOTTOM_Y - g_off,
    //                      MY_GOALBOX_RIGHT_X,
    //                      MY_GOALBOX_BOTTOM_Y - g_off);

    // unsigned int my_goalbox_left_charge =
    //     addSegmentCharge(MY_GOALBOX_REPULSION, MY_GOALBOX_LINE_SPREAD,
    //                      MY_GOALBOX_LEFT_X + g_off,
    //                      MY_GOALBOX_BOTTOM_Y + g_off,
    //                      MY_GOALBOX_LEFT_X + g_off,
    //                      MY_GOALBOX_TOP_Y - g_off,
    //                      false);

    // unsigned int my_goalbox_right_charge =
    //     addSegmentCharge(MY_GOALBOX_REPULSION, MY_GOALBOX_LINE_SPREAD,
    //                      MY_GOALBOX_RIGHT_X - g_off,
    //                      MY_GOALBOX_BOTTOM_Y + g_off,
    //                      MY_GOALBOX_RIGHT_X - g_off,
    //                      MY_GOALBOX_TOP_Y - g_off,
    //                      false);

    // unsigned int my_goalbox_top_charge =
    //     addSegmentCharge(MY_GOALBOX_REPULSION, MY_GOALBOX_LINE_SPREAD,
    //                      MY_GOALBOX_LEFT_X + g_off,
    //                      MY_GOALBOX_TOP_Y - g_off,
    //                      MY_GOALBOX_RIGHT_X - g_off,
    //                      MY_GOALBOX_TOP_Y - g_off,
    //                      false);

    /*unsigned int my_goalbox_middle_right_charge =
      addPointCharge(MY_GOALBOX_REPULSION, MY_GOALBOX_POINT_SPREAD,
      MIDFIELD_X +
      GOALBOX_WIDTH / 4.0f,
      FIELD_WHITE_BOTTOM_SIDELINE_Y);*/

    // unsigned int my_goalbox_point_charge =
	//     addPointCharge(MY_GOALBOX_REPULSION, MY_GOALBOX_POINT_SPREAD,
    //                    MIDFIELD_X, FIELD_WHITE_BOTTOM_SIDELINE_Y);

    // Need to build tuple full of all of these and send it off to python
    /*
      field_charge_ids = (left_sideline_charge,
      right_sideline_charge,
      opponent_goalline_charge,
      my_goalline_charge,
      my_goalbox_bottom_charge,
      my_goalbox_left_charge,
      my_goalbox_right_charge,
      my_goalbox_top_charge,
      my_goalbox_point_charge)
      #my_goalbox_middle_right_charge)
      #my_goalbox_middle_left_charge,
      #my_goalbox_middle_right_charge)
      # Note that all of these charges are part of the 'standard'
      # field setup
      for c_id in field_charge_ids:
      self.charges[c_id].is_standard = True
      return field_charge_ids
    */
}

void PotentialField::removeCharge(unsigned int charge_id) {
    /* Remove the charge corresponding to the given charge id from the
       potential field. */
    Charge *charge = charges[charge_id];
    delete charge;
    charges[charge_id] = 0;
}

void PotentialField::
movementVectorAt(Vector2D *vector, bool *x_eq, bool *y_eq,
                 const float at_x, const float at_y) const {
    /* Returns a 4-tuple:
     * The first 2 values are floats representing a normalized vector of the
     * relative x and y movements that the robot should make according to the
     * charges in the potential field.
     * The second 2 values are booleans indicating whether the dog has
     * reached a locaiton of approximate equillibrium in the x and y directions.
    */
    // Calculate the x and y gradients of the height function around
    // the given coordinates
    float x_slope = sumXGrads(at_x, at_y);
    float y_slope = sumYGrads(at_x, at_y);
    // Return the negative of the normalized vector given by the x and y
    // gradients: we want to the dog to move 'down' from high points on the
    // potiential field
    Vector2D not_normalized;
    not_normalized.x = -x_slope; not_normalized.y = -y_slope;
    *vector = normalize(not_normalized);

    // Test for equilibrium
    float current_height = heightAt(at_x, at_y);
    float height_at_x_minus_range = heightAt(at_x - EQUILIBRIUM_RANGE, at_y);
    float height_at_x_plus_range = heightAt(at_x + EQUILIBRIUM_RANGE, at_y);
    float height_at_y_minus_range = heightAt(at_x, at_y - EQUILIBRIUM_RANGE);
    float height_at_y_plus_range = heightAt(at_x, at_y + EQUILIBRIUM_RANGE);

    // Assume we are not at equilibrium unless we find otherwise
    *x_eq = false;
    *y_eq = false;
    // Test in the x direction
    if (current_height <= height_at_x_minus_range &&
        current_height <= height_at_x_plus_range)
        *x_eq = true;
    // Test in the y direction
    if (current_height <= height_at_y_minus_range &&
        current_height <= height_at_y_plus_range)
        *y_eq = true;
}

const float PotentialField::heightAt(const float at_x, const float at_y) const {
    /* Returns the height of the potential field at the given
       location, considering all point and line charges in the field. */
    return sumHeights(at_x, at_y);
}

const float PotentialField::sumHeights(const float at_x, const float at_y) const
{
    float height = 0.0f;
    for (unsigned int i = 0; i < charges.size() ; i++) {
        if (charges[i])
            height += charges[i]->heightAt(at_x, at_y);
    }
    return height;
}

const float PotentialField::sumXGrads(const float at_x, const float at_y) const
{
    /* Returns the x-gradient of the potential field at the given
       location, considering all point and line charges in the field. */
    float x_grad = 0.0f;
    for (unsigned int i = 0; i < charges.size() ; i++) {
        if (charges[i])
            x_grad += charges[i]->xGradAt(at_x, at_y);
    }
    return x_grad;
}

const float PotentialField::sumYGrads(const float at_x, const float at_y) const
{
    /* Returns the y-gradient of the potential field at the given
       location, considering all point and line charges in the field. */
    float y_grad = 0.0f;
    for (unsigned int i = 0; i < charges.size() ; i++) {
        if (charges[i])
            y_grad += charges[i]->yGradAt(at_x, at_y);
    }
    return y_grad;
}

const unsigned int PotentialField::addChargeGetId(Charge *charge)
{
    /* Generates an integer id to identify individual charges */
    // If it is bigger than all of them, it must be unique
    unsigned int new_id;
    if (charges.size() == 0)    new_id = 0;
    else new_id = charges.size();
    charges.push_back(charge);
    return new_id;
}
