

#include "Common.h"

#if ROBOT(NAO)
#include <sys/utsname.h> // uname()
#include <boost/shared_ptr.hpp>
#endif

#include "TOOLConnect.h"
#include "CommDef.h"
#if ROBOT(AIBO)
#  include "MotionDef.h"
#elif ROBOT(NAO)
#  include "Kinematics.h"
#else
#assert false
#endif
#include "SensorDef.h"


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


#if ROBOT(AIBO)
TOOLConnect::TOOLConnect (OVision *v)
  : state(TOOL_REQUESTING), vision(v)
#else
TOOLConnect::TOOLConnect (shared_ptr<Synchro> _synchro, Sensors *s, Vision *v)
  : Thread(_synchro, "TOOLConnect"),
    state(TOOL_REQUESTING),
    sensors(s), vision(v)
#endif
{
#if ROBOT(AIBO)
  serial.setblocking(false);
#endif
}

TOOLConnect::~TOOLConnect ()
{
}

#if ROBOT(NAO)
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
          reset();
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

#elif ROBOT(AIBO)

void
TOOLConnect::runStep ()
{
  // bind socket to port
  if (!serial.bound()){
    try {
      serial.bind();
    }catch (socket_error& e) {
      fprintf(stderr, "TOOLConnect caught an error on bind.\n");
      fprintf(stderr, "%s\n", e.what());
      return;
    }
  }

  // receive incoming connections
  if (!serial.connected()) {
    try {
      serial.accept();

#ifdef DEBUG_TOOL_CONNECTS
      printf("Connection received from the TOOL\n");
#endif
    }catch (socket_error& e) {
      // silently ignore non-blocking errors
      if (e.error() != EAGAIN) {
        fprintf(stderr, "TOOLConnect caught an error on accept.\n");
        fprintf(stderr, "%s\n", e.what());
      }
      return;
    }
  }

  // receive a single message
  try {
    receive();
  }catch (socket_error& e) {
    // silently ignore non-blocking errors
    if (e.error() != EAGAIN) {
      fprintf(stderr, "Caught error on socket.  TOOL connection reset.\n");
      fprintf(stderr, "%s\n", e.what());
      reset();
    }
  }
}

#else

#  error "Undefined robot type"

#endif /* ROBOT(NAO|AIBO) */

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

#if ROBOT(AIBO)
  try {
    serial.setblocking(true);
#endif

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

#if ROBOT(AIBO)
    serial.setblocking(false);

  }catch (socket_error& e) {
    try {
      serial.setblocking(false);
    }catch (socket_error& e2) {
      fprintf(stderr, "TOOLConnect failed to reset blocking mode.  "
          "Connection reset.\n");
      reset();
      throw e2;
    }
    throw e;
  }
#endif
}

void
TOOLConnect::handle_request (DataRequest &r) throw(socket_error&)
{

  // Robot information request
  if (r.info) {
    serial.write_byte(ROBOT_TYPE);
    // TODO - get robot name access
    std::string name = vision->getRobotName();
    serial.write_bytes((const byte*)name.c_str(), name.size());
    // TODO - get calibration file name access
    serial.write_bytes((byte*)"table.mtb", strlen("table.mtb"));
  }

  std::vector<float> v;

  // Joint data request
  if (r.joints) {
#if ROBOT(AIBO)
    float tmp[NUM_ACTUATORS];
    for (int i = 0; i < NUM_ACTUATORS; i++)
      tmp[i] = vision->getJoint(i); // Use OVision method

    serial.write_floats(&tmp[0], NUM_ACTUATORS);
#elif ROBOT(NAO)
    v = sensors->getVisionBodyAngles(); // Use sensors
    serial.write_floats(v);
#endif
  }

  // Sensor data request
  if (r.sensors) {
#if ROBOT(AIBO)
    // Use OVision
#elif ROBOT(NAO)
    // Use Sensors
    v = sensors->getFSR();
    serial.write_floats(v);
    v = sensors->getInertial();
    serial.write_floats(v);
    v = sensors->getSonar();
    serial.write_floats(v);
#endif
  }

  // Image data request
  if (r.image) {
#if ROBOT(AIBO)
    static byte image[RAW_IMAGE_SIZE];
    byte *i_ptr = &image[0];
    int x, y;

    // use corrected image
    //
    for (y = 0; y < IMAGE_HEIGHT; y++) {
      // write each row, YYYYYY...UUUUUU...VVVVVV...
      for (x = 0; x < IMAGE_WIDTH; x++)
        *i_ptr++ = vision->thresh->getCorrY(x, y);
      for (x = 0; x < IMAGE_WIDTH; x++)
        *i_ptr++ = vision->thresh->getCorrU(x, y);
      for (x = 0; x < IMAGE_WIDTH; x++)
        *i_ptr++ = vision->thresh->getCorrV(x, y);
    }

    /*
    byte *i_ptr = &image[0], *img_end = i_ptr + IMAGE_BYTE_SIZE;
    const byte *v_ptr = vision->thresh->getYUV(), *row_start;

    // use uncorrected image
    //
    // write image into the entire image[] array
    while (i_ptr < img_end) {
      row_start = v_ptr;
      // write each row
      while (v_ptr < row_start + IMAGE_WIDTH * 3)
        *i_ptr++ = *v_ptr++;
      // then skip to the next
      v_ptr = row_start + IMAGE_ROW_OFFSET;
    }
    */

    serial.write_bytes(&image[0], IMAGE_BYTE_SIZE);
#elif ROBOT(NAO)
    sensors->lockImage();
    if (!vision->thresh->inverted) {
      serial.write_bytes(sensors->getImage(), IMAGE_BYTE_SIZE);
      sensors->releaseImage();
    }else {
      unsigned char image[IMAGE_BYTE_SIZE], swap;
      // copy raw image data
      memcpy(&image[0], sensors->getImage(), IMAGE_BYTE_SIZE);
      sensors->releaseImage();
      // swap U and V pixels
      for (int i = 1; i < IMAGE_BYTE_SIZE; i += 4) {
        swap = image[i];
        image[i] = image[i+2];
        image[i+2] = swap;
      }
      serial.write_bytes(&image[0], IMAGE_BYTE_SIZE);
    }
#else
#  error "Undefined robot type"
#endif
  }

  if (r.thresh)
    // send thresholded image
    serial.write_bytes(&vision->thresh->thresholded[0][0],
                       IMAGE_WIDTH * IMAGE_HEIGHT);
}

void
TOOLConnect::handle_command (int cmd) throw(socket_error&)
{
  switch (cmd) {
    case CMD_TABLE:
      break;

    case CMD_MOTION:
#if ROBOT(AIBO)
      double motion[NUM_MOTION_ENG];
      serial.read_doubles(&motion[0], NUM_MOTION_ENG);
      vision->subject[sbjSendMotion]->SetData(&motion[0],sizeof(motion));
      vision->subject[sbjSendMotion]->NotifyObservers();
      vision->setMotionNeeded(false);
#endif
      break;

    case CMD_HEAD:
#if ROBOT(AIBO)
      double head[NUM_HEAD_ENG];
      serial.read_doubles(&head[0], NUM_HEAD_ENG);
      vision->subject[sbjSendHead]->SetData(&head[0], sizeof(head));
      vision->subject[sbjSendHead]->NotifyObservers();
      vision->setHeadNeeded(false);
#endif
      break;

    case CMD_JOINTS:
      break;

    default:
      fprintf(stderr, "Unimplemented command type");
  }
}

