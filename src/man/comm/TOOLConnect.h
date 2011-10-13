#ifndef TOOLConnect_H
#define TOOLConnect_H

#include "Common.h"

class TOOLConnect; // forward reference

#  include <boost/shared_ptr.hpp>
#  include "synchro.h"
#  include "Sensors.h"
#  include "Vision.h"

#include "CommDef.h"
#include "DataSerializer.h"
#include "LocSystem.h"
#include "BallEKF.h"
#include "GameController.h"

//
// DataRequest struct definition
//

#define SIZEOF_REQUEST 11

struct DataRequest {
    bool info;
    bool joints;
    bool sensors;
    bool image;
    bool thresh;
    bool jpeg;
    bool objects;
    bool motion;
    bool local;
    bool comm;
    bool mmekf;
};

static void
setRequest (DataRequest &r, byte buf[SIZEOF_REQUEST])
{
    r.info    = buf[0];
    r.joints  = buf[1];
    r.sensors = buf[2];
    r.image   = buf[3];
    r.thresh  = buf[4];
    r.jpeg    = buf[5];
    r.objects = buf[6];
    r.motion  = buf[7];
    r.local   = buf[8];
    r.comm    = buf[9];
    r.mmekf    = buf[10];
}

//
// TOOLConnect class definition
//

class TOOLConnect
    : public Thread
{
public:
    TOOLConnect(boost::shared_ptr<Sensors> s,
                boost::shared_ptr<Vision> v,
                boost::shared_ptr<GameController> gc);
    virtual ~TOOLConnect();

    void run();

    std::string getRobotName();
    int getState() { return state; }

    void setLocalizationAccess(boost::shared_ptr<LocSystem> _loc,
                               boost::shared_ptr<BallEKF> _ballEKF);

private:
    void reset();
    void receive       ()               throw(socket_error&);
    void handle_request(DataRequest& r) throw(socket_error&);
    void handle_command(int cmd)        throw(socket_error&);

    int state;
    // Serialized connection to remote host
    DataSerializer serial;

    // References to global data structures
    //   on the Aibo's, we have neither threads nor Sensors class
    boost::shared_ptr<Sensors> sensors; // thread-safe access to sensors
    boost::shared_ptr<Vision> vision; // access to vision processing
    boost::shared_ptr<GameController> gameController; // access to GameController
    boost::shared_ptr<LocSystem> loc; // access to localization data
    boost::shared_ptr<BallEKF> ballEKF; // access to localization data
};

#endif /* TOOLConnect_H */
