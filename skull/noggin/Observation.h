/**
 * Landmark.h
 *
 * @author Tucker Hermans
 * @version %I%
 */

#ifndef _Landmark_h_DEFINED
#define _Landmark_h_DEFINED
using namespace std;

#include "FieldConstants.h"
#include "VisualCorner.h"
#include "VisualLine.h"

// All possible landmark IDs
enum LandmarkID
{
    // Blue goal
    BLUE_GOAL_LEFT_POST;
    BLUE_GOAL_RIGHT_POST;
    BLUE_GOAL_POST_UNCLER;

    // Yellow goal
    YELLOW_GOAL_LEFT_POST;
    YELLOW_GOAL_RIGHT_POST;
    YELLOW_GOAL_POST_UNCLER;

    // Line intersections

    // Lines
};

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
    /**
     * @return The distance reported by the visual sighting
     */
    float getVisDist() { return visDist; }
    /*
     * @return The bearing reported by the visual sighting
     */
    float getVisBearing() { return visBearing; }
    /**
     * @return The standard deviation of the distance of the observation.
     */
    float getDistSD() { return distSD; }
    /*
     * @return The standard deviation of the bearing of the observation.
     */
    float getBearingSD() { return bearingSD; }

    /*
     * @return The ID of the landmark, element of LandmarkID enumeration.
     */
    int getID() { return id; }
    /*
     * @return true if the observed landmark was not IDed to a specifc (x,y).
     */
    bool isAmbgiuous() { return ambiguous; }
    /*
     * @return The x value of the landmark on the playing field.
     */
    float getX() { return x; }
    /*
     * @return The y value of the landmark on the playing field.
     */
    float getY() { return y; }

    // Setters

private:
    // Vision information
    float visDist;
    float visBearing;
    float distSD;
    float bearingSD;

    // Identity information
    int id;
    bool ambiguous;
    float x;
    float y;
    float slope;
}
#endif // _Landmark_h_DEFINED
