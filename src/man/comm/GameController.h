
#ifndef _GameController_h_DEFINED
#define _GameController_h_DEFINED

#include <vector>
#include <pthread.h>
#include <boost/shared_ptr.hpp>

#include "GameControllerConfig.h"
#include "RoboCupGameControlData.h"

class GameController;

#define USE_PYTHON_GC
#ifdef USE_PYTHON_GC
#include <Python.h>

typedef struct PyGameController_t {
    PyObject_HEAD
    boost::shared_ptr<GameController> _gc;
} PyGameController;

extern PyObject* PyGameController_new(boost::shared_ptr<GameController> _gc);

extern PyTypeObject PyGameControllerType;

#endif // USE_PYTHON_GC

class GameController
{
#ifdef USE_PYTHON_GC
    friend class PyGameController_t;
#endif
public:
    GameController();
    virtual ~GameController() {}

    /* interface functions for RoboCupGameControlData
       use these for on demand information rather than waiting
       for the updates */
    const TeamInfo* getMyTeam() const { return myTeam; }
    const RoboCupGameControlData& getGameData() const { return controlData; }

    void handle_packet(const char *msg, int len);
    bool shouldResetTimer();

    // Public data access interface
    const uint32 timeRemaining();
    const uint8 team();
    const TeamInfo teams(uint8 team);
    const uint8 color();
    const uint16 player();
    const uint8 kickOffTeam();
    const uint8 gameState();
    const uint8 gameSecondaryState();
    const uint16 penalty();
    const uint16 penalties(uint16 player);
    const uint16 penaltySeconds();
    const uint16 penaltySeconds(uint16 player);
    const bool isManuallyPenalized(void) const;
	const bool isPenalized(void);
    void setTeam(uint8 team);
    void setColor(uint8 color);
    void setPlayer(uint16 player);
    void setKickOffTeam(uint8 team);
    void setGameState(uint8 state);
    void setPenalty(uint16 penalized);

    // Button Click Methods
    void advanceButtonClickState();
    void toggleTeamColor();
    void toggleKickoff();
    void manualPenalize(bool penalize);
    bool shouldSendManualPenalty() const;
    void sentManualPenalty();

private:
    // check the validity of the given message and store the data
    // (if valid) into the referenced RoboCupGameControlData struct
    bool validatePacket(const char *msg, int len,
                        RoboCupGameControlData &packet);
    void swapTeams(int team);
    void rawSwapTeams(RoboCupGameControlData& data);

private:
    /* local copy of the GameController data */
    RoboCupGameControlData controlData;
    TeamInfo* myTeam;
    uint16 playerNumber;
    bool justManuallyPenalized;
    bool manuallyPenalized;

    static const uint8 NUM_TEAMS;//2

    // thread mutex to enable locking and thread-safe data access
    pthread_mutex_t mutex;
};

#endif
