/**
* @file FieldDimensionsBH.cpp
*
* Some useful functions regarding field dimensions.
*
* @author Max Risler
*/

#include "FieldDimensions.h"
#include "Tools/Debugging/Modify.h"
#include "Tools/Streams/InStreams.h"
#include "Platform/BHAssert.h"
#include "Tools/Math/Geometry.h"
#include "Representations/Infrastructure/RoboCupGameControlData.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <limits>

using namespace std;

/**
 * Helper class that supports the use of symbolic values in float fields.
 */
class InSymbolicMapFile : public InMapFile
{
private:
  std::unordered_map<std::string, float> values; /**< All symbolic values known. */
  const char* entry; /**< The name of the current entry processed. */

protected:
  /**
   * When reading a float, read a string instead. Try to replace symbolic value.
   * Symbolic value can be preceeded by a minus sign (without whitespace in between).
   */
  virtual void inFloat(float& value)
  {
    std::string buf;
    inString(buf);
    float sign = 1.f;
    if(buf[0] == '-')
    {
      sign = -1.f;
      buf = buf.substr(1);
    }

    std::unordered_map<std::string, float>::const_iterator i = values.find(buf);
    if(i != values.end())
      value = i->second * sign;
    else if(!buf.empty() && (isdigit(buf[0]) || buf[0] == '.'))
      value = (float) strtod(buf.c_str(), 0) * sign;
    else
      OUTPUT_ERROR("fieldDimensions.cfg: Unknown symbol '" << buf << "'");

    if(entry)
      values[entry] = value;
  }

public:
  InSymbolicMapFile(const std::string& name) : InMapFile(name), entry(0) {}

  virtual void select(const char* name, int type, const char * (*enumToString)(int))
  {
    Streaming::trimName(name);
    InMapFile::select(name, type, enumToString);
    entry = name;
  }
};

void FieldDimensionsBH::load()
{
  InSymbolicMapFile stream("fieldDimensions.cfg");
  ASSERT(stream.exists());
  stream >> *this;
  add(Vector2BH<>(xPosOpponentFieldBorder, yPosLeftFieldBorder));
  add(Vector2BH<>(xPosOwnFieldBorder, yPosRightFieldBorder));
}

Pose2DBH FieldDimensionsBH::randomPoseOnField() const
{
  Pose2DBH pose;
  do
    pose = Pose2DBH::random(x, y, RangeBH<>(-pi, pi));
  while(!isInsideField(pose.translation));
  return pose;
}

Pose2DBH FieldDimensionsBH::randomPoseOnCarpet() const
{
  Pose2DBH pose;
  do
    pose = Pose2DBH::random(x, y, RangeBH<>(-pi, pi));
  while(!isInsideCarpet(pose.translation));
  return pose;
}

void FieldDimensionsBH::draw() const
{
  drawLines();
  drawCorners();
}

void FieldDimensionsBH::drawLines() const
{
  DECLARE_DEBUG_DRAWING("field lines", "drawingOnField");
  COMPLEX_DRAWING("field lines",
  {
    ASSERT(carpetBorder.lines.size() <= 4);
    Vector2BH<> points[4];
    for(unsigned i = 0; i < carpetBorder.lines.size(); ++i)
      points[i] = carpetBorder.lines[i].corner.translation;
    POLYGON("field lines", (int) carpetBorder.lines.size(), points, 0, Drawings::ps_solid, ColorRGBA(0, 180, 0), Drawings::bs_solid, ColorRGBA(0, 140, 0));

    ColorRGBA lineColor(192, 192, 192);
    for(vector<LinesTable::Line>::const_iterator i = fieldLines.lines.begin(); i != fieldLines.lines.end(); ++i)
    {
      Vector2BH<> source = i->corner.translation;
      Pose2DBH target(i->corner);
      target.translate(i->length, 0);
      LINE("field lines", source.x, source.y, target.translation.x, target.translation.y, fieldLinesWidth, Drawings::ps_solid, lineColor);
    }
  });
}

void FieldDimensionsBH::drawPolygons(uint32 ownColor) const
{
  DECLARE_DEBUG_DRAWING("field polygons", "drawingOnField");
  COMPLEX_DRAWING("field polygons",
  {
    ColorRGBA own = ownColor == TEAM_BLUE ? ColorRGBA(50, 120, 127) : ColorRGBA(127, 50, 50);
    ColorRGBA opp = ownColor != TEAM_BLUE ? ColorRGBA(50, 120, 127) : ColorRGBA(127, 50, 50);

    Vector2BH<> goal[4];
    goal[0] = Vector2BH<>(xPosOwnGroundline - fieldLinesWidth * 0.5f, yPosLeftGoal);
    goal[1] = Vector2BH<>(xPosOwnGroundline - fieldLinesWidth * 0.5f, yPosRightGoal);
    goal[2] = Vector2BH<>(xPosOwnGoal, yPosRightGoal);
    goal[3] = Vector2BH<>(xPosOwnGoal, yPosLeftGoal);
    POLYGON("field polygons", 4, goal, 0, Drawings::ps_solid, own, Drawings::bs_solid, own);

    goal[0] = Vector2BH<>(xPosOpponentGroundline + fieldLinesWidth * 0.5f, yPosLeftGoal);
    goal[1] = Vector2BH<>(xPosOpponentGroundline + fieldLinesWidth * 0.5f, yPosRightGoal);
    goal[2] = Vector2BH<>(xPosOpponentGoal, yPosRightGoal);
    goal[3] = Vector2BH<>(xPosOpponentGoal, yPosLeftGoal);
    POLYGON("field polygons", 4, goal, 0, Drawings::ps_solid, opp, Drawings::bs_solid, opp);

    CIRCLE("field polygons", xPosOpponentGoalPost, yPosLeftGoal, 50, 0, Drawings::ps_solid,
    ColorRGBA(ColorClasses::yellow), Drawings::bs_solid, ColorRGBA(ColorClasses::yellow));
    CIRCLE("field polygons", xPosOpponentGoalPost, yPosRightGoal, 50, 0, Drawings::ps_solid,
    ColorRGBA(ColorClasses::yellow), Drawings::bs_solid, ColorRGBA(ColorClasses::yellow));

    CIRCLE("field polygons", xPosOwnGoalPost, yPosLeftGoal, 50, 0, Drawings::ps_solid,
    ColorRGBA(ColorClasses::yellow), Drawings::bs_solid, ColorRGBA(ColorClasses::yellow));
    CIRCLE("field polygons", xPosOwnGoalPost, yPosRightGoal, 50, 0, Drawings::ps_solid,
    ColorRGBA(ColorClasses::yellow), Drawings::bs_solid, ColorRGBA(ColorClasses::yellow));
  });
}

void FieldDimensionsBH::drawCorners() const
{
  DECLARE_DEBUG_DRAWING("field corners", "drawingOnField");
#ifndef RELEASE
  CornerClass c = xCorner;
#endif
  MODIFY_ENUM("fieldDimensions:cornerClass", c);
  COMPLEX_DRAWING("field corners",
  {
    for(CornersTable::const_iterator i = corners[c].begin(); i != corners[c].end(); ++i)
      LARGE_DOT("field corners", i->x, i->y, ColorRGBA(255, 255, 255), ColorRGBA(255, 255, 255));
  });
}

void FieldDimensionsBH::LinesTable::push(const Pose2DBH& p, float l, bool isPartOfCircle)
{
  LinesTable::Line line;
  line.corner = p;
  line.length = l;
  line.isPartOfCircle = isPartOfCircle;
  lines.push_back(line);
}

void FieldDimensionsBH::LinesTable::push(const Vector2BH<>& s, const Vector2BH<>& e, bool isPartOfCircle)
{
  Vector2BH<> d = e - s;
  push(Pose2DBH(d.angle(), s), d.abs(), isPartOfCircle);
}

void FieldDimensionsBH::LinesTable::pushCircle(const Vector2BH<>& center, float radius, int numOfSegments)
{
  Vector2BH<> p1, p2;
  for(float a = 0; a <= pi_4; a += pi2 / numOfSegments)
  {
    p1 = Vector2BH<>(sin(a), cos(a)) * radius;
    if(a > 0)
    {
      push(center + p1, center + p2, true);
      push(center + Vector2BH<>(p1.x, -p1.y), center + Vector2BH<>(p2.x, -p2.y), true);
      push(center + Vector2BH<>(-p1.x, p1.y), center + Vector2BH<>(-p2.x, p2.y), true);
      push(center - p1, center - p2, true);
      push(center + Vector2BH<>(p1.y, p1.x), center + Vector2BH<>(p2.y, p2.x), true);
      push(center + Vector2BH<>(p1.y, -p1.x), center + Vector2BH<>(p2.y, -p2.x), true);
      push(center + Vector2BH<>(-p1.y, p1.x), center + Vector2BH<>(-p2.y, p2.x), true);
      push(center + Vector2BH<>(-p1.y, -p1.x), center + Vector2BH<>(-p2.y, -p2.x), true);
    }
    p2 = p1;
  }
}

bool FieldDimensionsBH::LinesTable::isInside(const Vector2BH<>& v) const
{
  //note:
  //This function assumes that the point (0,0) is inside and
  //that for any point inside the area the line to (0,0) belongs to the area too.

  Geometry::Line testLine(v, -v);
  for(vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    float factor;
    Geometry::Line border(i->corner, i->length);
    if(Geometry::getIntersectionOfRaysFactor(border, testLine, factor))
      return false;
  }
  return true;
}

float FieldDimensionsBH::LinesTable::clip(Vector2BH<>& v) const
{
  if(isInside(v))
    return 0;
  else
  {
    Vector2BH<> old = v,
              v2;
    float minDist = 100000;
    for(vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    {
      Vector2BH<> diff = (Pose2DBH(old) - i->corner).translation;
      if(diff.x < 0)
        v2 = i->corner.translation;

      else if(diff.x > i->length)
        v2 = (i->corner + Pose2DBH(Vector2BH<>(i->length, 0))).translation;
      else
        v2 = (i->corner + Pose2DBH(Vector2BH<>(diff.x, 0))).translation;
      float dist = (old - v2).abs();
      if(minDist > dist)
      {
        minDist = dist;
        v = v2;
      }
    }
    return (v - old).abs();
  }
}

bool FieldDimensionsBH::LinesTable::getClosestPoint(Vector2BH<>& vMin, const Pose2DBH& p, int numberOfRotations, float minLength) const
{
  int trueNumberOfRotations = numberOfRotations;
  if(numberOfRotations == 2)
    numberOfRotations = 4;

  // target angle -> target index
  float r = p.rotation / pi2 * numberOfRotations + 0.5f;
  if(r < 0)
    r += numberOfRotations;
  int targetRot = int(r);
  ASSERT(targetRot >= 0 && targetRot < numberOfRotations);
  targetRot %= trueNumberOfRotations;
  Vector2BH<> v2;
  float minDist = 100000;
  for(vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->length >= minLength)
    {
      // angle -> index
      float r = (i->corner.rotation + pi_2) / pi2 * numberOfRotations + 0.5f;
      if(r < 0)
        r += numberOfRotations;
      else if(r >= numberOfRotations)
        r -= numberOfRotations;
      int rot = int(r);
      ASSERT(rot >= 0 && rot < numberOfRotations);
      rot %= trueNumberOfRotations;

      // index must be target index
      if(rot == targetRot)
      {
        Vector2BH<> diff = (p - i->corner).translation;
        if(diff.x < 0)
          v2 = i->corner.translation;
        else if(diff.x > i->length)
          v2 = (i->corner + Pose2DBH(Vector2BH<>(i->length, 0))).translation;
        else
          v2 = (i->corner + Pose2DBH(Vector2BH<>(diff.x, 0))).translation;
        Vector2BH<> vDiff = v2 - p.translation;
        float dist = vDiff.abs();
        if(minDist > dist)
        {
          minDist = dist;
          vMin = v2;
        }
      }
    }
  return (minDist < 100000);
}

float FieldDimensionsBH::LinesTable::getDistance(const Pose2DBH& pose) const
{
  float minDist = 100000;
  for(vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    Vector2BH<> v1 = (i->corner - pose).translation,
              v2 = (i->corner + Pose2DBH(Vector2BH<>(i->length, 0))
                    - pose).translation;
    if(v1.y < 0 && v2.y > 0)
    {
      float dist = v1.x + (v2.x - v1.x) * -v1.y / (v2.y - v1.y);
      if(dist >= 0 && dist < minDist)
        minDist = dist;
    }
  }
  return minDist == 100000 ? -1 : minDist;
}

const Vector2BH<>& FieldDimensionsBH::CornersTable::getClosest(const Vector2BH<>& p) const
{
  ASSERT(!empty());
  float maxDistance2 = numeric_limits<float>().max();
  const Vector2BH<>* closest = 0;
  for(const_iterator i = begin(); i != end(); ++i)
  {
    Vector2BH<> diff = p - *i;
    float distance2 = diff * diff;
    if(maxDistance2 > distance2)
    {
      maxDistance2 = distance2;
      closest = &*i;
    }
  }
  return *closest;
}

Vector2BH<int> FieldDimensionsBH::CornersTable::getClosest(const Vector2BH<int>& p) const
{
  return Vector2BH<int>(getClosest(Vector2BH<>(p)));
}

void FieldDimensionsBH::serialize(In* in, Out* out)
{
  ASSERT(in); // handling center circle only works when reading

  std::vector<LinesTable::Line>& carpetBorder(this->carpetBorder.lines);
  std::vector<LinesTable::Line>& fieldBorder(this->fieldBorder.lines);
  std::vector<LinesTable::Line>& fieldLines(this->fieldLines.lines);
  LinesTable::Circle centerCircle;
  std::vector<Vector2BH<> >& xCorner(corners[FieldDimensionsBH::xCorner]);
  std::vector<Vector2BH<> >& tCorner0(corners[FieldDimensionsBH::tCorner0]);
  std::vector<Vector2BH<> >& tCorner90(corners[FieldDimensionsBH::tCorner90]);
  std::vector<Vector2BH<> >& tCorner180(corners[FieldDimensionsBH::tCorner180]);
  std::vector<Vector2BH<> >& tCorner270(corners[FieldDimensionsBH::tCorner270]);
  std::vector<Vector2BH<> >& lCorner0(corners[FieldDimensionsBH::lCorner0]);
  std::vector<Vector2BH<> >& lCorner90(corners[FieldDimensionsBH::lCorner90]);
  std::vector<Vector2BH<> >& lCorner180(corners[FieldDimensionsBH::lCorner180]);
  std::vector<Vector2BH<> >& lCorner270(corners[FieldDimensionsBH::lCorner270]);

  STREAM_REGISTER_BEGIN;
  STREAM_BASE(SimpleFieldDimensions)
  STREAM(carpetBorder);
  STREAM(fieldBorder);
  STREAM(fieldLines);
  STREAM(centerCircle);
  this->fieldLines.pushCircle(centerCircle.center, centerCircle.radius, centerCircle.numOfSegments);

  STREAM(xCorner);
  STREAM(tCorner0);
  STREAM(tCorner90);
  STREAM(tCorner180);
  STREAM(tCorner270);
  STREAM(lCorner0);
  STREAM(lCorner90);
  STREAM(lCorner180);
  STREAM(lCorner270);
  STREAM_REGISTER_FINISH;
}

void FieldDimensionsBH::LinesTable::Line::serialize(In* in, Out* out)
{
  Vector2BH<>& from(corner.translation);
  Vector2BH<>to(corner * Vector2BH<>(length, 0.f));
  STREAM_REGISTER_BEGIN;
  STREAM(from);
  STREAM(to);
  STREAM_REGISTER_FINISH;
  if(in)
  {
    Vector2BH<> diff = to - from;
    corner.rotation = diff.angle();
    length = diff.abs();
  }
}
