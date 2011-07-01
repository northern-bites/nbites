/**
 * Objects for python usage that contain pointers to both vision and loc.
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
    class LocObject;
    class FieldObject;
    class MyInfo;

    /***************************
     *     LOCATION            *
     **************************/

    typedef float degrees, radians;

    class Location
    {
    public:
        Location(float, float);
        virtual ~Location() {};
        Location(const Location& other);
        virtual bool operator == (const Location& other) const;
        virtual bool operator != (const Location& other) const;

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

    protected:
        float x, y;
        const radians headingToInRad(const Location& other);
    };

    /***************************
     *     ROBOT LOCATION      *
     **************************/

    class RobotLocation : public Location
    {
    public:
        RobotLocation(float, float, degrees);
        virtual ~RobotLocation() {};
        RobotLocation(const RobotLocation& other);
        bool operator == (const RobotLocation& other) const;
        bool operator != (const RobotLocation& other) const;

        // Extra getter
        virtual const degrees getH(){ return h*TO_DEG; }

        // Extra setter
        void setH(degrees _h){ h = _h*TO_RAD;}

        // Other
        const degrees getRelativeBearing(Location& other);
        const float spinDirToPoint(Location& other);

    protected:
        radians h;
    };

    /***************************
     *     RELATIVE LOCATION   *
     **************************/

    class RelLocation : public RobotLocation
    {
    public:
        RelLocation(RobotLocation& my, float dx, float dy, degrees dh);
        ~RelLocation() {};
        RelLocation(const RelLocation& other);

        //Getters
        float getRelX(){ return relX; }
        float getRelY(){ return relY; }
        degrees getRelH(){ return relH*TO_DEG; }

        //Setters
        void setRelX(float _x){ relX = _x; }
        void setRelY(float _y){ relY = _y; }
        void setRelH(degrees _h){ relH = _h*TO_RAD; }

    private:
        float relX, relY;
        radians relH;
    };

    /***************************
     *     LOC OBJECT          *
     **************************/

    class LocObject : public Location
    {
    public:
        LocObject();
        virtual ~LocObject() {};

        // Tracking
        const int getTrackingFitness() { return trackingFitness; }
        void setTrackingFitness(int tf) { trackingFitness = tf; }

        bool operator < (const LocObject& other) const;
        bool operator > (const LocObject& other) const;

        // Implemented in inheriting classes:
        // virtual const float getLocDist();
        // virtual const float getLocBearing();
        // virtual const float getRelX();
        // virtual const float getRelY();

    private:
        int trackingFitness;
    };

    /***************************
     *     FIELD OBJECT        *
     **************************/

    class FieldObject : public LocObject
    {
    public:
        FieldObject(VisualFieldObject&,
                    py_constants::vis_landmark,
                    MyInfo&);
        ~FieldObject() {};

        VisualFieldObject* vis;

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

        // Loc getters
        const float getLocDist();
        const degrees getLocBearing();
        const float getRelX();
        const float getRelY();
        const float getDist() { return dist; }
        const degrees getBearing() { return bearing*TO_DEG; }
        const py_constants::vis_landmark getVisID() { return visID; }
        const int getLocalID() { return localID; }

        // Other
        void setBest();
        void associateWithRelativeLandmark(boost::python::tuple relLandmark);
    };

   /***************************
     *     MY INFO            *
     **************************/
    // Copies 3 values per frame from Loc. Needed for Location inheritance
    // compatibility.

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
        const float distTo(FieldObject& other, bool forceCalc);

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

}
#endif //CombObjects_h_DEFINED
