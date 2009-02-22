#ifndef WalkParameters_h_DEFINED
#define WalkParameters_h_DEFINED

#include <vector>
using namespace std;


enum WalkConfigIndices {
  pMaxStepLength = 0,
  pMaxStepHeight,
  pMaxStepSide,
  pMaxStepTurn,
  pZmpOffsetX,
  pZmpOffsetY
};

enum WalkExtraConfigIndices {
  pLHipRollBacklashCompensator = 0,
  pRHipRollBacklashCompensator,
  pHipHeight,
  pTorsoYOrientation
};

enum WalkArmsConfigIndices {
  pShoulderMedian = 0,
  pShoulderAmplitude,
  pElbowMedian,
  pElbowAmplitude
};


/**
 * This class wraps all of Aldebaran's walk configuration parameters.
 * Calling the apply method will use the proxy to set the parameters held in the
 * instance of the class.
 */
class WalkParameters {
 public:
  WalkParameters(const vector <float> _walkConfig = DEFAULT_CONFIG,
		 const vector <float> _walkExtraConfig = DEFAULT_EXTRA,
		 const vector <float> _walkArmsConfig = DEFAULT_ARMS)
    : walkConfig(_walkConfig), walkExtraConfig(_walkExtraConfig),
      walkArmsConfig(_walkArmsConfig) { }

// #ifdef NAOQI1
//   void apply(ALPtr<ALMotionProxy> proxy) const;
// #else
//   void apply(ALMotionProxy * proxy) const;
// #endif
 public: // Default parameters constants

  static const vector <float> DEFAULT_CONFIG;
  static const vector <float> DEFAULT_EXTRA;
  static const vector <float> DEFAULT_ARMS;

  static const WalkParameters DEFAULT_PARAMS;

  const float getMaxStepLength() const { return walkConfig[pMaxStepLength]; }
  const float getMaxStepSide() const { return walkConfig[pMaxStepSide]; }
  const float getMaxStepTurn() const { return walkConfig[pMaxStepTurn]; }

 private:
  const vector <float> walkConfig;
  const vector <float> walkExtraConfig;
  const vector <float> walkArmsConfig;
};

#endif
