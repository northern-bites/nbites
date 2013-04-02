#pragma once

#include "RoboGrams.h"
#include "Toggle.pb.h"
#include "GameState.pb.h"

namespace man{
namespace gamestate{

/**
 * @class: Should be a syncronized version of the game controller.
 *         In the cognition diagram, should attempt to provide
 *         accurate data about the game even when there is lag between
 *         game controller packets. Should store and update its data
 *         given no input change.
 */

class GameStateModule : public portals::Module
{
public:
    GameStateModule(int team, int player);
    virtual ~GameStateModule();

    void run_();

    portals::OutPortal<messages::GameState> gameStateOutput;

    portals::InPortal<messages::GameState> commInput;
    portals::InPortal<messages::Toggle>    buttonPressInput;
    portals::InPortal<messages::Toggle>    initialStateInput;
    portals::InPortal<messages::Toggle>    switchTeamInput;
    portals::InPortal<messages::Toggle>    switchKickOffInput;

private:
    void latchInputs();
    void update();
    void advanceState();
    void manual_penalize();
    void reset();
    void switchTeam();
    void switchKickOff();

    messages::GameState latest_data;
    unsigned int team_number;
    unsigned int player_number;

    bool last_button;
    bool last_initial;
    bool last_team;
    bool last_kickoff;

    bool keep_time; // @TODO implement this feature.
};

}
}
