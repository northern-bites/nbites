// ************************************************
// *                                              *
// *  RoboCup Image Acquisition Front End Tester  *
// *                                              *
// ************************************************

#ifndef _main_
#define _main_

#include "Stdafx.h"
#include <stdlib.h>
#include "Vision.h"
#include "Homography.h"

using namespace std;
using namespace man::vision;

// ************
// *          *
// *  Timing  *
// *          *
// ************
//
// This timer is intended to compare times for ASM and C++ versions. It
// doesn't actually time anything, just gather statistics and print a
// report

class CodeTimer
{
  int count;
  double sT[2], sT2[2];
  list<int> times[2];

public:
  double mhz;

  CodeTimer()
  {
    clear();
    mhz = 1600.0;
  }

  void clear()
  {
    for (int i = 0; i < 2; ++i)
    {
      sT[i] = sT2[i] = 0;
      times[i].clear();
    }
    count = 0;
  }

  void add(uint32_t t[])
  {
    // Keep ASM timing stats
    for (int i = 0; i < 2; ++i)
    {
      sT[i] += t[i];
      sT2[i] += (double)t[i] * t[i];
      times[i].push_back(t[i]);
    }
    ++count;
  }

  string print(int pixelCount, bool noAsm = false);
};

// *****************
// *               *
// *  Test Images  *
// *               *
// *****************

class TestImage
{
  void* allocBlock;

  bool random;

public:
  enum
  {
    width = 320,
    height = 240,
  };

  string path;
  YuvLite source;

  TestImage();
  ~TestImage();

  string read(const string& path);

  void synthetic(const FieldHomography& fh);

  void setRandom() { random = true;}

  void next();
};

#endif
