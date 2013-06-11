#include "../GameStateModule.h"
#include "CommModule.h"
#include "GuardianModule.h"
#include "BehaviorModule.h"

#include "RoboGrams.h"

#include "GameState.pb.h"
#include "GCResponse.pb.h"
#include "Toggle.pb.h"

#include <gtest/gtest.h>

class GameStateTest : public testing::Test
{
protected:
    GameStateModule gameState_;
    CommModule comm_;
    GuardianModule guardian_;
    BehaviorModule behaviors_;
};

TEST_F(GameStateTest, Constructor)
{
    EXPECT_EQ(16,16);
    EXPECT_EQ(16, 3);
}
