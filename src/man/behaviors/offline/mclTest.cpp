#include <vector>
#include <iostream>
using namespace std;

#include "Observation.h"
#include "MCL.h"

int main()
{
    // Testing the observation class stuff
    vector<Observation> obs;
    VisualLine* vl1 = new VisualLine();
    VisualLine* vl2 = new VisualLine();
    VisualCorner vc(100, 200, 200.0f,
                    50.0f, *vl1, *vl2, 200.1f, 300.0f);
    FieldObjects* fo = new FieldObjects(YELLOW_GOAL_RIGHT_POST);
    (*fo).setX(1000);
    (*fo).setY(200);
    obs.push_back(*(new Observation(vc)));
    obs.push_back(*(new Observation(*vl1)));
    obs.push_back(*(new Observation(*vl2)));
    obs.push_back(*(new Observation(*fo)));


    MCL* myLoc = new MCL();

    for(unsigned short i = 0; i < obs.size(); ++i) {
        cout << "(" << obs[i].getVisDist() << ", " << obs[i].getVisBearing() << ")"
             << endl;
    }

    // // Clean everything up
    delete vl1, vl2, fo;
}
