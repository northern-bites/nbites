#include "GameStateModule.h"

#include <iostream>

#include "RoboCupGameControlData.h"
#include "Common.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DebugConfig.h"
#include "utilities-pp.hpp"

namespace man{
    namespace gamestate{

        GameStateModule::GameStateModule(int team, int player) :
        portals::Module(),
        gameStateOutput(base()),
        gcResponseOutput(base()),
        team_number(team),
        player_number(player),
        last_button(false),
        last_initial(false),
        last_team(false),
        last_kickoff(false),
        response_status(GAMECONTROLLER_RETURN_MSG_ALIVE),
        keep_time(false),
        start_time(0)
        {
            reset();

            if (!sharedMemory.open()) {
                NBL_ERROR("GameStateModule could not open shared memory!");
                man::tts::say(IN_GAME, "game state module could not open shared memory");
            }

            //Don't use stop because whistle might not be running.
            sharedMemory.whistle_listening() = false;
            sharedMemory.whistle_heard() = false;
        }

        GameStateModule::~GameStateModule()
        {
            if (sharedMemory.isOpen()) sharedMemory.close();
        }

        void GameStateModule::run_()
        {
            latchInputs();
            update();

            latest_data.set_penalty_is_placement(true);

            portals::Message<messages::GameState> output(&latest_data);
            gameStateOutput.setMessage(output);

            portals::Message<messages::GCResponse> response(0);
            response.get()->set_status(response_status);
            gcResponseOutput.setMessage(response);
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
            game_state_t last, next;
            last = (game_state_t) latest_data.state();
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
                    start_time = 0;
                }
                else
                {
                    keep_time = true;
                    if (!start_time)
                    {
                        start_time = realtime_micro_time();
                    }
                }
                // Did GC get our message yet??
                for (int i = 0; i < latest_data.team_size(); ++i)
                {
                    messages::TeamInfo* team = latest_data.mutable_team(i);
                    if (team->team_number() == team_number)
                    {
                        messages::RobotInfo* player = team->mutable_player(player_number-1);
                        if (response_status == GAMECONTROLLER_RETURN_MSG_MAN_PENALISE)
                        {
                            if(player->penalty())
                            {
                                response_status = GAMECONTROLLER_RETURN_MSG_ALIVE;
                            }
                        }
                        else if (response_status == GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE)
                        {
                            if(!player->penalty())
                            {
                                response_status == GAMECONTROLLER_RETURN_MSG_ALIVE;
                            }
                        }
                    }
                }
            }
            if (keep_time && commInput.message().have_remote_gc())
            {
                long long diff_time = realtime_micro_time() - start_time;
                latest_data.set_secs_remaining(600 -
                                               static_cast<unsigned int>(diff_time/MICROS_PER_SECOND));
                //TODO keep track of penalty times
            }

            next = (game_state_t) latest_data.state();
//            whistleHandler(last, next);
            latest_data.set_state( (int) next);
        }

        enum spl_bp_state {
            FIRST_INITIAL,
            FIRST_PENALTY,
            NORMAL_PLAYING
        };

        void GameStateModule::advanceState()
        {
#ifndef USE_SPL_BUTTONS
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
                    start_time = realtime_micro_time();
                    break;
                case STATE_PLAYING:
                    manual_penalize();
                    break;
            }
#else
            static spl_bp_state bp_state = FIRST_INITIAL;

            switch (latest_data.state())
            {
                case STATE_INITIAL:
                    latest_data.set_state(STATE_PLAYING);
                    manual_penalize();

                    if ( bp_state == FIRST_INITIAL ) {
                        NBL_WARN("spl button presses FIRST_INITIAL -> FIRST_PENALTY");
                        bp_state = FIRST_PENALTY;
                    }
                    break;

                case STATE_READY:
                    latest_data.set_state(STATE_PLAYING);
                    keep_time = true;
                    start_time = realtime_micro_time();
                    break;

                case STATE_SET:
                    latest_data.set_state(STATE_PLAYING);
                    keep_time = true;
                    start_time = realtime_micro_time();
                    break;

                case STATE_PLAYING:
                    if (bp_state == FIRST_PENALTY) {
                        NBL_WARN("spl button presses FIRST_PENALTY -> NORMAL_PLAYING");
                        bp_state = NORMAL_PLAYING;
                        manual_penalize();

                        keep_time = true;
                        start_time = realtime_micro_time();
                    } else {
                        NBL_WARN("spl button presses PENALTY SWITCH");
                        bp_state = NORMAL_PLAYING;
                        manual_penalize();
                    }
                    break;
            }
#endif
        }

        void GameStateModule::manual_penalize()
        {
            for (int i = 0; i < latest_data.team_size(); ++i)
            {
                messages::TeamInfo* team = latest_data.mutable_team(i);
                if (team->team_number() == team_number)
                {
                    messages::RobotInfo* player = team->mutable_player(player_number-1);
                    if (player->penalty())
                    {
                        response_status = GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE;
                        if (!commInput.message().have_remote_gc())
                            player->set_penalty(PENALTY_NONE);
                    }
                    else
                    {
                        response_status = GAMECONTROLLER_RETURN_MSG_MAN_PENALISE;
                        if (!commInput.message().have_remote_gc())
                            player->set_penalty(PENALTY_MANUAL);
                    }
                    break;
                }
            }
        }

        void GameStateModule::reset()
        {
            keep_time = false;
            latest_data.Clear();

            latest_data.set_state(STATE_INITIAL);
            latest_data.set_kick_off_team(team_number);
            latest_data.set_secondary_state(STATE2_NORMAL);
            latest_data.set_drop_in_team(team_number);

            latest_data.set_have_remote_gc(false);

            messages::TeamInfo* myTeam = latest_data.add_team();
            myTeam->set_team_number(team_number);
            myTeam->set_team_color(TEAM_BLUE);
            myTeam->set_goal_color(1); // TODO eliminate goal color
            for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
            {
                messages::RobotInfo* player = myTeam->add_player();
                player->set_penalty(PENALTY_NONE);
            }

            messages::TeamInfo* them = latest_data.add_team();
            them->set_team_number(0);
            them->set_team_color(TEAM_RED);
            them->set_goal_color(1); // TODO eliminate goal color
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
            latest_data.set_kick_off_team(latest_data.kick_off_team() ? team_number : team_number+1);
        }

        void GameStateModule::whistleHandler(game_state_t last, game_state_t& next) {

            latest_data.set_whistle_override(false);

            switch(last) {
                case STATE_READY: {
                    switch(next) {
                        case STATE_READY: {
                            sharedMemory.gamestate_do_stop();
                        } break;

                        case STATE_SET: {
                            sharedMemory.gamestate_do_start();
                        } break;
                            
                        case STATE_PLAYING: {
                            sharedMemory.gamestate_do_stop();
                        } break;
                    }
                } break;

                case STATE_SET: {
                    switch(next) {
                        case STATE_READY: {
                            sharedMemory.gamestate_do_stop();
                        } break;

                        case STATE_SET: {
                            bool heard = sharedMemory.gamestate_do_query();

                            if (heard) {
                                NBL_WARN("\n:::: WHISTLE OVERRIDE ::::\n\n");
                                man::tts::say(IN_SCRIMMAGE, "whistle heard!");
                                next = STATE_PLAYING;
                                latest_data.set_whistle_override(true);
                            }
                        } break;
                            
                        case STATE_PLAYING: {
                            sharedMemory.gamestate_do_stop();
                            NBL_WARN("\n:::: WHISTLE MISSED ::::\n\n");
                            man::tts::say(IN_SCRIMMAGE, "whistle missed!");
                        } break;
                    }
                } break;

                case STATE_PLAYING: {
                    switch(next) {
                        case STATE_READY: {
                            sharedMemory.gamestate_do_stop();
                        } break;

                        case STATE_SET: {
                            bool heard = sharedMemory.gamestate_do_query();

                            if (heard) {
                                next = STATE_PLAYING;
                                latest_data.set_whistle_override(true);
                            }

                        } break;
                            
                        case STATE_PLAYING: {
                            sharedMemory.gamestate_do_stop();
                        } break;
                    }
                } break;
            }
        }
}
}
