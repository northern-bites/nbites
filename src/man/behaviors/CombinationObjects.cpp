/**
 * Objects for python usage that contain pointers to both vision and loc.
 * See the header for in-depth descriptions.
 */
#include "CombinationObjects.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace man {
namespace behaviors {

//////////// Base Location Class Methods //////////////
Location::Location(float _x, float _y)
    : x(_x), y(_y)
{
}

Location::Location(const Location& other)
    :x(other.x), y(other.y)
{
}

bool Location::operator == (const Location& other) const
{
    return (x == other.x && y == other.y);
}

bool Location::operator != (const Location& other) const
{
    return (x != other.x || y != other.y);
}

RelLocation Location::operator - (const Location& other) const
{
    return RelLocation(x - other.x, y - other.y);
}

boost::python::tuple Location::toTupleXY()
{
    return boost::python::make_tuple(x, y);
}

RelLocation Location::getRelLocationOf(const Location& other) const {
    return other - *this;
}

const float Location::distTo(const Location& other)
{
    // HACK for infinity values, shouldn't happen
    if (isinf(other.x) || isinf(other.y)) {
        std::cout << "INFINITY DISTANCE" << std::endl;
        return INFINITE_DISTANCE;
    }

    return hypotf((other.y-y), (other.x-x));
}

const degrees Location::headingToInDeg(const Location& other)
{
    return (NBMath::safe_atan2((other.y-y), (other.x-x)))*TO_DEG;
}

const radians Location::headingToInRad(const Location& other)
{
    return (NBMath::safe_atan2((other.y-y), (other.x-x)));
}

//////// In methods check if loc is in a certain field region
const bool Location::inOppGoalBox()
{
    return ((OPP_GOALBOX_LEFT_X - BOX_BUFFER < x) &&
            (OPP_GOALBOX_RIGHT_X + BOX_BUFFER > x) &&
            (OPP_GOALBOX_TOP_Y + BOX_BUFFER) > y &&
            y > (OPP_GOALBOX_BOTTOM_Y - BOX_BUFFER));
}

const bool Location::inMyGoalBox()
{
    return ( x < (MY_GOALBOX_RIGHT_X + BOX_BUFFER) &&
             (MY_GOALBOX_TOP_Y + BOX_BUFFER) > y &&
             y > (OPP_GOALBOX_BOTTOM_Y - BOX_BUFFER));
}

const bool Location::inCenterOfField()
{
    return (TOP_LIMIT > y && y > BOTTOM_LIMIT);
}

const bool Location::inTopOfField()
{
    return (TOP_LIMIT < y);
}

const bool Location::inBottomOfField()
{
    return (BOTTOM_LIMIT > y);
}

bool Location::hasattr(boost::python::object obj,
                       std::string const &attrName) {
    return PyObject_HasAttrString(obj.ptr(), attrName.c_str());
}

boost::python::str Location::toString()
{
    std::string info = "x = " + boost::lexical_cast<std::string>(x) +
        ", y = " + boost::lexical_cast<std::string>(y);
    return *(new boost::python::str(info));
}

////////// RobotLocation Methods //////////////

RobotLocation::RobotLocation(float _x, float _y, degrees _h)
    : Location(_x, _y), h(_h*TO_RAD)
{
}

RobotLocation::RobotLocation(const RobotLocation& other)
    : Location(other.x, other.y), h(other.h)
{
}

bool RobotLocation::operator == (const RobotLocation& other) const
{
    return (x == other.x && y == other.y && h == other.h);
}

bool RobotLocation::operator != (const RobotLocation& other) const
{
    return (x != other.x || y != other.y || h != other.h);
}

RelRobotLocation RobotLocation::operator - (const RobotLocation& other) const
{
    return RelRobotLocation(x - other.x, y - other.y, (h - other.h)*TO_DEG);
}

RobotLocation RobotLocation::operator+ (const RelRobotLocation& other) const
{
    return RobotLocation(x + other.getRelX(), y + other.getRelY(), h + other.getRelH());
}

    RelRobotLocation RobotLocation::getRelRobotLocationOf(const RobotLocation& other) const {
        RelRobotLocation relRobotLocation;

        float dx = other.x - x;
        float dy = other.y - y;

        float sinh, cosh;
        sincosf(-h, &sinh, &cosh);

        relRobotLocation.setRelX(cosh * dx - sinh * dy);
        relRobotLocation.setRelY(sinh * dx + cosh * dy);
        relRobotLocation.setRelH((other.h - h)*TO_DEG);

        return relRobotLocation;
    }

const degrees RobotLocation::getRelativeBearing(Location& other)
{
    return (headingToInRad(other) - h) * TO_DEG;
}

// Returns -1, 0, 1 to tell the robot which way to spin
const float RobotLocation::spinDirToPoint(Location& other)
{
    float targetH = getRelativeBearing(other);
    if(targetH == 0) return 0;
    else return NBMath::sign(targetH);
}

boost::python::str RobotLocation::toString()
{
    std::string info = "x = " + boost::lexical_cast<std::string>(x) +
        ", y = " + boost::lexical_cast<std::string>(y) + ", h = " +
        boost::lexical_cast<std::string>(h*TO_DEG) + " (in degrees)";
    return *(new boost::python::str(info));
}

//////////// RelLocation Methods ///////////////

RelLocation::RelLocation(float dx, float dy)
    : relX(dx), relY(dy)
{
}

RelLocation::RelLocation(const RelLocation& other)
    :relX(other.getRelX()), relY(other.getRelY())
{
}

boost::python::str RelLocation::toString()
{
    std::string info = "relx = " + boost::lexical_cast<std::string>(relX)
        + ", rely = " + boost::lexical_cast<std::string>(relY);
    return *(new boost::python::str(info));
}

//////////// RelRobotLocation Methods ///////////////

RelRobotLocation::RelRobotLocation(float dx, float dy, degrees dh)
    : RelLocation(dx, dy), relH(dh*TO_RAD)
{
}

RelRobotLocation::RelRobotLocation(const RelRobotLocation& other)
    : RelLocation(other.getRelX(), other.getRelY()), relH(other.getRelH()*TO_RAD)
{
}

RelRobotLocation RelRobotLocation::operator- (boost::python::tuple delta) const {
    return RelRobotLocation(getRelX() - boost::python::extract<float>(delta[0]),
                            getRelY() - boost::python::extract<float>(delta[1]),
                            getRelH() - boost::python::extract<float>(delta[2]));
}

RelRobotLocation RelRobotLocation::operator- (const RelRobotLocation& other) const
{
    return RelRobotLocation(relX - other.relX, relY - other.relY,
                            relH-other.relH);
}

void RelRobotLocation::rotate(degrees theta) {
    radians theta_r = theta*TO_RAD;

    float sint, cost;
    sincosf(theta_r, &sint, &cost);
    float new_x = relX * cost - relY * sint;
    float new_y = relX * sint + relY * cost;

    relX = new_x;
    relY = new_y;
    relH += theta_r;
}

bool RelRobotLocation::within(boost::python::tuple region) const {

    //check heading
    float h = boost::python::extract<float>(region[2])*TO_RAD;

    if (std::fabs(relH) > h)
        return false;

    //check xy-region through an ellipse check

    float x = boost::python::extract<float>(region[0]);
    float y = boost::python::extract<float>(region[1]);
    // (relX/x)^2 + (relY/y)^2 < 1
    // http://www.wolframalpha.com/input/?i=x%5E2%2Fa%5E2+%2B+y%5E2%2Fb%5E2+%3C%3D+1
    return (relX*relX)/(x*x) + (relY*relY)/(y*y) <= 1;
}

boost::python::str RelRobotLocation::toString()
{
    std::string info = "relx = " + boost::lexical_cast<std::string>(relX)
        + ", rely = " + boost::lexical_cast<std::string>(relY) +
        ", relh = " + boost::lexical_cast<std::string>(getRelH()) +
        " (in degrees)";
    return *(new boost::python::str(info));
}

////////// FieldObject Methods //////////////

// FieldObject::FieldObject(VisualFieldObject& vfo,
//                          vis_landmark id, PyLoc& _pl)
//     : vis(&vfo), loc(new LocObject(_pl)),
//       visID(id), localID(0), bearing(0), dist(0)
// {
// }

// Determine whether vis or loc values are better
// void FieldObject::setBest()
// {
//     // Use vis values if the object is seen
//     if (vis->isOn()) {
//         bearing = vis->getBearing();
//         dist = vis->getDistance();
//     }
//     // Or if it was seen recently
//     else if (vis->getFramesOff() < LOST_OBJECT_FRAMES_THRESH) return;

//     else {
//         bearing = loc->getBearing()*TO_RAD;
//         dist = loc->getDist();
//     }
// }

// Links this object to a relative landmark, gives it an x, y, and name
// void FieldObject::associateWithRelativeLandmark(
//     boost::python::tuple relLandmark)
// {
//     loc->setX(boost::python::extract<float>(relLandmark[0])());
//     loc->setY(boost::python::extract<float>(relLandmark[1])());
//     localID = boost::python::extract<int>(relLandmark[2])();
// }

// bool FieldObject::hasattr(boost::python::object obj,
//                           std::string const &attrName) {
//     return PyObject_HasAttrString(obj.ptr(), attrName.c_str());
// }

/////////////// LocObject Methods //////////////

// LocObject::LocObject(PyLoc& pl)
//     : Location(0.0, 0.0), trackingFitness(0), loc(&pl)
// {
// }

// < and > are based on objects' tracking fitnesses
// bool LocObject::operator < (const LocObject& other) const
// {
//     return trackingFitness < other.trackingFitness;
// }

// bool LocObject::operator > (const LocObject& other) const
// {
//     return trackingFitness > other.trackingFitness;
// }

// From me
// const float LocObject::getDist()
// {
//     // HACK for infinity values, shouldn't happen
//     if (isinf(loc->getXEst()) || isinf(loc->getYEst())) {
//         std::cout << "INFINITY DISTANCE" << std::endl;
//         return INFINITE_DISTANCE;
//     }

//     return hypotf((loc->getYEst()-y), (loc->getXEst()-x));
// }

// // From me
// const degrees LocObject::getBearing()
// {
//     return (NBMath::subPIAngle((NBMath::safe_atan2((loc->getYEst()-y),
//                                                    (loc->getXEst()-x)))
//                                - loc->getHEst()))*TO_DEG;
// }

// const float LocObject::getRelX()
// {
//     return std::fabs(getDist())*std::cos(getBearing());
// }

// const float LocObject::getRelY()
// {
//     return std::fabs(getDist())*std::sin(getBearing());
// }


//////////// LocBall Methods //////////////////

// LocBall::LocBall(PyLoc& pl, MyInfo& _my)
//     : Location(0.f, 0.f), my(&_my), loc(&pl)
// {
// }

// const float LocBall::getVelX()
// {
//     if (my->getTeamColor() == PY_TEAM_BLUE)
//         return loc->getBallXVelocityEst();
//     else return -(loc->getBallXVelocityEst());
// }

// const float LocBall::getVelY()
// {
//     if (my->getTeamColor() == PY_TEAM_BLUE)
//         return loc->getBallYVelocityEst();
//     else return -(loc->getBallYVelocityEst());
// }

// const float LocBall::getAccX()
// {
//     if (my->getTeamColor() == PY_TEAM_BLUE)
//         return loc->getBallXAccelerationEst();
//     else return -(loc->getBallXAccelerationEst());
// }

// const float LocBall::getAccY()
// {
//     if (my->getTeamColor() == PY_TEAM_BLUE)
//         return loc->getBallYAccelerationEst();
//     else return -(loc->getBallYAccelerationEst());
// }

// const degrees LocBall::getHeading()
// {
//     if (my->getTeamColor() == PY_TEAM_BLUE)
//         return (NBMath::safe_atan2((loc->getBallYEst()-loc->getYEst()),
//                                    (loc->getBallXEst()-loc->getXEst())))
//             *TO_DEG;
//     else return (NBMath::safe_atan2((loc->getYEst()-loc->getBallYEst()),
//                                     (loc->getXEst()-loc->getBallXEst())))
//              *TO_DEG;

// }

// Copies x and y values to comply with the location interface.
// void LocBall::update()
// {
//     if (my->getTeamColor() == PY_TEAM_BLUE) {
//         x = loc->getBallXEst();
//         y = loc->getBallYEst();
//     }
//     else {
//         x = FIELD_GREEN_WIDTH - loc->getBallXEst();
//         y = FIELD_GREEN_HEIGHT - loc->getBallYEst();
//     }

//     lastRelX = getRelX();
//     lastRelY = getRelY();

//     if (dx != 0) endY = getRelY() - (dy*(getRelX()/dx));
// }

/////////// MyInfo Methods /////////////////

// MyInfo::MyInfo(PyLoc &pl)
//     : RobotLocation(0.0, 0.0, 0.0), loc(&pl), team_color(PY_TEAM_BLUE),
//       playerNumber(3)
// {
// }

// Copy from loc
// void MyInfo::update()
// {
//     // x, y and h are relative, based on my team
//     if (team_color == PY_TEAM_BLUE) {
//         x = loc->getXEst();
//         y = loc->getYEst();
//         h = loc->getRadHEst();
//     }
//     else {
//         x = FIELD_GREEN_WIDTH - loc->getXEst();
//         y = FIELD_GREEN_HEIGHT - loc->getYEst();
//         h = radians(subPIAngle(float(loc->getRadHEst() - PI)));
//     }
// }

// const locScore MyInfo::getLocScoreTheta()
// {
//     if (loc->getHUncert() < GOOD_LOC_THETA_UNCERT_THRESH)
//         return GOOD_LOC;
//     if (loc->getHUncert() < OK_LOC_THETA_UNCERT_THRESH)
//         return OK_LOC;
//     else return BAD_LOC;
// }


// const locScore MyInfo::getLocScoreXY()
// {
//     if (loc->getXUncert() < GOOD_LOC_XY_UNCERT_THRESH &&
//         loc->getYUncert() < GOOD_LOC_XY_UNCERT_THRESH)
//         return GOOD_LOC;
//     if (loc->getXUncert() < OK_LOC_XY_UNCERT_THRESH &&
//         loc->getYUncert() < OK_LOC_XY_UNCERT_THRESH)
//         return OK_LOC;
//     else return BAD_LOC;
// }

// const locScore MyInfo::getLocScore()
// {
//     return std::min(getLocScoreTheta(), getLocScoreXY());
// }

//////////// Ball Methods /////////////

// Ball::Ball(VisualBall& vb, PyLoc& pl, MyInfo& mi)
//     : loc(new LocBall(pl, mi)), vis(&vb), dist(0.f), bearing(0.f)
// {
// }

// void Ball::update()
// {
//     bearing = loc->getBearing();
//     dist = loc->getDist();

//     if (vis->isOn()) {
//         loc->setDX(loc->getLastRelX() - loc->getRelX());
//         loc->setDY(loc->getLastRelY() - loc->getRelY());
//     }

//     loc->update();
// }

// bool Ball::hasattr(boost::python::object obj,
//                    std::string const &attrName) {
//     return PyObject_HasAttrString(obj.ptr(), attrName.c_str());
// }

}
}
