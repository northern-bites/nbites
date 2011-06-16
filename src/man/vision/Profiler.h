
#ifndef _Profiler_h_DEFINED
#define _Profiler_h_DEFINED

#include "profileconfig.h"
#include <algorithm>


#ifdef USE_TIME_PROFILING
#  define PROF_NFRAME(p)  ((p)->nextFrame())
#  define PROF_ENTER(p,c) ((p)->profiling && (p)->enterComponent(c))
#  define PROF_EXIT(p,c)  ((p)->profiling && (p)->exitComponent(c))
#else
#  define PROF_NFRAME(p)
#  define PROF_ENTER(p,c)
#  define PROF_EXIT(p,c)
#endif

enum ProfiledComponent {
  P_GETIMAGE = 0,
  P_VISION,
  P_TRANSFORM,
  P_THRESHRUNS,
  P_THRESHOLD,
  P_FGHORIZON,
  P_RUNS,
  P_OBJECT,

  P_EDGES,
  P_SOBEL,
  P_EDGE_PEAKS,

  P_HOUGH,
  P_MARK_EDGES,
  P_SMOOTH,
  P_HOUGH_PEAKS,
  P_SUPPRESS,

  P_LINES,

  P_VERT_LINES,
  P_HOR_LINES,
  P_CREATE_LINES,
  P_JOIN_LINES,
  P_FIT_UNUSED,
  P_INTERSECT_LINES,

  P_MEMORY_VISION,
  P_MEMORY_VISION_SENSORS,
  P_MEMORY_MOTION_SENSORS,
  P_MEMORY_IMAGE,

  P_PYTHON,
  P_PYUPDATE,
  P_PYRUN,
  P_DCM,
  P_SWITCHBOARD,
  P_SCRIPTED,
  P_CHOPPED,
  P_WALK,
  P_TICKLEGS,
  P_HEAD,
  P_ENACTOR,
  P_LOC,
  P_MCL,
  P_LOGGING,
  P_AIBOCONNECT,
  P_TOOLCONNECT,
  P_LIGHTS,
  P_FINAL,
};
static const int NUM_PCOMPONENTS = P_FINAL + 1;

class Profiler {
  public:

    Profiler(long long (*f)());
    ~Profiler();

    void profileFrames(int num_frames);
    void reset();

    void printCurrent();
    void printSummary();
    void printCSVSummary();
    void printIndentedSummary();

    bool nextFrame();

    inline bool enterComponent(ProfiledComponent c) {
      enterTime[c] = timeFunction();
      return profiling;
    }
    inline bool exitComponent(ProfiledComponent c) {
      lastTime[c] = timeFunction() - enterTime[c];
      minTime[c] = std::min(lastTime[c], minTime[c]);
      maxTime[c] = std::max(lastTime[c], maxTime[c]);
      return profiling;
    }

    inline bool shouldNotPrintLine(int i) {
        // Don't print those times which are zero, i.e. they weren't run.
        return (!printEmpty && sumTime[i] == 0);
    }

  public:
    bool profiling, printEmpty;
    int maxPrintDepth;
    enum { PRINT_ALL_DEPTHS = -1 };

  private:

    long long (*timeFunction) ();

    bool start_next_frame;
    int num_profile_frames;
    int current_frame;
    long long enterTime[NUM_PCOMPONENTS];
    long long lastTime[NUM_PCOMPONENTS];

public:
    long long minTime[NUM_PCOMPONENTS];
    long long maxTime[NUM_PCOMPONENTS];
    long long sumTime[NUM_PCOMPONENTS];
};

#endif

