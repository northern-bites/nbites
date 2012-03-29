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
#include "PySender.h"

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

TOOLConnect::TOOLConnect (shared_ptr<Sensors> s, shared_ptr<Vision> v,
                          shared_ptr<GameController> gc)
    : Thread("TOOLConnect"),
      state(TOOL_REQUESTING),
      sensors(s), vision(v), gameController(gc),
      loc(), ballEKF(), sender(new CommandSender)
{
    set_sender_pointer(sender);
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
    try {
        serial.bind();

        while (running) {
            PROF_ENTER(P_TOOLCONNECT);
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
            PROF_EXIT(P_TOOLCONNECT);
        }
    }catch (socket_error &e) {
        if (running) {
            fprintf(stderr, "Error occurred in TOOLConnect, thread has stopped.\n");
            fprintf(stderr, "%s\n", e.what());
        }
    }

    serial.closeAll();

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

        // reads the 256-byte message send in from Java
        byte cmd[SIZEOF_COMMAND];
        serial.read_bytes((byte*)&cmd[0], SIZEOF_COMMAND);

#ifdef DEBUG_TOOL_COMMANDS
        printf("Command recieved: %s\n", cmd);
#endif

        // Updates the CommandSender with new information
        sender->update((const char*)cmd);

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
    }

    std::vector<float> v;

    // Joint data request
    if (r.joints) {
        v = sensors->getVisionBodyAngles(); // Use sensors
        serial.write_floats(v);
    }

    // Sensor data request
    if (r.sensors) {
        std::vector<float> sensor_data(NUM_SENSORS);
        FSR lfsr = sensors->getLeftFootFSR();
        sensor_data[0] = lfsr.frontLeft;
        sensor_data[1] = lfsr.frontRight;
        sensor_data[2] = lfsr.rearLeft;
        sensor_data[3] = lfsr.rearRight;
        FSR rfsr = sensors->getRightFootFSR();
        sensor_data[4] = rfsr.frontLeft;
        sensor_data[5] = rfsr.frontRight;
        sensor_data[6] = rfsr.rearLeft;
        sensor_data[7] = rfsr.rearRight;
        FootBumper lfb = sensors->getLeftFootBumper();
        sensor_data[8] = lfb.left;
        sensor_data[9] = lfb.right;
        FootBumper rfb = sensors->getRightFootBumper();
        sensor_data[10] = rfb.left;
        sensor_data[11] = rfb.right;
        Inertial inertial = sensors->getInertial();
        sensor_data[12] = inertial.accX;
        sensor_data[13] = inertial.accY;
        sensor_data[14] = inertial.accZ;
        sensor_data[15] = inertial.gyrX;
        sensor_data[16] = inertial.gyrY;
        sensor_data[17] = inertial.angleX;
        sensor_data[18] = inertial.angleY;
        sensor_data[19] = sensors->getUltraSoundLeft();
        sensor_data[20] = sensors->getUltraSoundRight();
        sensor_data[21] = sensors->getSupportFoot();
        serial.write_floats(sensor_data);
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
            vector<float> obs_values;

            // Add point values to observed values
            vector<PointObservation> obs =
                loc->getLastPointObservations();
            for (vector<PointObservation>::iterator i = obs.begin();
                 i != obs.end() ; ++i){
                obs_values.push_back(static_cast<float>(i->getID()));
                obs_values.push_back(i->getVisDistance());
                obs_values.push_back(i->getVisBearing());
            }

            // Add corners to observed values
            vector<CornerObservation> corners =
                loc->getLastCornerObservations();
            for (vector<CornerObservation>::iterator i = corners.begin();
                 i != corners.end() ; ++i){
                obs_values.push_back(static_cast<float>(i->getID()));
                obs_values.push_back(i->getVisDistance());
                obs_values.push_back(i->getVisBearing());
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

            loc_values += ballEKF->getGlobalX(),
                ballEKF->getGlobalY(),
                ballEKF->getGlobalXUncert(),
                ballEKF->getGlobalYUncert(),
                ballEKF->getGlobalXVelocity(),
                ballEKF->getGlobalYVelocity(),
                ballEKF->getGlobalXVelocityUncert(),
                ballEKF->getGlobalYVelocityUncert();

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
