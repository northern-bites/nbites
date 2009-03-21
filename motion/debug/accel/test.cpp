#include<iostream>
#include <cstdio>
using namespace std;
#include "AccEKF.h"
int main(int argc, char * argv[]) {
    if (argc < 2){
        cout << "Not enough arguments" <<endl;
        return 1;
    }

    AccEKF a;
    FILE *f = fopen(argv[1],"r");
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
    return 0;
}
