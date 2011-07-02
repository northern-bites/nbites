/**
 * Objects for python usage that contain pointers to both vision and loc.
 * See the header for in-depth descriptions.
 */
#include "CombinationObjects.h"

namespace noggin {
    using namespace py_constants;

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

    boost::python::tuple Location::toTupleXY()
    {
        return boost::python::make_tuple(x, y);
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

    // In methods check if loc is in a certain field region
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

///////// RobotLocation Methods //////////////

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

    const degrees RobotLocation::getRelativeBearing(Location& other)
    {
        return (NBMath::subPIAngle(headingToInRad(other) - h))*TO_DEG;
    }

    // Returns -1, 0, 1 to tell the robot which way to spin
    const float RobotLocation::spinDirToPoint(Location& other)
    {
        float targetH = getRelativeBearing(other);
        if(targetH == 0) return 0;
        else return NBMath::sign(targetH);
    }


//////////// RelLocation Methods ///////////////

    RelLocation::RelLocation(RobotLocation& my, float dx, float dy, degrees dh)
        : RobotLocation(my.getX() + dx, my.getY() + dy, my.getH() + dh),
          relX(dx), relY(dy), relH(dh*TO_RAD)
    {
    }

    RelLocation::RelLocation(const RelLocation& other)
        :RobotLocation(other.x, other.y, other.h), relX(other.relX),
         relY(other.relY), relH(other.relH)
    {
    }

////////// FieldObject Methods //////////////

    FieldObject::FieldObject(VisualFieldObject& vfo,
                             vis_landmark id, PyLoc& _pl)
        : vis(&vfo), loc(new LocObject(_pl)),
          visID(id), localID(0), bearing(0), dist(0)
    {
    }

    const float FieldObject::getRelX()
    {
        return std::fabs(dist)*std::cos(bearing);
    }

    const float FieldObject::getRelY()
    {
        return std::fabs(dist)*std::sin(bearing);
    }

    // Determine whether vis or loc values are better
    void FieldObject::setBest()
    {
        // Use vis values if the object is seen
        if (vis->isOn()) {
            bearing = vis->getBearing();
            dist = vis->getDistance();
        }
        // Or if it was seen recently
        else if (vis->getFramesOff() < LOST_OBJECT_FRAMES_THRESH) return;

        else {
            bearing = loc->getBearing()*TO_RAD;
            dist = loc->getDist();
        }
    }

    // Links this object to a relative landmark, gives it an x, y, and name
    void FieldObject::associateWithRelativeLandmark(
        boost::python::tuple relLandmark)
    {
        loc->setX(boost::python::extract<float>(relLandmark[0])());
        loc->setY(boost::python::extract<float>(relLandmark[1])());
        localID = boost::python::extract<int>(relLandmark[2])();
    }

/////////////// LocObject Methods //////////////

    LocObject::LocObject(PyLoc& pl)
        : Location(0.0, 0.0), loc(&pl), trackingFitness(0)
    {
    }

    // < and > are based on objects' tracking fitnesses
    bool LocObject::operator < (const LocObject& other) const
    {
        return trackingFitness < other.trackingFitness;
    }

    bool LocObject::operator > (const LocObject& other) const
    {
        return trackingFitness > other.trackingFitness;
    }

    // From me
    const float LocObject::getDist()
    {
        // HACK for infinity values, shouldn't happen
        if (isinf(loc->getXEst()) || isinf(loc->getYEst())) {
            std::cout << "INFINITY DISTANCE" << std::endl;
            return INFINITE_DISTANCE;
        }

        return hypotf((loc->getYEst()-y), (loc->getXEst()-x));
    }

    // From me
    const degrees LocObject::getBearing()
    {
        return (NBMath::subPIAngle((NBMath::safe_atan2((loc->getYEst()-y),
                                                       (loc->getXEst()-x)))
                                   - loc->getHEst()))*TO_DEG;
    }

//////////// LocBall Methods //////////////////

    LocBall::LocBall(PyLoc& pl, MyInfo& _my)
        : LocObject(pl), my(&_my)
    {
    }

    const float LocBall::getVelX()
    {
        if (my->getTeamColor() == PY_TEAM_BLUE)
            return loc->getBallXVelocityEst();
        else return -(loc->getBallXVelocityEst());
    }

    const float LocBall::getVelY()
    {
        if (my->getTeamColor() == PY_TEAM_BLUE)
            return loc->getBallYVelocityEst();
        else return -(loc->getBallYVelocityEst());
    }

    const float LocBall::getAccX()
    {
        if (my->getTeamColor() == PY_TEAM_BLUE)
            return loc->getBallXAccelerationEst();
        else return -(loc->getBallXAccelerationEst());
    }

    const float LocBall::getAccY()
    {
        if (my->getTeamColor() == PY_TEAM_BLUE)
            return loc->getBallYAccelerationEst();
        else return -(loc->getBallYAccelerationEst());
    }

    const degrees LocBall::getHeading()
    {
        return (NBMath::safe_atan2((loc->getBallYEst()-loc->getYEst()),
                                   (loc->getBallXEst()-loc->getXEst())))*TO_DEG;
    }

    // Copies x and y values to comply with the location interface. 
    void LocBall::update()
    {
        if (my->getTeamColor() == PY_TEAM_BLUE) {
            x = loc->getBallXEst();
            y = loc->getBallYEst();
        }
        else {
            x = FIELD_GREEN_WIDTH - loc->getBallXEst();
            y = FIELD_GREEN_HEIGHT - loc->getBallYEst();
        }

        dx = lastRelX - getRelX();
        dy = lastRelY - getRelY();

        lastRelX = getRelX();
        lastRelY = getRelY();

        if (dx != 0) endY = getRelY() - (dy*(getRelX()/dx));
    }

/////////// MyInfo Methods /////////////////

    MyInfo::MyInfo(PyLoc &pl)
        : RobotLocation(0.0, 0.0, 0.0), loc(&pl), team_color(PY_TEAM_BLUE),
          playerNumber(3)
    {
    }

    // Copy from loc
    void MyInfo::update()
    {
        // x, y and h are relative, based on my team
        if (team_color == PY_TEAM_BLUE) {
            x = loc->getXEst();
            y = loc->getYEst();
            h = loc->getRadHEst();
        }
        else {
            x = FIELD_GREEN_WIDTH - loc->getXEst();
            y = FIELD_GREEN_HEIGHT - loc->getYEst();
            h = radians(subPIAngle(loc->getRadHEst() - PI));
        }
    }

    const locScore MyInfo::getLocScoreTheta()
    {
        if (loc->getHUncert() < GOOD_LOC_THETA_UNCERT_THRESH)
            return GOOD_LOC;
        if (loc->getHUncert() < OK_LOC_THETA_UNCERT_THRESH)
            return OK_LOC;
        else return BAD_LOC;
    }


    const locScore MyInfo::getLocScoreXY()
    {
        if (loc->getXUncert() < GOOD_LOC_XY_UNCERT_THRESH &&
            loc->getYUncert() < GOOD_LOC_XY_UNCERT_THRESH)
            return GOOD_LOC;
        if (loc->getXUncert() < OK_LOC_XY_UNCERT_THRESH &&
            loc->getYUncert() < OK_LOC_XY_UNCERT_THRESH)
            return OK_LOC;
        else return BAD_LOC;
    }

    const locScore MyInfo::getLocScore()
    {
        return std::min(getLocScoreTheta(), getLocScoreXY());
    }

}
