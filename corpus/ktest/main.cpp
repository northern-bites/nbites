#include "InverseKinematics.h"

using namespace NBMath;
using namespace std;
using namespace Kinematics;
void inverseKin(){

    //const ufvector3 goal = CoordFrame3D::vector3D(40,90,-310);
    const ufvector3 goal = CoordFrame3D::vector3D(0,50,-310);
    ChainID leg = Kinematics::LLEG_CHAIN;
    ChainID ankle = Kinematics::LANKLE_CHAIN;

    float legAngles[] = {0.0f,0.0f,0.0f,
                       0.0f,0.0f,0.0f};
    cout << "Start angles: " <<endl;
    for(int i =0; i < 6; i++){
        cout <<legAngles[i]<<endl;
    }
    Kinematics::IKLegResult result =  Kinematics::dls(leg,
                                                      goal, legAngles);

    cout << "Result angles: " <<endl;
    for(int i =0; i < 6; i++){
        cout <<result.angles[i]<<endl;
    }
    cout <<"Outcome "<< result.outcome<<endl;
    
    //Forward kinematics says we are at
    ufvector3 resultZ  = Kinematics::forwardKinematics(ankle,
                                                       result.angles);
    cout << "FK res 1" << resultZ<<endl;
    ufvector3 resultV  = Kinematics::forwardKinematics(leg,
                                                       result.angles);
    cout << "FK res 2" << resultV<<endl;
    

}


void forwardKin(){

    float legAngles[] = {0.0f,0.0f,-0.0f,
                       0.0f,-0.0f,0.0f};

    ufvector3 result1  = Kinematics::forwardKinematics(Kinematics::RANKLE_CHAIN,
                                                      legAngles);
    cout << "FK res1 " << result1<<endl;
    ufvector3 result2  = Kinematics::forwardKinematics(Kinematics::RLEG_CHAIN,
                                                      legAngles);
    cout << "FK res2 " << result2<<endl;
}


void ikSpeedTest(){
    float startAngles1[] = {0.0f,0.0f,0.0f,
                            0.0f,0.0f,0.0f};
    float startAngles2[] = {-0.35f,0.0f,0.0f,
                            0.0f,0.0f,0.0f};


    const int NUM_ITERS = 10000;


    for(int i = 0; i < NUM_ITERS; i++){
        ChainID leg;
        ChainID ankle;
        float  sign  = 1.0f;
        //set leg
        if(i%2 == 0){
            leg = Kinematics::LLEG_CHAIN;
            ankle = Kinematics::LANKLE_CHAIN;
            sign = 1.0f;
        }else{
            leg = Kinematics::RLEG_CHAIN;
            ankle = Kinematics::RANKLE_CHAIN;
            sign = -1.0f;
        }

        ufvector3 goal;
        float * startAngles;
        int goalDecider = i%5;
        
        switch(goalDecider){
        case 0:
            goal = CoordFrame3D::vector3D(0,sign*50,-310);
            startAngles = startAngles1;
            break;
        case 1:
            goal = CoordFrame3D::vector3D(20,sign*90,-310);
            startAngles = startAngles1;
            break;
        case 2:
            goal = CoordFrame3D::vector3D(-20,sign*90,-285);
            startAngles = startAngles1;
            break;
        case 3:
            goal = CoordFrame3D::vector3D(20,sign*90,-285);
            startAngles = startAngles2;
            break;

        default:
        case 4:
            goal = CoordFrame3D::vector3D(20,sign*90,-310);
            startAngles = startAngles2;
            break;

        }
#define DLS
#ifdef DLS
        Kinematics::IKLegResult result =  Kinematics::simpleLegIK(leg,
                                                                  goal, 
                                                                  startAngles);
#else
    ufvector3 footGoal = CoordFrame3D::vector3D(20,-60,-310);
    ufvector3 footOrientation = CoordFrame3D::vector3D(0,0,0);
    ufvector3 bodyGoal = CoordFrame3D::vector3D(0,0,0);
    ufvector3 bodyOrientation = CoordFrame3D::vector3D(0,0,0);

    Kinematics::IKLegResult result  = Kinematics::analyticLegIK(leg,
                                                                goal,
                                                                footOrientation,
                                                                bodyGoal,
                                                                bodyOrientation,
                                                                startAngles[0]);
#endif
    }

}


void anaIK(){
    float startAngles[] = {-1.0f,0.0f,0.0f,
                            0.0f,0.0f,0.0f};

    ufvector3 footGoal = CoordFrame3D::vector3D(20,-60,-310);
    ufvector3 footOrientation = CoordFrame3D::vector3D(0,0,0);
    ufvector3 bodyGoal = CoordFrame3D::vector3D(0,0,0);
    ufvector3 bodyOrientation = CoordFrame3D::vector3D(0,0,0);

    Kinematics::IKLegResult result  = Kinematics::analyticLegIK(RLEG_CHAIN,
                                                                footGoal,
                                                                footOrientation,
                                                                bodyGoal,
                                                                bodyOrientation,
                                                                startAngles[0]);
    cout << "Result angles: [";
    for(int i =0; i < 6; i++){
        cout <<result.angles[i]<<",";
    }
    cout <<"]"<<endl<<"Outcome "<< result.outcome<<endl;

    cout<<endl<<endl;
    Kinematics::IKLegResult result_dls =  Kinematics::dls(RLEG_CHAIN,
                                                          footGoal,startAngles);
    cout << "Result angles dls: [";
    for(int i =0; i < 6; i++){
        cout <<result_dls.angles[i]<<",";
    }
    cout <<"]"<<endl<<"Outcome dls"<<result_dls.outcome<<endl;

}

int main(){
    //anaIK();
    //inverseKin();
    ikSpeedTest();
    //forwardKin();
}
