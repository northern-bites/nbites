#pragma once

#include "Action.hpp"
#include "BodyModel.hpp"
#include "types/ActionCommand.hpp"

class Planner {
   virtual Action getAction(BodyModel p, ActionCommand::All a) = 0;
};

