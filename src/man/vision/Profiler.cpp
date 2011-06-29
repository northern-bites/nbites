
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>

#include "Profiler.h"

// #define PRINT_CSV

static const char *PCOMPONENT_NAMES[] = {
  "Main Loop",
  "GetImage",
  "Vision",
  "Transform",
  "ThreshRuns",
  "Threshold",
  "FGHorizon",
  "Runs",
  "Object",

  "Edges",
  "Sobel",
  "Edge Peaks",

  "Hough Transform",
  "Mark Hough Edges",
  "Smooth Hough Space",
  "Hough Peaks",
  "Suppress Hough Lines",

  "Lines",
  "Vert Lines",
  "Hor Lines",
  "Create Lines",
  "Join Lines",
  "Fit Unused",
  "Intersect Lines",

  "MemoryVision",
  "MemoryVisionSensors",
  "MemoryMotionSensors",
  "MemoryImage",

  "Localization",
  "MCL",
  "Logging",

  "Python",
  "PyUpdate",
  "PyRun",

  "Lights",

  "DCM",

  "Switchboard",
  "Scripted Provider CalcJS",
  "ChoppedCommand",
  "Walk Provider CalcJS",
  "tick_legs()",
  "Head Provider CalcJS",
  "Enactor",

  "Comm",
  "TOOLConnect",
  "RoboGuardian",

  "Total"
};

// Map from subcomponent (index) to meta-component (value) for calculating
// summary percentages.  Mapping to self means no parent.
static const ProfiledComponent PCOMPONENT_SUB_ORDER[] = {
    /*P_MAIN                    --> */ P_TOTAL,
	/*P_GETIMAGE				--> */ P_MAIN,
	/*P_VISION					--> */ P_MAIN,
	/*P_TRANSFORM				--> */ P_VISION,
	/*P_THRESHRUNS				--> */ P_VISION,
	/*P_THRESHOLD				--> */ P_THRESHRUNS,
	/*P_FGHORIZON				--> */ P_THRESHRUNS,
	/*P_RUNS					--> */ P_THRESHRUNS,
	/*P_OBJECT					--> */ P_VISION,

    /*P_EDGES,                  --> */ P_VISION,
    /*P_SOBEL,                  --> */ P_EDGES,
    /*P_EDGE_PEAKS,             --> */ P_EDGES,

    /*P_HOUGH,                  --> */ P_VISION,
    /*P_MARK_EDGES,             --> */ P_HOUGH,
    /*P_SMOOTH,                 --> */ P_HOUGH,
    /*P_HOUGH_PEAKS,            --> */ P_HOUGH,
    /*P_SUPPRESS,               --> */ P_HOUGH,

	/*P_LINES					--> */ P_VISION,
	/*P_VERT_LINES,				--> */ P_LINES,
	/*P_HOR_LINES,				--> */ P_LINES,
	/*P_CREATE_LINES,			--> */ P_LINES,
	/*P_JOIN_LINES,				--> */ P_LINES,
	/*P_FIT_UNUSED,				--> */ P_LINES,
	/*P_INTERSECT_LINES,		--> */ P_LINES,

	/*P_MEMORY_VISION,          --> */ P_MAIN,
	/*P_MEMORY_VISION_SENSORS,  --> */ P_MAIN,
	/*P_MEMORY_MOTION_SENSORS,  --> */ P_MAIN,
	/*P_MEMORY_IMAGE,           --> */ P_MAIN,

	/*P_LOC                     --> */ P_MAIN,
	/*P_MCL                     --> */ P_LOC,
	/*P_LOGGING                 --> */ P_MAIN,

	/*P_PYTHON					--> */ P_MAIN,
	/*P_PYUPDATE				--> */ P_PYTHON,
	/*P_PYRUN					--> */ P_PYTHON,

	/*P_LIGHTS                  --> */ P_MAIN,

	/*P_DCM                     --> */ P_TOTAL,

	/*P_SWITCHBOARD				--> */ P_TOTAL,
	/*P_SCRIPTED				--> */ P_SWITCHBOARD,
	/*P_CHOPPED					--> */ P_SCRIPTED,
	/*P_WALK					--> */ P_SWITCHBOARD,
	/*P_TICKLEGS				--> */ P_WALK,
	/*P_HEAD					--> */ P_SWITCHBOARD,
	/*P_ENACTOR					--> */ P_SWITCHBOARD,

	/*P_COMM                    --> */ P_TOTAL,
	/*P_TOOLCONNECT             --> */ P_TOTAL,
	/*P_ROBOGUARDIAN            --> */ P_TOTAL,

	/*P_TOTAL                   --> */ P_TOTAL
};



/**
 * Some important notes about how the Profiler class is structured to work.
 *
 * After initialization, or a call to reset(), all times are 0, the current
 * frame is 0, and num_profile_frames is -1.  When profiling is on (true), a
 * call to nextFrame() will increase the current_frame counter and add the
 * times recorded for the last frames to the running sums.  If
 * num_profile_frames is -1, this will continue forever.  Otherwise, once
 * num_profile_frames frames have been profiled (nextFrame() called that many
 * times, or current_frame == num_total_frames - 1 upon entry to nextFrame()),
 * profiling will be automatically turned off, and if USE_PROFILER_AUTO_PRINT
 * defined, will print the results.
 *
 * Calling profileFrames(n) will cause profiling to be turned on AFTER THE NEXT
 * CALL TO nextFrame().  This is so Python can turn on profiling mid-frame, and
 * it won't start until the frame is over.  The current_frame counter starts
 * the next frame at 0, then is incremented at the next call to nextFrame().
 *
 * When automatically stopped by the frame counter in nextFrame(),
 * current_frame is not incremented.  Thus when profiling is manually turned
 * off or when automatically over, current_frame represents THE LAST FRAME
 * profiled (indexing is zero-based).  If this weren't the case, it would be
 * much more difficult to accurately present the current and total frame count
 * in the printing methods.  printSummary() should be called after profiling is
 * false, whether manual or automatic, while current_frame has not been
 * incremented.  printCurrent() should be called before the call to nextFrame()
 * at the end of the desired frame, as nextFrame() resets the lastTime array to
 * all zeros, in order to make sure mid-frame stoppages only result in a
 * lastTime of 0, instead of some wildly large value.
 *
 * Some of this could probably be made easier by adding individual frame
 * counters for each component, but I think it's overhead and anything more
 * complicated should just use actual C++/gcc/gdb profiling.
 *
 * That's all for now, folks.
 */

Profiler* Profiler::instance = NULL;

Profiler::Profiler (long long (*f) ())
    : printEmpty(true), maxPrintDepth(PRINT_ALL_DEPTHS),
      timeFunction(f), verbose(false) {
    if (instance == NULL) {
        instance = this;
    }
    reset();
}

Profiler::~Profiler ()
{
}

void
Profiler::profileFrames (int num_frames)
{
  num_profile_frames = num_frames;
  start_next_frame = true;
}

void
Profiler::reset ()
{
  profiling = false;
  start_next_frame = false;
  num_profile_frames = -1;
  current_frame = 0;

  for (int i = 0; i < NUM_PCOMPONENTS; i++) {
    enterTime[i] = 0;
    lastTime[i] = 0;
    sumTime[i] = 0;
    maxTime[i] = 0;
    minTime[i] = INT_MAX;
  }
}

bool
Profiler::nextFrame() {
  // trigger start of profiling
  if (start_next_frame) {
    profiling = true;
    return start_next_frame = false;
  }

  // still currently profiling
  if (profiling) {
    // reached end of preset profile frames
    if (num_profile_frames >= 0 && current_frame >= num_profile_frames - 1) {
      // at finish, stop profiling
      profiling = false;
#ifdef USE_PROFILER_AUTO_PRINT
      printSummary();
#endif
      return false;
    }else {
      // add this frame's times to the sums
      for (int i = 0; i < NUM_PCOMPONENTS; i++) {
        sumTime[i] += lastTime[i];
        if (PCOMPONENT_SUB_ORDER[i] == P_TOTAL) {
            sumTime[P_TOTAL] += lastTime[i];
        }
        lastTime[i] = 0;
      }
      // continue to the next frame
      current_frame++;
      return true;
    }
  }else
    return false;
}

void
Profiler::printCurrent ()
{
  printf("Profiler Data: Frame %i:\n", (current_frame-1));
  for (int i = 0; i < NUM_PCOMPONENTS; i++) {
    printf("%-13s: %.6llu last, %.10llu total\n", PCOMPONENT_NAMES[i],
        lastTime[i], sumTime[i]);
  }
}

void
Profiler::printSummary ()
{
#ifdef PRINT_CSV
    printCSVSummary();
#else
    printIndentedSummary();
#endif /* PRINT_CSV */
}

void
Profiler::printCSVSummary()
{
    printf("%s,%s,%s", "Component Name", "Sum Time", "Avg. Time\n");
    for (int i = 0; i < NUM_PCOMPONENTS; ++i) {
        if (shouldNotPrintLine(i))
            continue;
        printf("%s,%.10llu,%.6llu\n",
               PCOMPONENT_NAMES[i],
               sumTime[i],
               (sumTime[i]/(current_frame+1) )
            );
    }
}

void
Profiler::printIndentedSummary()
{
  printf("Profiler Summary: %i Frames\n", (current_frame+1));

  // Calculate depths of sub-components, for indented display
  int depths[NUM_PCOMPONENTS];
  int length, max_length = 0;
  int comp;
  for (int i = 0; i < NUM_PCOMPONENTS; i++) {

    depths[i] = 0;
    comp = i;
	//comp = PCOMPONENT_SUB_ORDER[i];
    while (comp != PCOMPONENT_SUB_ORDER[comp]) {
      depths[i]++;
      comp = PCOMPONENT_SUB_ORDER[comp];
    }

    length = strlen(PCOMPONENT_NAMES[i]) + depths[i]*2;
    max_length = max_length > length ? max_length : length;
  }

  // Calculate and display the percentages (and totals) for each component
  float parent_sum;
  for (int i = 0; i < NUM_PCOMPONENTS; i++) {
    comp = PCOMPONENT_SUB_ORDER[i];
    parent_sum = (float)sumTime[comp];

    if (shouldNotPrintLine(i) &&
        (maxPrintDepth != PRINT_ALL_DEPTHS && depths[i] > maxPrintDepth))
        continue;

    // depth-based indentation
    printf("%*s", depths[i]*2, "");
    if (sumTime[i] == 0)
      printf("  %-*s:      0%% (0000000000 total, 000000 avg.)\n",
          (max_length-depths[i]*2), PCOMPONENT_NAMES[i]);
    else if (parent_sum == 0)
      printf("  %-*s: 100.00%% (%.10llu total, %.6llu avg.,"
             " %llu min, %.6llu max )\n",
             (max_length-depths[i]*2), PCOMPONENT_NAMES[i], sumTime[i],
             (sumTime[i] / (current_frame+1)), minTime[i], maxTime[i]);
    else {
        if (verbose == true) {
            printf("  %-*s: %6.2f%% (%.10llu total, %.6llu avg.,"
                    " %.6llu min, %.6llu max)\n",
                    (max_length-depths[i]*2), PCOMPONENT_NAMES[i],
                    ((float)sumTime[i] / parent_sum * 100), sumTime[i],
                    (sumTime[i] / (current_frame+1)), minTime[i], maxTime[i]);
        } else {
            printf("  %-*s: %6.2f%% (%.10llu total, %.6llu avg.)\n",
                            (max_length-depths[i]*2), PCOMPONENT_NAMES[i],
                            ((float)sumTime[i] / parent_sum * 100), sumTime[i],
                            (sumTime[i] / (current_frame+1)));
        }
    }
  }
}


