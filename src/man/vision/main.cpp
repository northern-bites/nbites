// ************************************************
// *                                              *
// *  RoboCup Image Acquisition Front End Tester  *
// *                                              *
// ************************************************

#include "Stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <list>
#include <iostream>
#include <fstream>
#include "main.h"
#include "FrontEnd.h"
#include "FastBlob.h"
#include "Edge.h"
#include "Hough.h"

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
  // Allocate space for Yuv image. On the robot this comes from a camera.
  uint8_t* src = (uint8_t*)
    alignedAlloc(TestImage::width * TestImage::height * 8, 4, allocBlock);
  source = YuvLite(TestImage::width, TestImage::height, TestImage::width * 4, src);
  setRandom();
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
    source.pixelAddr()[i] = (uint8_t)c;
  }

  image.close();
  random = false;
  return s;
}

void TestImage::synthetic(const FieldHomography& fh)
{
  syntheticField(source, fh);
  random = false;
}

void TestImage::next()
{
  if (random)
    for (int i = 0; i < width * height * 8; ++i)
      source.pixelAddr()[i] = (uint8_t)(rand() >> 7);
}

// This writes the output images for display by another program
void writeFile(const string& path, uint8_t* src, int count)
{
  ofstream image;
  image.open(path, ios_base::binary);
  image.write((const char*)src, count);
  image.close();
}

template <class T>
void writeImage(const ImageLite<T>& image, ofstream& file)
{
  for (int r = 0; r < image.height(); ++r)
    file.write((const char*)image.pixelAddr(0, r), image.width() * sizeof(T));
}

template <class T>
void writeImage(const ImageLite<T>& image, const string& path)
{
  ofstream file;
  file.open(path, ios_base::binary);
  writeImage(image, file);
  file.close();
}

void writeNum(ofstream& f, uint32_t n)
{
  for (int i = 3; i >= 0; --i)
    f.put((char)((n >> (8 * i)) & 0xFF));
}

void writeYuv(const YuvLite& image, const string& path)
{
  ofstream f;
  f.open(path, ios_base::binary);

  int32_t size = 8 * image.width() * image.height();
  int sum = 0;
  for (int i = 0; i < size; ++i)
    sum += image.pixelAddr()[i];
  string s = strPrintf("(nblog (version 6) (checksum %d) (contents ((type YUVImage) (from camera_TOP) (nbytes %d) (width %d) (height %d) (encoding \"[Y8(U8/V8)]\"))))",
                       sum, size, 2 * image.width(), 2 * image.height());
  writeNum(f, s.length());
  f.write(s.c_str(), s.length());
  writeNum(f, size);
  f.write((const char*)image.pixelAddr(), size);

  f.close();
}

// *********************************************
// *                                           *
// *  Objects for Testing, Loaded from main()  *
// *                                           *
// *********************************************

static Colors colors;
static ImageFrontEnd frontEnd;
static EdgeDetector ed;
static TestImage image;
static HoughSpace hs(TestImage::width, TestImage::height);
static AdjustSet adjustSet;
static FieldHomography fh;

static int iterationCount = 1;
static bool useColorTable = false;
static uint8_t* colorTable = 0;
static bool fieldEdges = true;

// window
static int winX0 = 0, winY0 = 0, winWd = TestImage::width, winHt = TestImage::height;

// *****************************
// *                           *
// *  Vision Helper Functions  *
// *                           *
// *****************************

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

void getSourceImage(ImageFrontEnd& ife = frontEnd)
{
  YuvLite source(image.source, winX0, winY0, winWd, winHt);
  ife.run(source, &colors, useColorTable ? colorTable : 0);
}

void getGradient()
{
  getSourceImage();
  ed.gradient(frontEnd.yImage());
}

void getEdges(EdgeList& edges)
{
  getGradient();
  ImageLiteU8 green;
  if (fieldEdges)
    green = frontEnd.greenImage();
  ed.edgeDetect(green, edges);
}

// ********************
// *                  *
// *  Test Functions  *
// *                  *
// ********************


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
  ImageFrontEnd slow;
  slow.fast(false);

  // Count errors (disagreements between C++ and ASM)
  int64_t errors[5];
  const char* errorNames[5] = {"Y", "White", "Green", "Orange", "Table"};
  for (int i = 0; i < 5; ++i)
    errors[i] = 0;
  int maxErrorIndex = useColorTable ? 5 : 4;

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
    getSourceImage();
    t[0] = frontEnd.time();
    getSourceImage(slow);
    t[1] = slow.time();

    // Keep ASM timing stats
    ct.add(t);

    // Count errors
    ImageLiteU16 yFast = frontEnd.yImage();
    ImageLiteU16 ySlow = slow.yImage();
    ImageLiteU8 wFast = frontEnd.whiteImage();
    ImageLiteU8 wSlow = slow.whiteImage();
    for (int y = 0; y < yFast.height(); ++y)
      for (int x = 0; x < yFast.width(); ++x)
      {
        errors[0] += (int)(*yFast.pixelAddr(x, y) != *ySlow.pixelAddr(x, y));
        for (int e = 1; e < maxErrorIndex; ++e)
        {
          int y2 = y + (e - 1) * wFast.height();
          errors[e] += (int)(*wFast.pixelAddr(x, y2) != *wSlow.pixelAddr(x, y2));
        }
      }
  }

  // Print error counts
  printf("\n");
  for (int e = 0; e < maxErrorIndex; ++e)
    printf("%8s %10lld errors\n", errorNames[e], errors[e]);

  // Print timing stats
  if (iterationCount > 1)
    cout << ct.print(frontEnd.yImage().width() * frontEnd.yImage().height()).c_str();
}

void gradientTest()
{
  EdgeDetector slowEd(ed);
  slowEd.fast(false);

  // Get source image
  getSourceImage();
  ImageLiteU16 yImage = frontEnd.yImage();

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
    t[0] = ed    .gradient(yImage);
    t[1] = slowEd.gradient(yImage);

    // Keep ASM timing stats
    ct.add(t);

    // Count errors
    for (int y = 0; y < ed.gradientImage().height(); ++y)
      for (int x = 0; x < ed.gradientImage().width(); ++x)
      {
        magErrors += ed.mag(x, y) != slowEd.mag(x, y);
        dirErrors += ed.dir(x, y) != slowEd.dir(x, y);
      }
  }

  ImageLiteU16 grad = ed.gradientImage();
  printf("\nGradient image %d x %d, pitch = %d, src pitch = %d\n",
         grad.width(), grad.height(), grad.pitch(), yImage.pitch());

  // Print error counts
  printf("\n%lld mag errors, %lld direction errors\n", magErrors, dirErrors);

  // Print timing stats
  if (iterationCount > 1)
    cout << ct.print(yImage.width() * yImage.height()).c_str();
}

void edgeTest()
{
  // Get source image and gradient
  getGradient();
  ImageLiteU8 green;
  if (fieldEdges)
    green = frontEnd.greenImage();

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
    t[0] = ed.edgeDetect(green, fastEdges);
    ed.fast(false);
    t[1] = ed.edgeDetect(green, slowEdges);

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
    cout << ct.print(ed.gradientImage().width() * ed.gradientImage().height()).c_str();
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
  fastList.mapToField(fh);
  for (list<HoughLine>::iterator hl = fastList.begin(); hl != fastList.end(); ++hl)
    printf("  %s\n", hl->print().c_str());

  //printf("\n%d slow lines:\n", slowList.size());
  //for (list<HoughLine>::iterator hl = slowList.begin(); hl != slowList.end(); ++hl)
  //  printf("  %s\n", hl->print().c_str());

  if (iterationCount > 1)
  {
    static bool noAsm[HoughSpace::NumTimes] = { true, true, false, false, true, false };
    for (int i = 0; i < HoughSpace::NumTimes; ++i)
      printf("\n%s:%s", HoughSpace::timeNames[i],
             ct[i].print(TestImage::width * TestImage::height, noAsm[i]).c_str());
  }
}

// ***********************************
// *                                 *
// *  Get Processing Results for C#  *
// *                                 *
// ***********************************

const char* testImagePath = "C:\\Users\\bill\\Documents\\life\\Bowdoin\\RoboCup\\Vision 2014\\FrontEnd\\test.xxx";

void runColors()
{
  getSourceImage();

  ofstream file;
  file.open(testImagePath, ios_base::binary);
  writeImage(frontEnd.     yImage(), file);
  writeImage(frontEnd. whiteImage(), file);
  writeImage(frontEnd. greenImage(), file);
  writeImage(frontEnd.orangeImage(), file);
  file.close();
}

void runGradient()
{
  getGradient();
  writeImage(ed.gradientImage(), testImagePath);
}

void runEdges()
{
  EdgeList edges(24000);
  getEdges(edges);

  struct SimpleEdge
  {
    int x, y;
    int mag, dir;
  }
  *edgeArray = new SimpleEdge[edges.count()];
  AngleBinsIterator<Edge> abi(edges);
  int i = 0;
  for (Edge* e = *abi; e; e = *++abi, ++i)
  {
    edgeArray[i].x = e->x();
    edgeArray[i].y = e->y();
    edgeArray[i].mag = e->mag();
    edgeArray[i].dir = e->angle();
  }

  writeFile(testImagePath, (uint8_t*)edgeArray, edges.count() * sizeof(SimpleEdge));
  delete[] edgeArray;
}

void runHough()
{
  EdgeList edges(24000);
  getEdges(edges);

  HoughLineList lines(128);
  hs.run(edges, lines);
  lines.mapToField(fh);

  FieldLineList fLines;
  fLines.find(lines);

  string cal;
  fLines.tiltCalibrate(fh, &cal);
  cout << cal.c_str();

  string s = "((lines";
  for (list<HoughLine>::iterator hl = lines.begin(); hl != lines.end(); ++hl)
  {
    int fieldLineSide = -1;
    if (hl->fieldLine() >= 0)
      fieldLineSide = (int)(&fLines.at(hl->fieldLine())[1] == &*hl);
    s += strPrintf(" ((rt %d %d) (score %.2f) (fit %.3f) (fIndex %d %d) (image %s) (field %s))",
                   hl->rIndex(), hl->tIndex(),
                   hl->score(), hl->fitError(),
                   hl->fieldLine(), fieldLineSide,
                   hl->GeoLine::print().c_str(), hl->field().print().c_str());
  }

  const double ticksPerUs = 1600;
  s += strPrintf(") (times %.1f %.1f %.1f %.1f))",
                 frontEnd.time() / ticksPerUs, ed.gradientTime() / ticksPerUs,
                 ed.edgeTime() / ticksPerUs, hs.time(HoughSpace::NumTimes - 1) / ticksPerUs);

  ofstream f;
  f.open(testImagePath, ios_base::binary);
  f.write(s.c_str(), s.length() + 1);

  ImageLiteU16 hSpace(hs.rWidth(), 256, hs.rPitch(), (uint16_t*)hs.pSpace(-hs.rRadius(), 0));
  writeImage(hSpace, f);

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

string stringArg(int& argIndex, int argc, char* argv[])
{
  if (argIndex < argc)
    return argv[argIndex++];
  throw string("No more arguments while looking for string");
}

int main(int argc, char* argv[])
{
  // Here are some parameters found to be useful.
  colors.white .load(-0.04f, -0.04f,  0.12f,  0.12f, -0.06f, -0.06f);
  colors.green .load( 0.08f,  0.01f, -0.02f, -0.23f, -0.06f, -0.06f);
  colors.orange.load( 0.13f,  0.05f, -0.13f,  0.11f, -0.06f,  0.06f);

  // Color lookup table, filled with random values.
  colorTable = new uint8_t[0x200000];
  for (int i = 0; i < 0x200000; ++i)
    colorTable[i] = (uint8_t)(rand() >> 7);

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
        {
          string imageAttributes = image.read(stringArg(argIndex, argc, argv));
          //printf("\n%s\n", imageAttributes.c_str());
        }
        break;

      case 'rand':
        image.setRandom();
        break;

      case 'syn':
        image.synthetic(fh);
        writeYuv(image.source, "syn.nblog");
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
        adjustSet.params[1].fitThreshold = floatArg(argIndex, argc, argv);
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

      case 'x0':
        fh.wx0(floatArg(argIndex, argc, argv));
        break;

      case 'y0':
        fh.wy0(floatArg(argIndex, argc, argv));
        break;

      case 'z0':
      case 'hght':
        fh.wz0(floatArg(argIndex, argc, argv));
        break;

      case 'flen':
        fh.flen(floatArg(argIndex, argc, argv));
        break;

        // Window
      case 'wind':
        int x0, y0, wd, ht;
        if (sscanf_s(argv[argIndex], "%d,%d,%d,%d", &x0, &y0, &wd, &ht) == 4)
        {
          winX0 = max(x0, 0);
          winY0 = max(y0, 0);
          winWd = min(x0 + wd, TestImage::width) - winX0;
          winHt = min(y0 + ht, TestImage::height) - winY0;
          ++argIndex;
        }
        else
          throw strPrintf("Bad window parameters %s", argv[argIndex]); 
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

        // Run functions
      case 'rcl':
        runColors();
        break;

      case 'rgr':
        runGradient();
        break;

      case 'red':
        runEdges();
        break;

      case 'rht':
        runHough();
        break;

      case 'cal':
        {
          EdgeList edges(24000);
          getEdges(edges);
          HoughLineList lines(12);
          hs.run(edges, lines);
          lines.mapToField(fh);
          FieldLineList fLines;
          fLines.find(lines);

          if (fh.calibrateFromStar(fLines))
            printf("Roll = %.2f, tilt = %.2f\n", fh.roll() * (180 / M_PI), fh.tilt() * (180 / M_PI));
          else
            printf("Can't find 3 legs of star target\n");
        }
        break;

      case 'star':
        {
          StarCal sc(fh);
          int n = intArg(argIndex, argc, argv);
          EdgeList edges(24000);
          HoughLineList lines(12);
          FieldLineList fLines;
          for (int i = 0; i < n; ++i)
          {
            image.read(stringArg(argIndex, argc, argv));
            getEdges(edges);
            hs.run(edges, lines);
            lines.mapToField(fh);
            fLines.find(lines);
            sc.add(fLines);
          }
          printf("Count = %d, roll = %.2f, tilt = %.2f\n",
                 sc.count(), sc.roll() * (180 / M_PI), sc.tilt() * (180 / M_PI));
        }
        break;

      case 'hrzn':
        printf("\n%s\n", fh.horizon(320).print(true).c_str());
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
