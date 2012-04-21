/**
 * Objects for python usage that contain pointers to both vision and loc.
 * Prevents unneccessary copying of vision, localization values into Python and
 * allows most calculations to be done here rather than in Python. Includes
 * Location, the base class for all of the others, RobotLocation, RelLocation,
 * LocObject, FieldObject, and MyInfo. See below for further description of
 * each class. These are wrapped using boost/python in PyObjects.*.
 */

#ifndef CombObjects_h_DEFINED
#define CombObjects_h_DEFINED

#include "VisualFieldObject.h"
#include "VisualLandmark.h"
#include "VisualBall.h"
#include "PyLoc.h"
#include "BallEKF.h"
#include "PyLoc.h"
#include "NBMath.h"
#include <vector>
#include <math.h>
#include "PyNogginConstants.h"
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
using boost::shared_ptr;

namespace noggin {

    // Forward declarations of all of the classes in this file
    class Location;
    class RobotLocation;
    class RelLocation;
    class RelRobotLocation;
    class LocObject;
    class FieldObject;
    class MyInfo;

    // Sections of the field for Location use
    static const float TOP_LIMIT = FIELD_HEIGHT*2.0f/3.0f;
    static const float BOTTOM_LIMIT = FIELD_HEIGHT/3.0f;

    // Buffer for size of Box
    static const int BOX_BUFFER = 10;

    /**
     * Degrees and radians are both floats, but used to indicate when
     * values are expected to be in degrees or radians. This gets complicated
     * since python requires degrees but all C++ values are stored in rads.
     */
     typedef float degrees, radians;

    /*
     * LOCATION
     * The base class for many others in this file. Holds x, y values
     * representing a location on the field. Contains methods to convert
     * these to a python tuple, get the distance and bearing to a location
     * from another location, and to determine whether a location is in
     * various parts of the field.
     */

    class Location
    {
    public:
        Location(float, float);
        virtual ~Location() {};
        Location(const Location& other);
        virtual bool operator == (const Location& other) const;
        virtual bool operator != (const Location& other) const;
        virtual RelLocation operator - (const Location& other) const;

        static const float INFINITE_DISTANCE = 10000000.0f;

        //Getters
        virtual const float getX(){ return x; }
        virtual const float getY(){ return y; }

        //Setters
        void setX(float _x){ x = _x; }
        void setY(float _y){ y = _y; }

        //Other
        boost::python::tuple toTupleXY();
        virtual const float distTo(const Location&);
        virtual const degrees headingToInDeg(const Location&);
        virtual const bool inOppGoalBox();
        virtual const bool inMyGoalBox();
        virtual const bool inCenterOfField();
        virtual const bool inTopOfField();
        virtual const bool inBottomOfField();
        virtual RelLocation getRelLocationOf(const Location& other) const;

        // For Python
        bool hasattr(boost::python::object, const std::string&);
        boost::python::str toString();

    protected:
        float x, y;
        const radians headingToInRad(const Location& other);
    };

    /*
     * ROBOT LOCATION
     * A location that also contains a heading, since a robot needs to know
     * the direction in which it's facing. Heading is stored in rads but
     * passed to python in degrees. Provides a method to get the relative
     * bearing to another location and to determine which way to spin to
     * face another point.
     */

    class RobotLocation : public Location
    {
    public:
        RobotLocation(float, float, degrees);
        virtual ~RobotLocation() {};
        RobotLocation(const RobotLocation& other);
        bool operator == (const RobotLocation& other) const;
        bool operator != (const RobotLocation& other) const;
        RelRobotLocation operator- (const RobotLocation& other) const;
        RobotLocation operator+ (const RelRobotLocation& other) const;
        RelRobotLocation getRelRobotLocationOf(const RobotLocation& other) const;
        // Extra getter
        virtual const degrees getH(){ return h*TO_DEG; }

        // Extra setter
        void setH(degrees _h){ h = _h*TO_RAD;}

        // Other
        const degrees getRelativeBearing(Location& other);
        const float spinDirToPoint(Location& other);

        //For python
        boost::python::str toString();

    protected:
        radians h;
    };

    /*
     * REL LOCATION
     * A robot location that is created by specifying differences in
     * x, y, and h from a given existing location. The dx, dy, and dh
     * are also stored.
     */

    class RelLocation
    {
    public:
        RelLocation(float dx = 0, float dy = 0);
        virtual ~RelLocation() {};
        RelLocation(const RelLocation& other);

        //Getters
        virtual const float getRelX() const { return relX; }
        virtual const float getRelY() const { return relY; }
        virtual const degrees getBearing() const {
            return NBMath::safe_atan2(relY, relX) * TO_DEG;
        }
        virtual const float getDist() const {
            return hypotf(relY, relX);
        }

        //Setters
        virtual void setRelX(float _x){ relX = _x; }
        virtual void setRelY(float _y){ relY = _y; }

        // For python
        virtual boost::python::str toString();

    protected:
        float relX, relY;
    };

    /*
     * REL LOCATION
     * A robot location that is created by specifying differences in
     * x, y, and h from a given existing location. The dx, dy, and dh
     * are also stored.
     */

    class RelRobotLocation : public RelLocation
    {
    public:
        RelRobotLocation(float dx = 0, float dy = 0, degrees dh = 0);
        virtual ~RelRobotLocation() {};
        RelRobotLocation(const RelRobotLocation& other);
        virtual RelRobotLocation operator- (boost::python::tuple delta) const;

        virtual const degrees getRelH() const { return relH*TO_DEG; }
        virtual void setRelH(degrees _h){ relH = _h*TO_RAD; }

        virtual void rotate(degrees theta);

        //region is a (x, y, h) tuple
        //check if this rel robot location is within the ellipse specified by
        // x, y and the heading specified by h
        bool within(boost::python::tuple region) const;

        // For python
        boost::python::str toString();

    protected:
        radians relH;
    };

    /*
     * LOC OBJECT
     * A class for objects that have locations. Provides a way to hold basic
     * tracking info.
     */

    class LocObject : public Location
    {
    public:
        LocObject(PyLoc&);
        virtual ~LocObject() {};

        // Loc getters
        const float getDist();
        const degrees getBearing();
        const float getRelX();
        const float getRelY();

        // Tracking: Is this used?
        const int getTrackingFitness() { return trackingFitness; }
        void setTrackingFitness(int tf) { trackingFitness = tf; }

        bool operator < (const LocObject& other) const;
        bool operator > (const LocObject& other) const;

    private:
        int trackingFitness;
        PyLoc* loc;
    };

    /*
     * LOC BALL
     * A loc object specialized to get all of the ball's loc values. Also needs
     * a pointer to MyInfo to make position, velocity, acceleration values
     * relative to the correct team.
     */

    class LocBall : public Location, public RelLocation
    {
    public:
        LocBall(PyLoc&, MyInfo&);

        //Loc getters
        const float getXUncert() { return loc->getBallXUncert(); }
        const float getYUncert() { return loc-> getBallYUncert(); }
        const float getSD() { return getXUncert()*getYUncert(); }
        const float getVelX();
        const float getVelY();
        const float getVelXUncert() { return loc->getXVelocityUncert(); }
        const float getVelYUncert() { return loc->getYVelocityUncert(); }
        const degrees getHeading();
        const float getRelX() const { return loc->getBallRelXEst(); }
        const float getRelY() const { return loc->getBallRelYEst(); }
        const float getRelVelX() { return loc->getRelXVelocityEst(); }
        const float getRelVelY() { return loc->getRelYVelocityEst(); }
        const float getAccX();
        const float getAccY();
        const float getAccXUncert() { return loc->getXAccelerationUncert(); }
        const float getAccYUncert() { return loc->getYAccelerationUncert(); }
        const float getRelAccX() { return loc->getRelXAccelerationEst(); }
        const float getRelAccY() { return loc->getRelYAccelerationEst(); }
        const float dX() { return dx; }
        const float dY() { return dy; }
        const float getEndY() { return endY; }
        const float getDist() const { return loc->getBallDistance(); }
        const degrees getBearing() const { return loc->getBallBearingDeg(); }

        // Other
        void update();
        void setDX(float _dx) { dx = _dx; }
        void setDY(float _dy) { dy = _dy; }
        const float getLastRelX() { return lastRelX; }
        const float getLastRelY() { return lastRelY; }

    private:
        MyInfo* my;
        PyLoc* loc;
        float lastRelX, lastRelY, dx, dy, endY;
    };

    /*
     * FIELD OBJECT
     * Currently means one of the four goalposts. A LocObject that also
     * holds a pointer to the visual object connected with a particular
     * location for quick access of vision info. VisID and localID describe
     * which landmark this object is; see PyNogginConstants for their
     * possible values. Dist and bearing are the best dist and bearing value,
     * depending on whether loc values or vision values are more reliable
     * for a particular frame. MyInfo is a pointer to the robot's current
     * information. Other info is available through the vis pointer. See
     * PyVision.cpp for the VisualFieldObject values exposed to Python.
     */

    class FieldObject
    {
    public:
        FieldObject(VisualFieldObject&,
                    py_constants::vis_landmark,
                    PyLoc&);
        ~FieldObject() {};

        VisualFieldObject* vis;
        LocObject* loc;

    private:
        // These shouldn't be copied!
        FieldObject(FieldObject& other);

        py_constants::vis_landmark visID;
        int localID;
        radians bearing;
        float dist;
        MyInfo* my;

    public:
        static const int LOST_OBJECT_FRAMES_THRESH = 7;

        const float getDist() { return dist; }
        const degrees getBearing() { return bearing*TO_DEG; }
        const py_constants::vis_landmark getVisID() { return visID; }
        const int getLocalID() { return localID; }

        // Other
        void setBest();
        void associateWithRelativeLandmark(boost::python::tuple relLandmark);

        // For Python
        bool hasattr(boost::python::object, const std::string&);
    };

    /*
     * MY INFO
     * A RobotLocation that stores localization information for the robot as
     * well as a few other useful infos, such as team color and number. Stores
     * a pointer to the loc system for easy access. Overloads distTo from
     * location so that it doesn't have to be calculated if not needed. Update
     * gets the latest x, y, h values from loc. This copying is necessary for
     * using the Location interface. LocScores represent how good localization
     * is, based on the three uncert values.
     */
    class MyInfo : public RobotLocation
    {
    public:
        MyInfo(PyLoc& pl);
        ~MyInfo() {};
        PyLoc* loc;

    private:
        MyInfo(const MyInfo& other);

    public:
        void update();

        //Getters
        const py_constants::teamColor getTeamColor() { return team_color; }
        const py_constants::locScore getLocScoreTheta();
        const py_constants::locScore getLocScoreXY();
        const py_constants::locScore getLocScore();
        const int getPlayerNumber() { return playerNumber; }
        const float getXUncert() { return loc->getXUncert(); }
        const float getYUncert() { return loc->getYUncert(); }
        const degrees getHUncert() { return loc->getHUncert(); }

        //Setters
        void setTeamColor(py_constants::teamColor tc) { team_color = tc; }
        void setPlayerNumber(int pn) { playerNumber = pn; }

    private:
        py_constants::teamColor team_color;
        int playerNumber;
    };

    /*
     * BALL
     * Similar to a FieldObject, but for the ball. Holds a pointer to both a loc
     * ball and a vision ball, accessible through loc and vis. Also stores the
     * best values for dist and bearing.
     */

    class Ball
    {
    public:
        Ball(VisualBall&, PyLoc&, MyInfo&);
        ~Ball() {};

        LocBall* loc;
        VisualBall* vis;

        // Getters
        const float getDist() { return dist; }
        const float getBearing() { return bearing; }

        void update();

        // For Python
        bool hasattr(boost::python::object, const std::string&);

    private:
        Ball(const Ball& other);
        float dist;
        degrees bearing;
    };
}
#endif //CombObjects_h_DEFINED
