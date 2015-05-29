// ************************************************
// *                                              *
// *  RoboCup Image Acquisition Front End Tester  *
// *                                              *
// ************************************************

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <list>
#include <iostream>
#include <fstream>
#include "main.h"
#include "FrontEnd.h"
#include "fastBlob.h"
#include "edge2015.h"
#include "hough2015.h"

// ************
// *          *
// *  Timing  *
// *          *
// ************

string CodeTimer::print(int pixelCount, bool noAsm)
{
  double ticks[5][2];
  for (int i = 0; i < 2; ++i)
  {
    ticks[0][i] = sT[i] / count;
    ticks[1][i] = sqrt(count * sT2[i] - sT[i] * sT[i]) / count;
    times[i].sort();
    ticks[2][i] = times[i].front();
    ticks[3][i] = times[i].back();
    std::list<int>::const_iterator it = times[i].begin();
    for (int j = 0; j < (count >> 1); ++j, ++it);
    int medianT = *it;
    if ((count & 1) == 0)
      medianT = (medianT + *--it) / 2;
    ticks[4][i] = medianT;
  }

  string s =
    "\n                  ASM                        C++\n"
    "       ticks/pixel   microsecs    ticks/pixel   microsecs\n"
    "---------------------------------------------------------\n";

  const char* statNames[5] = {"Mean", "StDev", "Min", "Max", "Median"};
  for (int j = 0; j < 5; ++j)
  {
    s += strPrintf("%7s", statNames[j]);
    for (int i = 0; i < 2; ++i)
      if (i > 0 || !noAsm)
        s += strPrintf(" %8.2f   %10.1f     ", ticks[j][i] / pixelCount, ticks[j][i] / mhz);
      else
        s += "                           ";
    s += strPrintf("\n");
  }

  return s;
}

// ***************************************
// *                                     *
// *  Read and Write Images for Testing  *
// *                                     *
// ***************************************

// This reads a standard .nblog image and returns the attributes string
TestImage::TestImage()
{
  // Allocate source and output images for the ASM code aligned to 256-byte boundry.
  // This allows the code to use movdqa, and also provides L1 and L2 cache line
  // alignment in an attempt to yield more consistent timing tests. Note that movdqa
  // was for timing tests and found not to be much of an improvement, and it makes
  // the code harder to use on windows (subsets) of images. Note also that
  // aligning the memory to cache line boundaries did not make the timing more
  // consistent.
  source = (uint8_t*)alignedAlloc(width * height * 14, 8, allocBlock);
}

TestImage::~TestImage()
{
  delete[] allocBlock;
}

string TestImage::read(const string& path)
{
  ifstream image;
  image.open(path, ios_base::binary);
  if (!image.is_open())
    throw strPrintf("Failed to open %s", path.c_str());

  this->path = path;
  int n = 0;
  for (int i = 0; i < 4; ++i)
  {
    char c;
    image.get(c);
    n = (n << 8) + (uint8_t)c;
  }
  string s;
  for (int i = 0; i < n; ++i)
  {
    char c;
    image.get(c);
    s += c;
  }

  for (int i = 0; i < 4; ++i)
  {
    char c;
    image.get(c);
    n = (n << 8) + (uint8_t)c;
  }
  for (int i = 0; i < n; ++i)
  {
    char c;
    image.get(c);
    source[i] = (uint8_t)c;
  }

  image.close();
  return s;
}

void TestImage::next()
{
  if (path.length() == 0)
    for (int i = 0; i < width * height * 8; ++i)
      source[i] = (uint8_t)(rand() >> 7);
}

// This writes the output images for display by another program
void writeFile(const string& path, uint8_t* src, int count)
{
  ofstream image;
  image.open(path, ios_base::binary);
  image.write((const char*)src, count);
  image.close();
}

void writeRows(const string& path, uint8_t* src, int width, int height, int pitch)
{
  ofstream image;
  image.open(path, ios_base::binary);
  for (int r = 0; r < height; ++r)
    image.write((const char*)(src + r * pitch), width);
  image.close();
}

// *********************************************
// *                                           *
// *  Objects for Testing, Loaded from main()  *
// *                                           *
// *********************************************

static Colors colors;
static EdgeDetector ed;
static TestImage image;
static HoughSpace hs(TestImage::width, TestImage::height);
static AdjustSet adjustSet;
static FieldHomography fh;

static int iterationCount = 1;
static bool useColorTable = false;
static bool fieldEdges = true;

// ********************
// *                  *
// *  Test Functions  *
// *                  *
// ********************

const char* testImagePath = "C:\\Users\\bill\\Documents\\life\\Bowdoin\\RoboCup\\Vision 2014\\FrontEnd\\test.xxx";

int flushCache()
{
  uint8_t* temp = new uint8_t[0x400000];
  int sum = 0;

  // Flush the cache by reading 4MB, one byte per 32-byte cache line. Do something
  // with the bytes so the compiler doesn't optimize away the entire loop
  for (int i = 0; i < 0x400000; i += 32)
    sum += temp[i];

  delete[] temp;

  return sum;
}

uint8_t* getSourceImage(void*& memBlock, int& size)
{
  size = TestImage::width * TestImage::height;
  uint8_t* source = (uint8_t*)alignedAlloc(size * 6, 4, memBlock);
  newAcquire(image.source, TestImage::width, TestImage::height, TestImage::pitch, &colors,
             source, 0);
  return source;
}

uint8_t* getGradient(void*& memBlock, int& size)
{
  uint8_t* source = getSourceImage(memBlock, size);
  ed.gradient((int16_t*)source, TestImage::width, TestImage::height, TestImage::width);
  return source;
}

void getEdges(EdgeList& edges)
{
  void* memBlock;
  int size;
  uint8_t* source = getGradient(memBlock, size);
  uint8_t* gImg = fieldEdges ? source + 3 * size : 0;
  ed.edgeDetect(gImg, TestImage::width, edges);
  delete[] memBlock;
}

void blobTest()
{
  Connectivity connect;
  string message;
  CodeTimer ct;
  bool ok = true;

  for (int n = 0; n < iterationCount && ok; ++n)
  {
    uint32_t t[2];
    for (int i = 0; i < 2 && ok; ++i)
    {
      connect.fast(i == 0);
      ok = connect.test(message);
      t[i] = connect.ticks();
    }
    ct.add(t);
  }
  cout << message.c_str();
  if (ok)
    cout << connect.print().c_str();
  if (iterationCount > 1)
    cout << ct.print(320 * 240).c_str();
}

void frontTest()
{
  void* allocBlock;
  int size = TestImage::width * TestImage::height;
  uint8_t* dest = (uint8_t*)alignedAlloc(size * 6, 8, allocBlock);
  uint8_t* test = new uint8_t[size * 6];

  // Color lookup table, filled with random values.
  uint8_t* colorTable = 0;
  if (useColorTable)
  {
    colorTable = new uint8_t[0x200000];
    for (int i = 0; i < 0x200000; ++i)
      colorTable[i] = (uint8_t)(rand() >> 7);
  }

  // Count errors (disagreements between C++ and ASM)
  int64_t errors[5];
  const char* errorNames[5] = {"Y", "White", "Green", "Orange", "Table"};
  for (int i = 0; i < 5; ++i)
    errors[i] = 0;

  // ASM execution time stats. Times are in clock ticks.
  CodeTimer ct;

  // Run the code lots of times in an attempt to get more consistent timing data. Also,
  // a more rigorous test when using random source pixel values.
  for (int n = 0; n < iterationCount; ++n)
  {
    image.next();

    flushCache();

    // Generate ASM and C++ results
    uint32_t t[2];
    t[0] = newAcquire (image.source, TestImage::width, TestImage::height, TestImage::pitch, &colors, dest, colorTable);
    t[1] = testAcquire(image.source, TestImage::width, TestImage::height, TestImage::pitch, &colors, test, colorTable);

    // Keep ASM timing stats
    ct.add(t);

    // Count errors
    int i = 0;
    for (int e = 0; e < 5; ++e)
      for (; i < size * (e + 2); ++i)
        errors[e] += (int)(dest[i] != test[i]);
  }

  // Print error counts
  printf("\n");
  for (int e = 0; e < 5; ++e)
    printf("%8s %10lld errors\n", errorNames[e], errors[e]);

  // Print timing stats
  if (iterationCount > 1)
    cout << ct.print(size).c_str();

  // Save output for later display
  writeFile(testImagePath, dest, size * 5);

  delete[] colorTable;
  delete[] test;
  delete[] allocBlock;
}

void gradientTest()
{
  EdgeDetector slowEd(ed);
  slowEd.fast(false);

  // Get source image
  void* allocBlock;
  int size;
  uint8_t* source = getSourceImage(allocBlock, size);

  // ASM execution time stats. Times are in clock ticks.
  CodeTimer ct;

  int64_t magErrors = 0, dirErrors = 0;

  // Run the code lots of times in an attempt to get more consistent timing data.
  for (int n = 0; n < iterationCount; ++n)
  {
    image.next();

    flushCache();

    // Generate ASM and C++ results
    uint32_t t[2];
    t[0] = ed    .gradient((int16_t*)source, TestImage::width, TestImage::height, TestImage::width);
    t[1] = slowEd.gradient((int16_t*)source, TestImage::width, TestImage::height, TestImage::width);

    // Keep ASM timing stats
    ct.add(t);

    // Count errors
    for (int y = 0; y < ed.dstHeight(); ++y)
      for (int x = 0; x < ed.dstWidth(); ++x)
      {
        magErrors += ed.mag(x, y) != slowEd.mag(x, y);
        dirErrors += ed.dir(x, y) != slowEd.dir(x, y);
      }
  }

  printf("\nGradient image %d x %d, pitch = %d\n", ed.dstWidth(), ed.dstHeight(), ed.dstPitch());

  // Print error counts
  printf("\n%lld mag errors, %lld direction errors\n", magErrors, dirErrors);

  // Print timing stats
  if (iterationCount > 1)
    cout << ct.print(size).c_str();

  // Save output for later display
  writeFile(testImagePath, (uint8_t*)ed.gradientImage(), ed.dstPitch() * ed.dstHeight() * 2);

  delete[] allocBlock;
}

void edgeTest()
{
  // Get source image and gradient
  void* allocBlock;
  int size;
  uint8_t* source = getGradient(allocBlock, size);
  uint8_t* gImg = fieldEdges ? source + 3 * size : 0;

  // ASM execution time stats. Times are in clock ticks.
  CodeTimer ct;

  EdgeList slowEdges(32000);
  EdgeList fastEdges(32000);

  long long errors = 0;

  // Run the code lots of times in an attempt to get more consistent timing data.
  for (int n = 0; n < iterationCount; ++n)
  {
    image.next();

    // Generate ASM and C++ results
    uint32_t t[2];
    ed.fast(true);
    t[0] = ed.edgeDetect(gImg, TestImage::width, fastEdges);
    ed.fast(false);
    t[1] = ed.edgeDetect(gImg, TestImage::width, slowEdges);

    AngleBinsIterator<Edge> abf(fastEdges), abs(slowEdges);
    while (*abf || *abs)
    {
      Edge* fe = *abf;
      Edge* se = *abs;
      if (!fe || !se)
        ++errors;
      else if (fe->x() != se->x() || fe->y() != se->y() ||
               fe->mag() != se->mag() || fe->angle() != se->angle())
        ++errors;
      ++abf;
      ++abs;
    }

    // Keep ASM timing stats
    ct.add(t);
  }

  ed.fast(true);

  printf("\n%d fast edges, %d slow edges, %lld errors\n", fastEdges.count(), slowEdges.count(), errors);

  // Print timing stats
  if (iterationCount > 1)
    cout << ct.print(size).c_str();

  // Save output for later display
  struct SimpleEdge
  {
    int x, y;
    int mag, dir;
  }
  *edgeArray = new SimpleEdge[fastEdges.count()];
  AngleBinsIterator<Edge> abi(fastEdges);
  int i = 0;
  for (Edge* e = *abi; e; e = *++abi, ++i)
  {
    edgeArray[i].x = e->x();
    edgeArray[i].y = e->y();
    edgeArray[i].mag = e->mag();
    edgeArray[i].dir = e->angle();
  }

  writeFile(testImagePath, (uint8_t*)edgeArray, fastEdges.count() * sizeof(SimpleEdge));
  delete[] edgeArray;

  delete[] allocBlock;
}

void houghTest()
{
  EdgeList edges(24000);
  getEdges(edges);

  CodeTimer ct[HoughSpace::NumTimes];

  HoughLineList fastList(128), slowList(128);
  //hs.adjustSteps(0);

  for (int n = 0; n < iterationCount; ++n)
  {
    image.next();

    // Generate ASM and C++ results
    uint32_t t[HoughSpace::NumTimes][2];
    hs.fast(false);
    hs.run(edges, slowList);
    for (int i = 0; i < HoughSpace::NumTimes; ++i)
      t[i][1] = hs.time(i);

    hs.fast(true);
    hs.run(edges, fastList);
    for (int i = 0; i < HoughSpace::NumTimes; ++i)
      t[i][0] = hs.time(i);

    // Keep ASM timing stats
    for (int i = 0; i < HoughSpace::NumTimes; ++i)
      ct[i].add(t[i]);
  }

  printf("\n%d edges\n", edges.count());

  printf("\n%d fast lines:\n", fastList.size());
  for (list<HoughLine>::iterator hl = fastList.begin(); hl != fastList.end(); ++hl)
    printf("  %s\n", hl->print().c_str());

  printf("\n%d slow lines:\n", slowList.size());
  for (list<HoughLine>::iterator hl = slowList.begin(); hl != slowList.end(); ++hl)
    printf("  %s\n", hl->print().c_str());

  if (iterationCount > 1)
  {
    static bool noAsm[HoughSpace::NumTimes] = { true, true, false, false, true, false };
    for (int i = 0; i < HoughSpace::NumTimes; ++i)
      printf("\n%s:%s", HoughSpace::timeNames[i],
             ct[i].print(TestImage::width * TestImage::height, noAsm[i]).c_str());
  }

  fastList.mapToField(fh);

  ofstream f;
  f.open(testImagePath, ios_base::binary);

  string s = strPrintf("%d,", fastList.size());
  for (list<HoughLine>::iterator hl = fastList.begin(); hl != fastList.end(); ++hl)
  {
    s += hl->GeoLine::print();
    s += hl->field().print();
  }
  f.write(s.c_str(), s.length() + 1);

  const char* p = (const char*)(hs.pSpace(-hs.rRadius(), 0));
  for (int t = 0; t < 256; ++t)
    f.write(p + 2 * t * hs.rPitch(), 2 * hs.rWidth());

  f.close();
}

// ******************
// *                *
// *  Main Program  *
// *                *
// ******************

int intArg(int& argIndex, int argc, char* argv[])
{
  if (argIndex < argc)
  {
    int n;
    if (sscanf_s(argv[argIndex], "%d", &n) != 1)
      throw strPrintf("Bad integer %s", argv[argIndex]);
    ++argIndex;
    return n;
  }
  throw string("No more arguments while looking for integer");
}

float floatArg(int& argIndex, int argc, char* argv[])
{
  if (argIndex < argc)
  {
    float n;
    if (sscanf_s(argv[argIndex], "%f", &n) != 1)
      throw strPrintf("Bad float %s", argv[argIndex]);
    ++argIndex;
    return n;
  }
  throw string("No more arguments while looking for float");
}

int main(int argc, char* argv[])
{
  // Here are some parameters found to be useful.
  colors.white .load(-0.04f, -0.04f,  0.12f,  0.12f, -0.06f, -0.06f);
  colors.green .load( 0.08f,  0.01f, -0.02f, -0.23f, -0.06f, -0.06f);
  colors.orange.load( 0.13f,  0.05f, -0.13f,  0.11f, -0.06f,  0.06f);

  try
  {
    for (int argIndex = 1; argIndex < argc;)
    {
      int cmd = 0;
      for (int i = 0; i < 4; ++i)
      {
        int c = tolower(argv[argIndex][i]);
        if (c == 0)
          break;
        cmd = (cmd << 8) | c;
      }

      ++argIndex;

      switch (cmd)
      {
      case 'img':
        if (argIndex < argc)
        {
          string imageAttributes = image.read(argv[argIndex++]);
          printf("\n%s\n", imageAttributes.c_str());
        }
        else
          throw string("No more arguments while looking for image filename");
        break;

      case 'rand':
        image.setRandom();
        break;

      case 'ic':
        iterationCount = intArg(argIndex, argc, argv);
        break;

        // Colors
      case 'orn':
      case 'grn':
      case 'wht':
        if (argIndex < argc)
        {
          float darkU, darkV, lightU, lightV, fuzzyU, fuzzyV;
          if (sscanf_s(argv[argIndex], "%f,%f,%f,%f,%f,%f", &darkU, &darkV, &lightU, &lightV, &fuzzyU, &fuzzyV) == 6)
          {
            switch (cmd)
            {
            case 'orn':
              colors.orange.load(darkU, darkV, lightU, lightV, fuzzyU, fuzzyV);
              break;
            case 'grn':
              colors.green.load(darkU, darkV, lightU, lightV, fuzzyU, fuzzyV);
              break;
            case 'wht':
              colors.white.load(darkU, darkV, lightU, lightV, fuzzyU, fuzzyV);
              break;
            }
            ++argIndex;
          }
          else
            throw strPrintf("Bad color parameters %s", argv[argIndex]);
        }
        else
          throw string("No more arguments while looking for color values");
        break;

      case 'ctab':
        useColorTable = intArg(argIndex, argc, argv) != 0;
        break;

        // Gradient parameters
      case 'grth':
        ed.gradientThreshold(intArg(argIndex, argc, argv));
        break;

        // Edge detection parameters
      case 'dirc':
        ed.correctEdgeDirection(intArg(argIndex, argc, argv) != 0);
        break;

      case 'fedg':
        fieldEdges = intArg(argIndex, argc, argv) != 0;
        break;

      case 'edth':
        ed.edgeThreshold(intArg(argIndex, argc, argv));
        break;

      case 'gnth':
        ed.greenThreshold(intArg(argIndex, argc, argv));
        break;

        // Hough parameters
      case 'hthr':
        hs.acceptThreshold(intArg(argIndex, argc, argv));
        break;

      case 'endw':
        adjustSet.params[1].lineEndWeight = floatArg(argIndex, argc, argv);
        hs.adjustSet(adjustSet);
        break;

      case 'fitt':
        adjustSet.params[1].fitThresold = floatArg(argIndex, argc, argv);
        hs.adjustSet(adjustSet);
        break;

      case 'adjs':
        hs.adjustSteps(intArg(argIndex, argc, argv));
        break;

        // Homography
      case 'tilt':
        fh.tilt(floatArg(argIndex, argc, argv) * (M_PI / 180));
        break;

      case 'roll':
        fh.roll(floatArg(argIndex, argc, argv) * (M_PI / 180));
        break;

      case 'az':
        fh.azimuth(floatArg(argIndex, argc, argv) * (M_PI / 180));
        break;

      case 'hght':
        fh.wz0(floatArg(argIndex, argc, argv));
        break;

        // Test functions
      case 'blob':
        blobTest();
        break;

      case 'fron':
        frontTest();
        break;

      case 'grad':
        gradientTest();
        break;

      case 'edge':
        edgeTest();
        break;

      case 'ht':
        houghTest();
        break;

      default:
        throw strPrintf("Unknown command %s\n", argv[argIndex - 1]);
      }
    }
  }
  catch (string& s)
  {
    printf("%s\n", s.c_str());
    return 1;
  }

	return 0;
}
