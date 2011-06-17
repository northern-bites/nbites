#include "Common.h"

#include <sys/utsname.h> // uname()
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "TOOLConnect.h"
#include "CommDef.h"
#include "Kinematics.h"
#include "SensorDef.h"
#include "MMLocEKF.h"

using std::vector;
using namespace boost;

//
// Debugging ifdef switches
//

//#define DEBUG_TOOL_CONNECTS
//#define DEBUG_TOOL_REQUESTS
//#define DEBUG_TOOL_COMMANDS

//
// Begin class code
//

TOOLConnect::TOOLConnect (shared_ptr<Synchro> _synchro, shared_ptr<Sensors> s,
                          shared_ptr<Vision> v, shared_ptr<GameController> gc)
    : Thread(_synchro, "TOOLConnect"),
      state(TOOL_REQUESTING),
      sensors(s), vision(v), gameController(gc),
      loc(), ballEKF()
{
}

TOOLConnect::~TOOLConnect ()
{
}

void TOOLConnect::setLocalizationAccess (shared_ptr<LocSystem> _loc,
                                         shared_ptr<BallEKF> _ballEKF)
{
#ifdef USE_MM_LOC_EKF
	loc = dynamic_pointer_cast<MMLocEKF>(_loc);
#else
  loc = _loc;
#endif
  ballEKF = _ballEKF;
}

void
TOOLConnect::run ()
{
    running = true;
    trigger->on();

    try {
        serial.bind();

        while (running) {
            serial.accept();
#ifdef DEBUG_TOOL_CONNECTS
            printf("Connection received from the TOOL\n");
#endif

            try {
                while (running && serial.connected()) {
                    receive();
                }
            }catch (socket_error& e) {
                if (running) {
                    fprintf(stderr, "Caught error on socket.  TOOL connection reset.\n");
                    fprintf(stderr, "%s\n", e.what());
                }
            }
        }
    }catch (socket_error &e) {
        if (running) {
            fprintf(stderr, "Error occurred in TOOLConnect, thread has stopped.\n");
            fprintf(stderr, "%s\n", e.what());
        }
    }

    serial.closeAll();

    running = false;
    trigger->off();
}

void
TOOLConnect::reset ()
{
    serial.close();
    state = TOOL_REQUESTING;
}

void
TOOLConnect::receive () throw(socket_error&)
{
    byte b = serial.read_byte();

    if (b == REQUEST_MSG) {
        state = TOOL_REQUESTING;

        byte buf[SIZEOF_REQUEST];
        DataRequest r;

        // read request from socket
        serial.read_bytes((byte*)&buf[0], SIZEOF_REQUEST);
        setRequest(r, buf);

#ifdef DEBUG_TOOL_REQUESTS
        printf("TOOL request received: ijsItomlS\n");
        printf("                       ");
        for (int i = 0; i < SIZEOF_REQUEST; i++)
            buf[i] ? printf("1") : printf("0");
        printf("\n");
#endif

        // pass off request to handler
        handle_request(r);

    } else if (b == COMMAND_MSG) {

        state = TOOL_COMMANDING;
        int cmd = serial.read_int();
#ifdef DEBUG_TOOL_COMMANDS
        printf("Command received: type=%i\n", cmd);
#endif

        handle_command(cmd);

    } else if (b == DISCONNECT) {

        reset();

    } else {

        fprintf(stderr, "Unimplemented message type received.  "
                "TOOL connection reset.\n");
        reset();
    }
}

void
TOOLConnect::handle_request (DataRequest &r) throw(socket_error&)
{

    // Robot information request
    if (r.info) {
        serial.write_byte(ROBOT_TYPE);
        //TODO: this is dumb, remove it
        std::string name = "";
        serial.write_bytes((const byte*)name.c_str(), name.size());
        // TODO - get calibration file name access
        serial.write_bytes((byte*)"table.mtb", strlen("table.mtb"));
    }

    std::vector<float> v;

    // Joint data request
    if (r.joints) {
        v = sensors->getVisionBodyAngles(); // Use sensors
        serial.write_floats(v);
    }

    // Sensor data request
    if (r.sensors) {
        v = sensors->getAllSensors();
        serial.write_floats(v);
    }

    // Image data request
    if (r.image) {
        sensors->lockImage();
        serial.write_bytes(
            reinterpret_cast<const uint8_t*>(sensors->getNaoImage()),
            NAO_IMAGE_BYTE_SIZE);
        sensors->releaseImage();
    }

    if (r.thresh)
        // send thresholded image
        serial.write_bytes(
            reinterpret_cast<const uint8_t*>(sensors->getColorImage()),
            COLOR_IMAGE_BYTE_SIZE);

	if (r.objects) {
		if (loc.get()) {
			vector<Observation> obs = loc->getLastObservations();
			vector<float> obs_values;

			for (unsigned int i=0; i < obs.size() ; ++i){
				obs_values.push_back(static_cast<float>(obs[i].getID()));
				obs_values.push_back(obs[i].getVisDistance());
				obs_values.push_back(obs[i].getVisBearing());
			}
			serial.write_floats(obs_values);
		}
	}

    if (r.local) {
        // send localization data
        vector<float> loc_values;

        if (loc.get()) {
			loc_values += loc->getXEst(), loc->getYEst(),
				loc->getHEst(), loc->getXUncert(),
				loc->getYUncert(),
				loc->getHUncert();
			loc_values += ballEKF->getXEst(), ballEKF->getYEst(),
				ballEKF->getXUncert(), ballEKF->getYUncert(),
				ballEKF->getXVelocityEst(), ballEKF->getYVelocityEst(),
				ballEKF->getXVelocityUncert(),
				ballEKF->getYVelocityUncert();
          loc_values += loc->getLastOdo().deltaF, loc->getLastOdo().deltaL,
                        loc->getLastOdo().deltaR;
        } else
          for (int i = 0; i < 19; i++)
            loc_values += 0;

        serial.write_floats(loc_values);
    }

	if (r.comm) {
		vector<int> gc_values;
		gc_values += gameController->team(),
			gameController->player(),
			gameController->color();
		serial.write_ints(gc_values);
	}

	if (r.mmekf){
#ifdef USE_MM_LOC_EKF
		const list<LocEKF*> models = loc->getModels();
		list<LocEKF*>::const_iterator model;
		vector<float> mm_values;

		for(model = models.begin(); model != models.end() ; ++model){
			if (!(*model)->isActive())
				continue;
			mm_values += (*model)->getXEst(),
				(*model)->getYEst(),
				(*model)->getHEst(),
				(*model)->getXUncert(),
				(*model)->getYUncert(),
				(*model)->getHUncert();
		}
		serial.write_floats(mm_values);
#endif
	}
}

void
TOOLConnect::handle_command (int cmd) throw(socket_error&)
{
    switch (cmd) {
    case CMD_TABLE:
        break;

    case CMD_MOTION:
        break;

    case CMD_HEAD:
        break;

    case CMD_JOINTS:
        break;

    default:
        fprintf(stderr, "Unimplemented command type");
    }
}
