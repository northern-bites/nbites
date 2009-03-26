#include<iostream>
#include <cstdio>
using namespace std;
#include "AccEKF.h"
#include "ZmpEKF.h"
#include "BasicWorldConstants.h"

void AccFilter(FILE *f){

    AccEKF a;

    FILE *output = fopen("/tmp/accel_log.xls","w");
    fprintf(output,
            "time\taccX\taccY\taccZ\tfilteredAccX\tfilteredAccY\tfilteredAccZ\tfilteredAccXUnc\tfilteredAccYUnc\tfilteredAccZUnc\n");
    while (!feof(f)) {
        float accX,accY,accZ;
        fscanf(f,"%f\t%f\t%f\n",&accX,&accY,&accZ);
        a.update(accX,accY,accZ);
        float fAccX = a.getX();  float fAccY = a.getY();float fAccZ = a.getZ();
        float fAccXUnc = a.getXUnc();
        float fAccYUnc = a.getYUnc();
        float fAccZUnc = a.getZUnc();
        static float time = 0.0;
        fprintf(output,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
                time,accX,accY,accZ,fAccX,fAccY,fAccZ,
                fAccXUnc, fAccYUnc, fAccZUnc);
        time +=.02;
    }

}


void ZmpFilter(FILE *f){

    // parse the header of the file
    cout << fscanf(f,"time\tcom_x\tcom_y\tpre_x\tpre_y\tzmp_x\tzmp_y\t"
            "sensor_zmp_x\tsensor_zmp_y\treal_com_x\treal_com_y\tangleX\t"
            "angleY\taccX\taccY\taccZ\t"
            "lfl\tlfr\tlrl\tlrr\trfl\trfr\trrl\trrr\t"
            "state\n") <<endl;;

    ZmpEKF a;

    FILE *output = fopen("/tmp/zmp_log.xls","w");
    fprintf(output,"time\tcom_x\tcom_y\tpre_x\tpre_y\tzmp_x\tzmp_y\t"
            "filtered_zmp_x\tfiltered_zmp_y\tunfiltered_zmp_x\tunfiltered_zmp_y\n");
    while (!feof(f)) {

        float comX,comY,zmpX,zmpY,preX,preY,estZMPX,estZMPY;
        float angleX,angleY,accX =0.0f,accY=0.0f,accZ;
        float time;
        float bogus;
        int bogi;
        //cout <<
        fscanf(f,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t"
               "%f\t%f\t%f\t"
               "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",
               &time,&comX,&comY,&preX,&preY,&zmpX,&zmpY,
               &estZMPX,&estZMPY,&bogus,&bogus, &angleX,&angleY,
               &accX,&accY,&accZ,
               &bogus,&bogus,&bogus,&bogus,&bogus,&bogus,&bogus,&bogus,&bogi);
            //<<endl;

        estZMPX = comX + 310/GRAVITY_mss * accX;
        estZMPY = comY + 310/GRAVITY_mss * accY;

        ZmpTimeUpdate tUp = {zmpX,zmpY};
        //float accX = (estZMPX - comX)*GRAVITY_mss/0.31f;
        //float accY = (estZMPY - comY)*GRAVITY_mss/0.31f;
        //cout << "real_zmp* = "<<estZMPX <<","<<estZMPY<<endl;
        cout << "real_zmp - com* = "<<estZMPX-comX <<","<<estZMPY-comY<<endl;
        //cout << "com* = "<<comX <<","<<comY<<endl;
        cout << "accZ" << accZ<<endl;
        cout << "acc* = "<<accX <<","<<accY<<endl;
        ZmpMeasurement pMeasure = {comX,comY,accX,accY};
        a.update(tUp,pMeasure);

        float filtered_zmp_x = a.get_zmp_x();
        float filtered_zmp_y = a.get_zmp_y();
        float filtered_zmp_unc_x = a.get_zmp_unc_x();
        float filtered_zmp_unc_y = a.get_zmp_unc_y();

        fprintf(output, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
                time,
                comX, comY, preX, preY, zmpX, zmpY,
                filtered_zmp_x, filtered_zmp_y,
                estZMPX, estZMPY);
                //filtered_zmp_unc_x, filtered_zmp_unc_y,


        //float fAccX = a.getX();  float fAccY = a.getY();float fAccZ = a.getZ();
        //float fAccXUnc = a.getXUnc();
        //float fAccYUnc = a.getYUnc();
        //float fAccZUnc = a.getZUnc();
//         static float time = 0.0;
//         fprintf(output,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
//                 time,accX,accY,accZ,fAccX,fAccY,fAccZ,
//                 fAccXUnc, fAccYUnc, fAccZUnc);
//         time +=.02;
    }

}

int main(int argc, char * argv[]) {
    if (argc < 2){
        cout << "Not enough arguments" <<endl;
        return 1;
    }
    FILE *f = fopen(argv[1],"r");
    ZmpFilter(f);

    return 0;
}
