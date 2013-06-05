#ifndef _Profiler_h_DEFINED
#define _Profiler_h_DEFINED

#include <algorithm>
#include <boost/shared_ptr.hpp>
#include "Common.h"
#include "DebugConfig.h"
class Profiler;

#ifdef USE_TIME_PROFILING
#  define PROF_NFRAME()  (Profiler::getInstance()->nextFrame());
#  define PROF_ENTER(c) (Profiler::getInstance()->enterComponent(c));
#  define PROF_EXIT(c)  (Profiler::getInstance()->exitComponent(c));
#else
#  define PROF_NFRAME()
#  define PROF_ENTER(c)
#  define PROF_EXIT(c)
#endif

enum ProfiledComponent {
    // COGNITION THREAD
    P_COGNITION_THREAD = 0,

    P_TOP_DQBUF,
    P_BOT_DQBUF,
    P_ACQUIRE_IMAGE,

    P_VISION,
    P_TRANSFORM,
    P_THRESHRUNS,
    P_FGHORIZON,
    P_RUNS,
    P_OBJECT,
    P_ROBOTS,

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

    P_SELF_LOC,
    P_BALL_TRACK,

    P_BEHAVIORS,
    P_PYTHON,

    P_LIGHTS,

    // MOTION THREAD
    P_MOTION_THREAD,

    P_JOINT_ENACTOR,
    P_PRE_PROCESS,
    P_SEND_JOINTS,
    P_SEND_HARDNESS,

    P_MOTION,
    P_SCRIPTED,
    P_CHOPPED,
    P_WALK,
    P_HEAD,

    // COMM THREAD
    P_COMM_THREAD,
    P_COMM_RECEIVE,
    P_COMM_SEND,
    P_COMM_BUILD_PACKET,
    P_COMM_SERIALIZE_PACKET,
    P_COMM_TO_SOCKET,
    P_COMM_TIMER,

    // GUARDIAN THREAD
    P_GUARDIAN_THREAD,

    P_GUARDIAN,
    P_AUDIO,

    P_TOTAL
};

static const int NUM_PCOMPONENTS = P_TOTAL + 1;

class Profiler {
  public:

    Profiler(long long (*thread_time_f)(),
             long long (*process_time_f)(),
             long long (*global_time_f)());
    ~Profiler();

    void profileFrames(int num_frames);
    void reset();

    void printCurrent();
    void printSummary();
    void printCSVSummary();
    void printIndentedSummary();

    bool nextFrame();

    static Profiler* getInstance();

    inline void enterComponent(ProfiledComponent c) {
        if (!profiling)
            return;
        enterTime[c] = thread_timeFunction();
    }
    inline void exitComponent(ProfiledComponent c) {
        if (!profiling)
            return;
        lastTime[c] = thread_timeFunction() - enterTime[c];
        minTime[c] = std::min(lastTime[c], minTime[c]);
        maxTime[c] = std::max(lastTime[c], maxTime[c]);
    }

    inline bool shouldNotPrintLine(int i) {
        // Don't print those times which are zero, i.e. they weren't run.
        return (!printEmpty && sumTime[i] == 0);
    }

  public:
    enum { PRINT_ALL_DEPTHS = -1 };

  private:

    bool profiling;
    bool printEmpty;
    int maxPrintDepth;

    long long (*thread_timeFunction) ();
    long long (*process_timeFunction) ();
    long long (*global_timeFunction) ();

    bool start_next_frame;
    int num_profile_frames;
    int current_frame;
    long long enterTime[NUM_PCOMPONENTS];
    long long lastTime[NUM_PCOMPONENTS];
    long long profile_process_start_time;
    long long profile_global_start_time;

    bool verbose;

public:
    long long minTime[NUM_PCOMPONENTS];
    long long maxTime[NUM_PCOMPONENTS];
    long long sumTime[NUM_PCOMPONENTS];
};

#endif
