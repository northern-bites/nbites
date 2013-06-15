#include <gtest/gtest.h>

#define private public
#define protected public
#include "../GameStateModule.h"
#undef private
#undef protected

#include "Common.h"
#include "RoboCupGameControlData.h"

#include "RoboGrams.h"
#include "GameState.pb.h"
#include "GCResponse.pb.h"
#include "Toggle.pb.h"

#include <string>

class GameStateTest : public testing::Test
{
public:
    GameStateTest() : testing::Test(),
                      gameState(16,2)
    {}

protected:
    man::gamestate::GameStateModule gameState;
    messages::GameState testData;

    virtual void SetUp() {
        testData.set_state(STATE_INITIAL);
        testData.set_kick_off_team(TEAM_BLUE);
        testData.set_secondary_state(STATE2_NORMAL);
        testData.set_drop_in_team(TEAM_BLUE);

        messages::TeamInfo *blue = testData.add_team();
        blue->set_team_number(16);
        blue->set_team_color(TEAM_BLUE);
        blue->set_goal_color(GOAL_YELLOW);

        messages::RobotInfo *bot;
        for(int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
        {
            bot = blue->add_player();
            bot->set_penalty(PENALTY_NONE);
        }

        messages::TeamInfo *red = testData.add_team();
        red->set_team_number(15);
        red->set_team_color(TEAM_RED);
        red->set_goal_color(GOAL_YELLOW);

        for(int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
        {
            bot = red->add_player();
            bot->set_penalty(PENALTY_NONE);
        }
    }
};

TEST_F(GameStateTest, Constructor)
{
    EXPECT_EQ(16, gameState.team_number);
    EXPECT_EQ(2 , gameState.player_number);
    EXPECT_EQ(false, gameState.last_button);
    EXPECT_EQ(false, gameState.last_initial);
    EXPECT_EQ(false, gameState.last_team);
    EXPECT_EQ(false, gameState.last_kickoff);
    EXPECT_EQ(GAMECONTROLLER_RETURN_MSG_ALIVE, gameState.response_status);
}

TEST_F(GameStateTest, AdvanceState)
{
    gameState.latest_data = testData;

    EXPECT_EQ(STATE_INITIAL, gameState.latest_data.state());

    gameState.advanceState();
    EXPECT_EQ(STATE_READY, gameState.latest_data.state());

    gameState.advanceState();
    EXPECT_EQ(STATE_SET, gameState.latest_data.state());

    gameState.advanceState();
    EXPECT_EQ(STATE_PLAYING, gameState.latest_data.state());
}

TEST_F(GameStateTest, PenaltyTest)
{
    testData.set_state(STATE_PLAYING);
    gameState.latest_data = testData;

    gameState.advanceState();
    EXPECT_EQ(GAMECONTROLLER_RETURN_MSG_MAN_PENALISE,
              gameState.response_status);
    EXPECT_EQ(PENALTY_MANUAL,
              gameState.latest_data.team(TEAM_BLUE).player(2-1).penalty());

    gameState.advanceState();
    EXPECT_EQ(GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE,
              gameState.response_status);
    EXPECT_EQ(PENALTY_NONE,
              gameState.latest_data.team(TEAM_BLUE).player(2-1).penalty());

    gameState.latest_data.mutable_team(TEAM_BLUE)->
        mutable_player(2-1)->set_penalty(PENALTY_SPL_BALL_HOLDING);

    gameState.advanceState();
    EXPECT_EQ(GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE,
              gameState.response_status);
    EXPECT_EQ(PENALTY_NONE,
              gameState.latest_data.team(TEAM_BLUE).player(2-1).penalty());
}

TEST_F(GameStateTest, SwitchTeams)
{
    gameState.latest_data = testData;

    gameState.switchTeam();

    std::string old_blue, old_red, new_blue, new_red;

    gameState.latest_data.team(TEAM_BLUE).SerializeToString(&new_blue);
    gameState.latest_data.team(TEAM_RED).SerializeToString(&new_red);

    // change testData's team colors so that we get a match.
    testData.mutable_team(TEAM_BLUE)->set_team_color(TEAM_RED);
    testData.mutable_team(TEAM_RED)->set_team_color(TEAM_BLUE);

    testData.team(TEAM_BLUE).SerializeToString(&old_blue);
    testData.team(TEAM_RED).SerializeToString(&old_red);

    EXPECT_EQ(old_blue, new_red);
    EXPECT_EQ(old_red, new_blue);
}

TEST_F(GameStateTest, SwitchKickoff)
{
    gameState.latest_data = testData;

    EXPECT_EQ(TEAM_BLUE, gameState.latest_data.kick_off_team());

    gameState.switchKickOff();
    EXPECT_EQ(TEAM_RED, gameState.latest_data.kick_off_team());

    gameState.switchKickOff();
    EXPECT_EQ(TEAM_BLUE, gameState.latest_data.kick_off_team());
}

TEST_F(GameStateTest, Reset)
{
    gameState.latest_data = testData;
    gameState.switchTeam();

    gameState.reset();

    std::string expected, actual;

    testData.mutable_team(TEAM_RED)->set_team_number(0);

    testData.SerializeToString(&expected);
    gameState.latest_data.SerializeToString(&actual);

    EXPECT_EQ(expected, actual);
}
