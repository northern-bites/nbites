#ifndef InverseKinematics_h
#define InverseKinematics_h

#include "Kinematics.h"
namespace Kinematics{
    /*
     * Declarations for constants and methods concerning forward and inverse
     * kinematics.
     */
    //Accuracy constants for dls
    //in mm, how close dls will get to the target
    static const float UNBELIEVABLY_LOW_ERROR = 0.01f; //mm
    static const float REALLY_LOW_ERROR = 0.1f; //mm
    static const float ACCEPTABLE_ERROR = 0.5f; //mm
    static const float COARSE_ERROR     = 1.0f; //mm

    static const float dampFactor = 0.4f;
    static const float maxDeltaTheta = 0.5f;
    static const int maxAnkleIterations = 60;
    static const int maxHeelIterations = 20;


    static const float HYP_NOT_SET = -1000.0f;

    enum IKOutcome {
        STUCK = 0,
        SUCCESS = 1
    };

    struct IKLegResult {
        IKOutcome outcome;
        float angles[6];
    };

    const IKLegResult simpleLegIK(const ChainID chainID,
                                  const NBMath::ufvector3 & legGoal,
                                  float startAngles []);

    const IKLegResult legIK(const ChainID chainID,
                            const NBMath::ufvector3 &footGoal,
                            const NBMath::ufvector3 &footOrientation,
                            const NBMath::ufvector3 &bodyGoal,
                            const NBMath::ufvector3 &bodyOrientation,
                            const float HYPAngle = HYP_NOT_SET);
    /**
     * Wrapper method for IK which finds leg angles given a location
     * for the leg, and angles for the body relative to the world
     * and the HYP angle
     */
    const IKLegResult angleXYIK(const ChainID chainID,
                                const NBMath::ufvector3 & legGoal,
                                const float bodyAngleX,
                                const float bodyAngleY,
                                const float HYPAngle);

    const void clipChainAngles(const ChainID id,
                               float angles[]);
    const float getMinValue(const ChainID id, const int jointNumber);
    const float getMaxValue(const ChainID id, const int jointNumber);
    const NBMath::ufvector3 forwardKinematics(const ChainID id,
                                              const float angles[]);
    const NBMath::ufmatrix3 buildJacobians(const ChainID chainID,
                                              const float angles[]);


    // Both adjustment methods return whether the search was successful.
    // The correct angles required to fulfill the goal are returned through
    // startAngles by reference.
    const bool adjustAnkle(const ChainID chainID,
                           const NBMath::ufvector3 &goal,
                           float startAngles[],
                           const float maxError);
    const bool adjustHeel(const ChainID chainID,
                          const NBMath::ufvector3 &goal,
                          float startAngles[],
                          const float maxError);
    const IKLegResult dls(const ChainID chainID,
                          const NBMath::ufvector3 &goal,
                          const float startAngles[],
                          const float maxError = ACCEPTABLE_ERROR,
                          const float maxHeelError = UNBELIEVABLY_LOW_ERROR);


    const IKLegResult analyticLegIK(const ChainID chainID,
                                    const NBMath::ufvector3 &footGoal,
                                    const NBMath::ufvector3 &footOrientation,
                                    const NBMath::ufvector3 &bodyGoal,
                                    const NBMath::ufvector3 &bodyOrientation,
                                    const float givenHYPAngle = HYP_NOT_SET);


    NBMath::ufmatrix4 rotationHYPRightInv(const float HYP);
    NBMath::ufmatrix4 rotationHYPLeftInv(const float HYP);

};
#endif
