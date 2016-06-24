#include <fstream>
#include <sstream>
#include <cmath>
#include "RLPlanner.hpp"

using namespace std;

RLPlanner::RLPlanner(std::string file) {
   // load the RL file
   loadFile(file);
}

bool RLPlanner::loadFile(std::string plannerFile) {
   ifstream f(plannerFile.c_str());   
   if (!f.is_open()) return false;
   
   string line;
   line = getLineNoComments(f); 


   while (f.good()) {
      line = getLineNoComments(f); 
      stringstream s(line);
      int goal, x, xdot, t, action;
      s >> goal >> x >> xdot >> t >> action;
      table[goal][(x+50)/5][(xdot + 250)/25][(t)/20] = action;
   }
   f.close();
   return true;
}

string RLPlanner::getLineNoComments(ifstream &f) {
   while (f.good()) {
      std::string line;
      getline(f, line);

      // throw away commented lines
      if (!(line[0] == '/' && line[1] == '/')) return line;
   }
   return "";
}

Action RLPlanner::getAction(BodyModel b, ActionCommand::All actionCommand) {
   Action a;
   a.ankleRotationL = 0;
   a.ankleRotationR = 0;

   // convert pendulum model variables to correct form
   float x = b.pendulumModel.x;
   float dx = b.pendulumModel.dx;
   int xIndex = (x + 50)/5;
   int dxIndex = (dx + 250)/25;
   int tIndex = (b.getWalkCycle().t * 1000/20);

   // check index's
   if (xIndex < 0 || xIndex >= 21) return a;
   if (dxIndex < 0 || dxIndex >= 21) return a;
   if (tIndex < 0 || tIndex >= 25) return a;
   
   int dir = 1;
   if (actionCommand.body.forward > 20) dir = 2;
   if (actionCommand.body.forward < -20) dir = 0;
   a.ankleRotationL = table[dir][xIndex][dxIndex][tIndex];
   a.ankleRotationR = table[dir][xIndex][dxIndex][tIndex];
   return a;
}

