#ifndef StepCommand_h
#define StepCommand_h

class StepCommand : public MotionCommand {


public:
    typedef boost::shared_ptr<StepCommand> ptr;

    StepCommand(float _x_mms, float _y_mms, float _theta_rads, int nSteps = 1)
  : MotionCommand(MotionConstants::WALK),
    x_mms(_x_mms),y_mms(_y_mms),theta_rads(_theta_rads), numSteps(nSteps)
        { setChainList(); }
    ~StepCommand(){}
public:
//    velocities
    const float x_mms;    //mm/second
    const float y_mms;    //mm/second
    const float theta_rads; //rad/second
    const int numSteps;

protected:
	virtual void setChainList() {
        chainList.assign(MotionConstants::STEP_CHAINS,
                         MotionConstants::STEP_CHAINS +
                         MotionConstants::STEP_NUM_CHAINS);
    }

public:
    friend std::ostream& operator<< (std::ostream &o, const StepCommand &w)
        {
            return o << "StepCommand("
                     << w.x_mms << "," << w.y_mms << ","
                     << w.theta_rads << ",nSteps=" <<w.numSteps
                     << ") ";
        }
};

#endif
