
#ifndef Sensors_H
#define Sensors_H
#include <vector>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;
#include <list>
#include <pthread.h>
#include <Python.h>

#include "SensorDef.h"
#if ROBOT(AIBO)
#  include "MotionDef.h"
#elif ROBOT(NAO_RL)
#  include "NaoDef.h"
#else
#  error Undefined robot type
#endif
#include "VisionDef.h"

class Sensors;

//
// Python Sensors class definitions
//

typedef struct PySensors_t {
  PyObject_HEAD
  Sensors *_sensors;
  PyObject *angles;
  PyObject *errors;
#if ROBOT(NAO)
  PyObject *fsr;
  PyObject *inertial;
  PyObject *sonarLeft;
  PyObject *sonarRight;
#endif
  PyObject *image;
} PySensors;


struct FSR {
    FSR(const float fl, const float fr,
        const float rl, const float rr)
        : frontLeft(fl), frontRight(fr), rearLeft(rl), rearRight(rr) { }

    float frontLeft;
    float frontRight;
    float rearLeft;
    float rearRight;
};

struct FootBumper {
    // Since the foot bumpers only have pressed/unpressed states, but are stored
    // as floats with values 0.0f and 1.0f, we just test whether the value is
    // bigger than a half or not and assign a boolean based on that.
    FootBumper(const float _left, const float _right) {
        left = _left > 0.5f;
        right = _right > 0.5f;
    }

    bool left;
    bool right;
};


class Sensors {
  //friend class Man;
  public:
    Sensors();
    ~Sensors();

    // Locking data retrieval methods
    //   Each of these methods first locks the associated mutex, copies the
    //   requested values, then unlocks the mutex before returning
    std::vector<float> getBodyAngles();
    std::vector<float> getVisionBodyAngles();
    const float getBodyAngle(const int index); // NOT wrapped for python use
    std::vector<float> getBodyAngleErrors();
	const float getBodyAngleError(int index); // Also NOT wrapped for python
#if ROBOT(NAO)
    const FSR getLeftFootFSR();
    const FSR getRightFootFSR();
    const FootBumper getLeftFootBumper();
    const FootBumper getRightFootBumper();
    std::vector<float> getInertial();
    std::vector<float> getSonar();
#endif

    // Locking data storage methods
    //   Each of these methods first lockes the associated mutex, stores
    //   the specified values, then unlocks the mutex before returning
    void setBodyAngles(std::vector<float>& v);
    void setVisionBodyAngles(std::vector<float>& v);
    void setBodyAngleErrors(std::vector<float>& v);
#if ROBOT(NAO)
    void setLeftFootFSR(const float frontLeft, const float frontRight,
                        const float rearLeft, const float rearRight);
    void setRightFootFSR(const float frontLeft, const float frontRight,
                         const float rearLeft, const float rearRight);
    void setFSR(const FSR &leftFootFSR, const FSR &rightFootFSR);
    void setLeftFootBumper(const float left, const float right);
    void setLeftFootBumper(const FootBumper& bumper);
    void setRightFootBumper(const float left, const float right);
    void setRightFootBumper(const FootBumper& bumper);
    void setInertial(std::vector<float>& v);
    void setSonar(float l, float r);
#endif

    // special methods
    //   the image retrieval and locking methods are a little different, as we
    //   don't copy the raw image data.  If locking is needed for some period
    //   of time while processing, the lockImage() method is called, followed
    //   by getImage() and MUST be followed finally by relaseImage().  The
    //   getImage() method will always retrieve the latest image pointer, but
    //   it is only guaranteed to be valid and unmodified between calls to
    //   lockImage() and releaseImage().
    //   Also of import is that the locking is only valid if the source of the
    //   image pointer decides to abide by it.  setImage() does not not lock on
    //   its own, and there is no way, even with locking, to guarantee that the
    //   underlying data at the image pointer location is not modified while
    //   the image is locked in Sensors.
    const unsigned char* getImage();
    void setImage(const unsigned char* img);
    void lockImage();
    void releaseImage();

    void updatePython();
    // The following method will internally save a snapshot of the current body
    // angles. This way we can save joints that are synchronized to the most
    // current image. At the same time, the bodyAngles vector will still have the
    // most recent angles if some other module needs them.
    void updateVisionAngles();

  private:

    void add_to_module();

    // Locking mutexes
    pthread_mutex_t angles_mutex;
    pthread_mutex_t vision_angles_mutex;
    pthread_mutex_t errors_mutex;
#if ROBOT(NAO)
    pthread_mutex_t fsr_mutex;
    pthread_mutex_t bumper_mutex;
    pthread_mutex_t inertial_mutex;
    pthread_mutex_t sonar_mutex;
#endif
    pthread_mutex_t image_mutex;

    // Joint angles and sensors
    // Make the following distinction: bodyAngles is a vector of the most current
    // angles. visionBodyAngles is a snapshot of what the most current angles
    // were when the last vision frame started.
    std::vector<float> bodyAngles;
    std::vector<float> visionBodyAngles;
    std::vector<float> bodyAnglesError;
#if ROBOT(NAO)
    // FSR sensors
    FSR leftFootFSR;
    FSR rightFootFSR;
    // Feet bumper sensors
    FootBumper leftFootBumper;
    FootBumper rightFootBumper;
    // Inertial sensors
    std::vector<float> inertial;
    // Sonar sensors
    float sonarLeft;
    float sonarRight;
#endif

    const unsigned char *image;
    PySensors *pySensors;
};



PyMODINIT_FUNC init_sensors(void);

int c_init_sensors(void);


#endif /* Sensors_H */
