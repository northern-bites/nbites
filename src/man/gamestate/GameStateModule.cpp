#include "GameStateModule.h"

#include <iostream>

#include "RoboCupGameControlData.h"
#include "Common.h"

namespace man{
namespace gamestate{

GameStateModule::GameStateModule(int team, int player) :
    portals::Module(),
    gameStateOutput(base()),
    team_number(team),
    player_number(player)
{
    reset();
}

GameStateModule::~GameStateModule()
{
}

void GameStateModule::run_()
{
    latchInputs();
    update();

    portals::Message<messages::GameState> output(&latest_data);
    gameStateOutput.setMessage(output);
}

void GameStateModule::latchInputs()
{
    commInput         .latch();
    buttonPressInput  .latch();
    initialStateInput .latch();
    switchTeamInput   .latch();
    switchKickOffInput.latch();
}

void GameStateModule::update()
{
    // Check comm input last so we can reset button toggles.
    if (buttonPressInput.message().toggle() != last_button)
    {
        last_button = !last_button;
        if (!commInput.message().have_remote_gc()
            || latest_data.state() == STATE_PLAYING)
        {
            advanceState();
        }
    }
    if (initialStateInput.message().toggle() != last_initial)
    {
        last_initial = !last_initial;
        if (!commInput.message().have_remote_gc())
            reset();
    }
    if (switchTeamInput.message().toggle() != last_team)
    {
        last_team = !last_team;
        if (!commInput.message().have_remote_gc()
            && latest_data.state() == STATE_INITIAL)
            switchTeam();
    }
    if (switchKickOffInput.message().toggle() != last_kickoff)
    {
        last_kickoff = !last_kickoff;
        if (!commInput.message().have_remote_gc()
            && latest_data.state() == STATE_INITIAL)
            switchKickOff();
    }
    if (commInput.message().have_remote_gc())
    {
        latest_data = commInput.message();
        if (latest_data.state() != STATE_PLAYING)
        {
            keep_time = false;
        }
        else
        {
            keep_time = true;
        }
    }
    if (keep_time)
    {
        // @TODO: keep track of secs remaining if we have no comm.
    }
}

void GameStateModule::advanceState()
{
    switch (latest_data.state())
    {
    case STATE_INITIAL:
        latest_data.set_state(STATE_READY);
        break;
    case STATE_READY:
        latest_data.set_state(STATE_SET);
        break;
    case STATE_SET:
        latest_data.set_state(STATE_PLAYING);
        keep_time = true;
        break;
    case STATE_PLAYING:
        manual_penalize();
        break;
    }
}

void GameStateModule::manual_penalize()
{
    int i;
    for (i = 0; i < latest_data.team_size(); ++i)
    {
        messages::TeamInfo* team = latest_data.mutable_team(i);
        if (team->team_number() == team_number)
        {
            messages::RobotInfo* player = team->mutable_player(player_number-1);
            player->penalty() ? player->set_penalty(PENALTY_NONE) :
                player->set_penalty(PENALTY_MANUAL);
            break;
        }
    }
}

void GameStateModule::reset()
{
    keep_time = false;
    latest_data.Clear();
    messages::TeamInfo* myTeam = latest_data.add_team();
    myTeam->set_team_number(team_number);
    myTeam->set_team_color(TEAM_BLUE);
    myTeam->set_goal_color(GOAL_YELLOW);
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        messages::RobotInfo* player = myTeam->add_player();
        player->set_penalty(PENALTY_NONE);
    }

    messages::TeamInfo* them = latest_data.add_team();
    them->set_team_color(TEAM_RED);
    them->set_goal_color(GOAL_YELLOW);
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        messages::RobotInfo* player = them->add_player();
        player->set_penalty(PENALTY_NONE);
    }
}
void GameStateModule::switchTeam()
{
    // Switch the order of the TeamInfo messages' indicies
    // Set the new blue team to blue, and the new red team to red
    latest_data.mutable_team()->SwapElements(0,1);
    latest_data.mutable_team(TEAM_BLUE)->set_team_color(TEAM_BLUE);
    latest_data.mutable_team(TEAM_RED)->set_team_color(TEAM_RED);
}
void GameStateModule::switchKickOff()
{
    latest_data.set_kick_off_team(latest_data.kick_off_team() ? TEAM_BLUE : TEAM_RED);
}

}
}
