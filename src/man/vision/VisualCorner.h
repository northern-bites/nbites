#ifndef VisualCorner_hpp_defined
#define VisualCorner_hpp_defined

#include <cstdlib>
#include <iomanip>
#include <vector>
#include "Structs.h"

class VisualCorner;

#include "ConcreteCorner.h"
#include "Utility.h"
#include "VisualDetection.h"
#include "VisualLandmark.h"
#include "VisualLine.h"
#include <boost/shared_ptr.hpp>


class VisualCorner : public VisualDetection, public VisualLandmark<cornerID> {
private: // Constants
    // Number of pixels that must extend beyond the intersection for a line to
    // be considered a T
    static const float MIN_EXTEND_DIST;
    // We consider ourselves to be towards the bottom of the screen and in the
    // middle in terms of whether we are inside or outside of a corner
    static const point <int> naoLocation;

public:
    VisualCorner(const int _x, const int _y, const float _distance,
                 const float _bearing,
                 boost::shared_ptr<VisualLine> l1,
                 boost::shared_ptr<VisualLine> l2, const float _t1,
                 const float _t2);
    // destructor
    virtual ~VisualCorner();
    // copy constructor
    VisualCorner(const VisualCorner&);

    friend std::ostream& operator<< (std::ostream &o, const VisualCorner &c)
    {
        return o << std::setprecision(2)
        << "(" << c.getX() << "," << c.getY() << ") \tDistance: "
        << c.getDistance() << "\tBearing: " << c.getBearing()
        << "\tShape: " << ConcreteCorner::getShapeString(
                c.getShape());
    }

    void identifyLinesInCorner();
    std::vector<boost::shared_ptr<VisualLine> > getLines() {
        return lines;
    }

    void identifyFromLines();

    ////////////////////////////////////////////////////////////
    // GETTERS
    ////////////////////////////////////////////////////////////
    const std::list <const ConcreteCorner *> getPossibleCorners() const {
        return possibleCorners; }
    boost::shared_ptr<VisualLine> getLine1() const { return line1; }
    boost::shared_ptr<VisualLine> getLine2() const { return line2; }

    boost::shared_ptr<VisualLine> getTBar() const { return tBar; }
    boost::shared_ptr<VisualLine> getTStem() const { return tStem; }

    // See FieldLines.cc intersectLines to see how this is calculated and used
    const float getT1() const { return t1; }
    const float getT2() const { return t2; }
    const shape getShape() const { return cornerType; }
    const shape getSecondaryShape() const { return secondaryShape; }

    // DO NOT USE THIS UNLESS getShape() returns inner or outer L; I have
    // not yet hooked up the angle thing for T corners
    const float getAngleBetweenLines() const { return angleBetweenLines; }

    const float getOrientation() const { return orientation; }

    const point<int> getTStemEndpoint() const;
    const bool doesItPointDown();
    const bool doesItPointUp();
    const bool doesItPointRight();
    const bool doesItPointLeft();

    virtual const bool hasPositiveID();

    const std::vector<cornerID> getIDs();

    ////////////////////////////////////////////////////////////
    // SETTERS
    ////////////////////////////////////////////////////////////
    void setPossibleCorners(std::list <const ConcreteCorner *>
                            _possibleCorners);
    void setPossibleCorners(std::vector <const ConcreteCorner *>
                            _possibleCorners);
    void setShape(const shape s) { cornerType = s; }
    void setSecondaryShape(const shape s);
    void setLine1(boost::shared_ptr<VisualLine> l1) { line1 = l1; }
    void setLine2(boost::shared_ptr<VisualLine> l2) { line2 = l2; }
    void setDistanceWithSD(float _distance);
    void setBearingWithSD(float _bearing);
    void setID(cornerID _id) { id = _id; }
    void setTOrientation();


private: // private methods
    const shape getLClassification();

    void IDFromLine(const boost::shared_ptr<VisualLine> line);

    void determineCornerIDFromShape();
    void determineCornerShape(); // called on object instantiation


    inline static float cornerDistanceToSD(float _distance) {
        return std::sqrt(2.0f * std::max(10 + _distance*0.00125f, 250.0f));
    }
    inline static float cornerBearingToSD(float _bearing) {
        return std::sqrt(static_cast<float>(M_PI) / 4.0f);
    }


private:
    // This list will hold all the possibilities for this corner's specific ID
    // It will get set from within FieldLines.cc.
    std::list <const ConcreteCorner *> possibleCorners;
    shape cornerType;
    shape secondaryShape;

    boost::shared_ptr<VisualLine> line1;
    boost::shared_ptr<VisualLine> line2;
    std::vector<boost::shared_ptr<VisualLine> > lines;

    // These indicate what distance the corner is from the startpoints of the
    // respective line (1 and 2).
    float t1, t2;

    // Will not mean much unless the corner is actually a T
    boost::shared_ptr<VisualLine> tBar;
    boost::shared_ptr<VisualLine> tStem;

    // The angle between the two lines whose intersection creates this corner.
    // In the case of a T corner, we report the smaller angle (the second
    // angle can be found by subtracting this angle from 180)
    float angleBetweenLines;
    // the orientation of the corner
    float orientation;
    bool up;       // does the bisector point up?
    bool right;    // does the bisector point right?
};

// functor that checks if the shape of one corner equals the given shape
class cornerShapeEquals : public std::unary_function<VisualCorner, bool> {
    shape s;
public:
    explicit cornerShapeEquals(const shape& shape) : s(shape) { }
    bool operator() (const VisualCorner& c) const { return c.getShape() == s; }
};

class LCornerNearEdgeOfScreen : public std::unary_function<VisualCorner,bool>
{
    rectangle edges;
    int minPixelSeparation;
public:
    explicit LCornerNearEdgeOfScreen(rectangle _edges, int _pixels) :
    edges(_edges), minPixelSeparation(_pixels) {}
    bool operator() (const VisualCorner& c) const {
        int x = c.getX();
        int y = c.getY();

        // Must be an L..
        return (c.getShape() == INNER_L ||
                c.getShape() == OUTER_L) &&
                // Edges must match
                (abs(edges.left - x) < minPixelSeparation ||
                        abs(edges.right - x) < minPixelSeparation ||
                        abs(edges.top - y) < minPixelSeparation ||
                        abs(edges.bottom - y) < minPixelSeparation);
    }
};

class TCornerNearEdgeOfScreen : public std::unary_function<VisualCorner,bool>
{
    rectangle edges;
    int minPixelSeparation;
public:
    explicit TCornerNearEdgeOfScreen(rectangle _edges, int _pixels) :
    edges(_edges), minPixelSeparation(_pixels) {}
    bool operator() (const VisualCorner& c) const {
        int x = c.getX();
        int y = c.getY();

        // Must be an L..
        return (c.getShape() == T &&
                // Edges must match
                (abs(edges.left - x) < minPixelSeparation ||
                        abs(edges.right - x) < minPixelSeparation ||
                        abs(edges.top - y) < minPixelSeparation ||
                        abs(edges.bottom - y) < minPixelSeparation));
    }
};


#endif
