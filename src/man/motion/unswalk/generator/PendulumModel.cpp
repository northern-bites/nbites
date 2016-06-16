#include "PendulumModel.hpp"
#include <cmath>
#include <iostream>

using namespace std;

void PendulumModel::predictNext(float dt, float supportPivot){
   x = x + dx * dt + accX * (dt*dt)/2; 
   dx = dx + accX * dt;
   theta = atan2((x - supportPivot), h);
   //cout << "x-p: " << (x - supportPivot) << endl;
   accX = g * sin(theta) * cos(theta);
   //walkCycle.next();
}
