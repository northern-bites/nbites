/**
 * Landmark.h
 *
 * @author Tucker Hermans
 */

#ifndef _Landmark_h_DEFINED
#define _Landmark_h_DEFINED
using namespace std;

#include "FieldConstants.h"
#include "VisualCorner.h"
#include "VisualLine.h"

class Landmark
{
public:
    // Construcotrs & Destructors
    Landmark(FieldObject fo);
    Landmark(VisualCorner c);
    Landmark(VisualLine l);
    virtual ~Landmark();

    // Core Functions

    // Getters
    float getVisDist() { return visDist;}
    float getVisBearing() { return visBearing;}
    int getID() { return id;}
    float getX() {return x;}
    float getY() {return y;}

    // Setters

private:
    // Vision information
    float visDist;
    float visBearing;

    // Identity information
    int id;
    float x;
    float y;
}
#endif // _Landmark_h_DEFINED
