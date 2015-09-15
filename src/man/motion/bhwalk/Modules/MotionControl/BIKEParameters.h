/**
* @file BIKEParameters.h
* @author <a href="mailto:judy@informatik.uni-bremen.de">Judith Müller</a>
*/

#pragma once

#define NUM_OF_POINTS 3

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Enum.h"

// class DynPoint: public Streamable
// {
// public:
//   int limb;
//   int phaseNumber;
//   int duration;
//   Vector3BH<> translation;
//   Vector3BH<> angle;
//   Vector3BH<> odometryOffset;

//   DynPoint() {};

//   DynPoint(int limb, int phaseNumber, int duration, Vector3BH<> translation, Vector3BH<>angle, Vector3BH<> odometryOffset):
//     limb(limb),
//     phaseNumber(phaseNumber),
//     duration(duration),
//     translation(translation),
//     angle(angle),
//     odometryOffset(odometryOffset)
//   {};

//   DynPoint(int limb, int phaseNumber, Vector3BH<> translation):
//     limb(limb),
//     phaseNumber(phaseNumber),
//     duration(0), //no change
//     translation(translation),
//     angle(Vector3BH<>(0.f, 0.f, 0.f)), //no change
//     odometryOffset(Vector3BH<>(0.f, 0.f, 0.f)) //no change
//   {};

//   ~DynPoint() {};

// private:
//   virtual void serialize(In* in, Out* out)
//   {
//     STREAM_REGISTER_BEGIN;
//     STREAM(limb);
//     STREAM(phaseNumber);
//     STREAM(duration);
//     STREAM(translation);
//     STREAM(angle);
//     STREAM(odometryOffset);
//     STREAM_REGISTER_FINISH;
//   }
// };

// class Phase: public Streamable
// {
// public:
//   ENUM(Limb,
//        leftFootTra,
//        leftFootRot,
//        rightFootTra,
//        rightFootRot,
//        leftArmTra,
//        leftHandRot,
//        rightArmTra,
//        rightHandRot
//       );

//   unsigned int duration;

//   Vector3BH<> controlPoints[Phase::numOfLimbs][NUM_OF_POINTS];
//   Vector2BH<> comTra[NUM_OF_POINTS];
//   Vector2BH<> headTra[NUM_OF_POINTS];

//   Vector3BH<> originPos[Phase::numOfLimbs];
//   Vector2BH<> comOriginPos, comOriginOffset, headOrigin;
//   Vector3BH<> odometryOffset;

//   virtual void serialize(In* in, Out* out);

//   Phase() {};

//   ~Phase() {}
// };

// class BIKEParameters : public Streamable
// {
// public:

//   int numberOfPhases;
//   float preview;
//   bool loop, autoComTra, ignoreHead;
//   char name[260];

//   Vector3BH<> footOrigin, armOrigin, footRotOrigin, handRotOrigin;
//   Vector2BH<> comOrigin, headOrigin;

//   std::vector<Phase> phaseParameters;
//   float kpx, kdx, kix, kpy, kdy, kiy;

//   /**
//   * The method serializes this object.
//   * @param in Points to an In-stream, when currently reading.
//   * @param out Points to an Out-stream, when currently writing.
//   */
//   virtual void serialize(In* in, Out* out);

//   void calcControlPoints();

//   Vector3BH<> getPositionBlame(const float& phase, const int& phaseNumber, const int& limb);


//   Vector2BH<> getComRefPositionBlame(const float& phase, const int& phaseNumber);
//   Vector2BH<> getHeadRefPositionBlame(const float& phase, const int& phaseNumber);

//   void initFirstPhase();
//   void initFirstPhase(Vector3BH<>* origins, Vector2BH<> head);

//   BIKEParameters():
//     numberOfPhases(0),
//     preview(150),
//     loop(false),
// 	ignoreHead(false),
//     footOrigin(0.f, 0.f, 0.f),
//     armOrigin(0.f, 0.f, 0.f),
//     footRotOrigin(0.f, 0.f, 0.f),
//     handRotOrigin(0.f, 0.f, 0.f),
// 	headOrigin(0.f, 0.f),
//     phaseParameters(0),
//     kpx(0.f),
//     kdx(0.f),
//     kix(0.f),
//     kpy(0.f),
//     kdy(0.f),
//     kiy(0.f)
//   {
//   };

//   ~BIKEParameters()
//   {};
// };
