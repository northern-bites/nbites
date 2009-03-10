/**
 * @file   NogginStructs.h
 * @author Tucker Hermans <thermans@bowdoin.edu>
 * @date   Mon Mar  9 19:32:25 2009
 *
 * @brief  Houses all of our struct classes for the Noggin module
 */

#ifndef NogginStructs_h_DEFINED
#define NogginStructs_h_DEFINED

// Odometery change
class MotionModel
{
public:
    MotionModel(float f, float l, float r) :
        deltaF(f), deltaL(l), deltaR(r) {}
    MotionModel(const MotionModel& other) :
        deltaF(other.deltaF), deltaL(other.deltaL), deltaR(other.deltaR) {}
    MotionModel() {}
    float deltaF;
    float deltaL;
    float deltaR;
};

// Pose Estimate
class PoseEst
{
public:
    // Constructors
    PoseEst(float _x, float _y, float _h) :
        x(_x), y(_y), h(_h) {}
    PoseEst(const PoseEst& other) :
        x(other.x), y(other.y), h(other.h) {}
    PoseEst() {}
    float x;
    float y;
    float h;

    PoseEst operator+ (const PoseEst o)
    {
        return PoseEst(o.x + x,
                       o.y + y,
                       o.h + h);
    }
    void operator+= (const PoseEst o)
    {
        x += o.x;
        y += o.y;
        h += o.h;
    }
    PoseEst operator+ (const MotionModel u_t)
    {
        // Translate the relative change into the global coordinate system
        // And add that to the current estimate
        float calcFromAngle = h - M_PI / 2.0f;
        return PoseEst(u_t.deltaF * -cos(calcFromAngle) +
                       u_t.deltaL * sin(calcFromAngle),
                       u_t.deltaF * -sin(calcFromAngle) -
                       u_t.deltaL * cos(calcFromAngle),
                       h += u_t.deltaR);
    }
    void operator+= (const MotionModel u_t)
    {
        // Translate the relative change into the global coordinate system
        // And add that to the current estimate
        float calcFromAngle = h - M_PI / 2.0f;
        x += u_t.deltaF * -cos(calcFromAngle) + u_t.deltaL * sin(calcFromAngle);
        y += u_t.deltaF * -sin(calcFromAngle) - u_t.deltaL * cos(calcFromAngle);
        h += u_t.deltaR;
    }

  friend std::ostream& operator<< (std::ostream &o, const PoseEst &c)
  {
      return o << "(" << c.x << ", " << c.y << ", " << c.h << ")";
  }


};

/**
 * Class to hold the ball positin and velocity in the x and y directions
 */
class BallPose
{
public:
    float x;
    float y;
    float velX;
    float velY;

    BallPose operator+ (const BallPose o) {
        return BallPose(x + o.velX,
                        y + o.velY,
                        o.velX,
                        o.velY);
    }

    void operator+= (const BallPose o) {
        x += o.velX;
        y += o.velY;
        velX = o.velX;
        velY = o.velY;
    }

    BallPose(float _x, float _y, float _vx, float _vy) :
        x(_x), y(_y), velX(_vx), velY(_vy) {}
    BallPose() {}

};

/**
 * Stores field location information about a concrete point landmark
 */
class PointLandmark
{
public:
    PointLandmark(float _x, float _y) : x(_x), y(_y) {}
    PointLandmark() {}
    float x;
    float y;
};

/**
 * Stores field location information about a concrete line landmark
 */
class LineLandmark
{
public:
    LineLandmark(float _x1, float _y1, float _x2, float _y2) :
        x1(_x1), y1(_y2), x2(_x2), y2(_y2) {}
    LineLandmark() {}
    float x1;
    float y1;
    float x2;
    float y2;
};
#endif // NogginStructs
