
#ifndef _Profiler_h_DEFINED
#define _Profiler_h_DEFINED

#include "profileconfig.h"
#include <algorithm>
class Profiler;

#ifdef USE_TIME_PROFILING
#  define PROF_NFRAME()  (Profiler::getInstance()->nextFrame());
#  define PROF_ENTER(c) (Profiler::getInstance()->profiling && \
        Profiler::getInstance()->enterComponent(c));
#  define PROF_EXIT(c)  (Profiler::getInstance()->profiling && \
        Profiler::getInstance()->exitComponent(c));
#else
#  define PROF_NFRAME()
#  define PROF_ENTER(c)
#  define PROF_EXIT(c)
#endif

enum ProfiledComponent {
    P_MAIN = 0,
    P_GETIMAGE,
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

    P_LOC,
    P_MCL,
    P_LOGGING,

    P_PYTHON,
    P_PYUPDATE,
    P_PYRUN,

    P_LIGHTS,

    P_DCM,

    P_SWITCHBOARD,
    P_SCRIPTED,
    P_CHOPPED,
    P_WALK,
    P_TICKLEGS,
    P_HEAD,
    P_ENACTOR,

    P_COMM,
    P_TOOLCONNECT,
    P_ROBOGUARDIAN,

    P_TOTAL
};
static const int NUM_PCOMPONENTS = P_TOTAL + 1;

class Profiler {
  public:

    Profiler(long long (*thread_time_f)(),
             long long (*global_time_f)());
    ~Profiler();

    void profileFrames(int num_frames);
    void reset();

    void printCurrent();
    void printSummary();
    void printCSVSummary();
    void printIndentedSummary();

    bool nextFrame();

    static Profiler* getInstance() { return instance;}

    inline bool enterComponent(ProfiledComponent c) {
      enterTime[c] = thread_timeFunction();
      return profiling;
    }
    inline bool exitComponent(ProfiledComponent c) {
      lastTime[c] = thread_timeFunction() - enterTime[c];
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

    long long (*thread_timeFunction) ();
    long long (*global_timeFunction) ();

    bool start_next_frame;
    int num_profile_frames;
    int current_frame;
    long long enterTime[NUM_PCOMPONENTS];
    long long lastTime[NUM_PCOMPONENTS];
    long long profile_start_time;

    static Profiler* instance;

    bool verbose;

public:
    long long minTime[NUM_PCOMPONENTS];
    long long maxTime[NUM_PCOMPONENTS];
    long long sumTime[NUM_PCOMPONENTS];
};

#endif

