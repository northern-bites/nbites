#include "SonarRecorder.hpp"


SonarRecorder::SonarRecorder(){

   command_counter = 0;
   cycle_counter = 0;
   first_call = true;

   commands.clear();
   commands.push_back(Sonar::Mode::TLRL); // left   
   commands.push_back(Sonar::Mode::TLRR); // middle
   commands.push_back(Sonar::Mode::TRRR); // right
   commands.push_back(Sonar::Mode::TLRL); // left  
   commands.push_back(Sonar::Mode::TRRL); // other middle 
   commands.push_back(Sonar::Mode::TRRR); // right

   std::vector<int> left;
   std::vector<int> middle;
   std::vector<int> right;
   sonarWindow.push_back(left);
   sonarWindow.push_back(middle);
   sonarWindow.push_back(right);

}


/* Update the rolling window of recent observations and return the next request */
float SonarRecorder::update(float sonar[Sonar::NUMBER_OF_READINGS]){
  
   if (first_call){
      first_call = false;
      return commands[command_counter];
   }

   cycle_counter++;
   if (cycle_counter == CYCLES_PER_PING){
      cycle_counter = 0;
      // add RBOTH so we can continue to read left and right instead of just one set of values
      return processUpdate(sonar) + Sonar::Mode::RBOTH; 
   } else {
      return Sonar::Mode::NO_PING; 
   }

}


float SonarRecorder::processUpdate(float sonar[Sonar::NUMBER_OF_READINGS]){

   float command = commands[command_counter];
   std::vector<int> left;
   std::vector<int> right;

   for(int i=Sonar::Left0; i<=Sonar::Left9; i++){
      if (sonar[i] < Sonar::MAX && sonar[i] >= Sonar::MIN)
         left.push_back(static_cast <int>(sonar[i]*1000.f));
   }

   for(int i=Sonar::Right0; i<=Sonar::Right9; i++){
      if (sonar[i] < Sonar::MAX && sonar[i] >= Sonar::MIN)
         right.push_back(static_cast <int>(sonar[i]*1000.f));
   }

   if(command == Sonar::Mode::TLRL ){  
      // receiving on left
      sonarWindow[Sonar::LEFT].insert(sonarWindow[Sonar::LEFT].end(), left.begin(), left.end());
      leftDrop.push_back((int)left.size());
      if(leftDrop.size() > WINDOW_SIZE){
         int drop = leftDrop.front();
         sonarWindow[Sonar::LEFT].erase(sonarWindow[Sonar::LEFT].begin(), sonarWindow[Sonar::LEFT].begin()+drop);
         leftDrop.pop_front();
      }
   } else if (command == Sonar::Mode::TRRR ){  
      // receiving on right
      sonarWindow[Sonar::RIGHT].insert(sonarWindow[Sonar::RIGHT].end(), right.begin(), right.end());
      rightDrop.push_back((int)right.size());
      if(rightDrop.size() > WINDOW_SIZE){
         int drop = rightDrop.front();
         sonarWindow[Sonar::RIGHT].erase(sonarWindow[Sonar::RIGHT].begin(), sonarWindow[Sonar::RIGHT].begin()+drop);
         rightDrop.pop_front();
      }
   } else if (command == Sonar::Mode::TRRL ){
      // receiving on middle (left)
      sonarWindow[Sonar::MIDDLE].insert(sonarWindow[Sonar::MIDDLE].end(), left.begin(), left.end());
      middleDrop.push_back((int)left.size());
      if(middleDrop.size() > WINDOW_SIZE){
         int drop = middleDrop.front();
         sonarWindow[Sonar::MIDDLE].erase(sonarWindow[Sonar::MIDDLE].begin(), sonarWindow[Sonar::MIDDLE].begin()+drop);
         middleDrop.pop_front();
      }
   } else if (command == Sonar::Mode::TLRR ){
      // receiving on middle (right)
      sonarWindow[Sonar::MIDDLE].insert(sonarWindow[Sonar::MIDDLE].end(), right.begin(), right.end());
      middleDrop.push_back((int)right.size());
      if(middleDrop.size() > WINDOW_SIZE){
         int drop = middleDrop.front();
         sonarWindow[Sonar::MIDDLE].erase(sonarWindow[Sonar::MIDDLE].begin(), sonarWindow[Sonar::MIDDLE].begin()+drop);
         middleDrop.pop_front();
      }
   }

   // update the command
   if (command_counter == (int)commands.size()-1) command_counter = 0;
   else command_counter++;
   return commands[command_counter];

}

