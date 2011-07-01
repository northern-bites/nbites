/**
 * Objects for python usage that contain pointers to both vision and loc.
 */
#include "CombinationObjects.h"

namespace noggin {
    using namespace py_constants;

//////////// Base Location Class Methods //////////////
    Location::Location(float _x = 0, float _y = 0)
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
        // HACK for infinity values
        if (isinf(other.x) || isinf(other.y)) {
            std::cout << "INFINITY DISTANCE" << std::endl;
            return INFINITE_DISTANCE;
        }

        return hypotf((other.y-y), (other.x-x));
    }

    const degrees Location::headingToInDeg(const Location& other)
    {
        return (std::atan2((other.y-y), (other.x-x)))*TO_DEG;
    }

    const radians Location::headingToInRad(const Location& other)
    {
        return (std::atan2((other.y-y), (other.x-x)));
    }

    const bool Location::inOppGoalBox()
    {
        return ( OPP_GOALBOX_LEFT_X < x &&  OPP_GOALBOX_RIGHT_X > x
                 && OPP_GOALBOX_TOP_Y > y && y > OPP_GOALBOX_BOTTOM_Y );
    }

    const bool Location::inMyGoalBox()
    {
        return ( x < MY_GOALBOX_RIGHT_X && MY_GOALBOX_TOP_Y > y &&
                 y > OPP_GOALBOX_BOTTOM_Y );
    }

    const bool Location::inCenterOfField()
    {
        return ((FIELD_HEIGHT*2.0/3.0) > y && y > (FIELD_HEIGHT/3.0));
    }

    const bool Location::inTopOfField()
    {
        return ((FIELD_HEIGHT*2.0/3.0) < y);
    }

    const bool Location::inBottomOfField()
    {
        return ((FIELD_HEIGHT/3.0) > y);
    }

///////// RobotLocation Methods //////////////

    RobotLocation::RobotLocation(float _x = 0, float _y = 0, degrees _h = 0)
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
                             vis_landmark id, MyInfo& _my)
        : LocObject(), vis(&vfo), visID(id), localID(0), bearing(0), dist(0),
          my(&_my)
    {
    }

    const float FieldObject::getLocDist() {
        return my->distTo(*this, true);
    }

    const degrees FieldObject::getLocBearing()
    {
        return my->getRelativeBearing(*this);
    }

    const float FieldObject::getRelX()
    {
        return std::fabs(dist)*std::cos(bearing);
    }

    const float FieldObject::getRelY()
    {
        return std::fabs(dist)*std::sin(bearing);
    }

    void FieldObject::setBest()
    {
        if (vis->isOn()) {
            bearing = vis->getBearing();
            dist = vis->getDistance();
        }
        else if (vis->getFramesOff() < LOST_OBJECT_FRAMES_THRESH) return;

        else {
            bearing = getLocBearing()*TO_RAD;
            dist = getLocDist();
        }
    }

    void FieldObject::associateWithRelativeLandmark(
        boost::python::tuple relLandmark)
    {
        x = boost::python::extract<float>(relLandmark[0])();
        y = boost::python::extract<float>(relLandmark[1])();
        localID = boost::python::extract<int>(relLandmark[2])();
    }

/////////////// LocObject Methods //////////////

    LocObject::LocObject()
        : Location(0.0, 0.0), trackingFitness(0)
    {
    }

    bool LocObject::operator < (const LocObject& other) const
    {
        return trackingFitness < other.trackingFitness;
    }

    bool LocObject::operator > (const LocObject& other) const
    {
        return trackingFitness > other.trackingFitness;
    }

/////////// MyInfo Methods /////////////////

    MyInfo::MyInfo(PyLoc &pl)
        : RobotLocation(0.0, 0.0, 0.0), loc(&pl), team_color(PY_TEAM_BLUE),
          playerNumber(3)
    {
    }

    void MyInfo::update()
    {
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

    const float MyInfo::distTo(FieldObject& other, bool forceCalc)
    {
        if (!forceCalc) return other.getDist();
        else return Location::distTo(other);
    }


}
