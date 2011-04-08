#ifndef LINEFIT_H
#define LINEFIT_H
#include "math.h"

class LineFit
{
public:
    LineFit();
    void clear();
    double area() {return sumW;}
    double centerX() { return sumX / sumW;}
    double centerY() { return sumY / sumW;}
    double firstPrincipalLength() { solve(); return pLen1;}
    double secondPrincipalLength() { solve(); return pLen2;}
    bool hasPrincipalAxes() {solve(); return uPAI != 0 || vPAI != 0;}
    double firstPrincipalAxisU() { solve(); return uPAI;}
    double firstPrincipalAxisV() {solve(); return vPAI;}
    double secondPrincipalAxisU() {solve(); return -vPAI;}
    double secondPrincipalAxisV() {solve(); return uPAI;}
    double firstPrincipalAngle() {solve(); return atan2(vPAI, uPAI);}
    double secondPrincipalAngle() {solve(); return atan2(-uPAI, vPAI);}
    double sumSquaredError() {solve(); return ssError;}
    double rmsError() { return sqrt(sumSquaredError() / sumW);}
    void add(double x, double y);
    void add(double x, double y, double w);
    void sub(double w, double x, double y);
    void sum(double x, double y);
    void draw();
    void solve();

private:
    double sumW;
    double sumX, sumY, sumXY, sumX2, sumY2;

    bool solved;

    double pLen1, pLen2;         // principal lengths, pLen1 >= pLen2
    double uPAI, vPAI;           // components of unit vector along 1st principal axis
    double ssError;
};

#endif // LINEFIT_H
