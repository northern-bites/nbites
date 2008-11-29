
#ifndef TOOLConnect_H
#define TOOLConnect_H

#include "Common.h"

class TOOLConnect; // forward reference

#if ROBOT(AIBO)
#  include "OVision.h"
#elif ROBOT(NAO)
#  include <boost/shared_ptr.hpp>
#  include "corpus/synchro.h"
#  include "Sensors.h"
#  include "Vision.h"
#else
#  error Undefined robot type
#endif

#include "CommDef.h"
#include "DataSerializer.h"

//
// DataRequest struct definition
//

#define SIZEOF_REQUEST 10

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
}

//
// TOOLConnect class definition
//

class TOOLConnect
#if ROBOT(NAO)
  : public Thread
#endif
{
  public:
#if ROBOT(AIBO)
    TOOLConnect(OVision *s);
#else
    TOOLConnect(boost::shared_ptr<Synchro> _synchro, Sensors *s, Vision *v);
#endif
    ~TOOLConnect();

#if ROBOT(AIBO)
    void runStep();
#else
    void run();
#endif

    std::string getRobotName();
    int getState() { return state; }

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
#if ROBOT(AIBO)
    OVision *vision; // access to all robot information
#elif ROBOT(NAO)
    Sensors *sensors; // thread-safe access to sensor information
    Vision *vision; // access to vision processing information
#endif
};

#endif /* TOOLConnect_H */
