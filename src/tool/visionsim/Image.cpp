#include "Image.h"
#include <vector>
#include <iostream>
#include <math.h>

using namespace Eigen;

namespace tool{
namespace visionsim{

/*
 * The constructor sets up the vectors of vision objects
 *
 * @param state -- the model of the physical world
 * @see World.h
 * @param which -- the camera type for this image, ie top or bottom
 * @see ImageConstants.h for the enum
 */

Image::Image(World& state, Camera which) : world(state),
                                           type(which)
{
    // Set up camera information
    if(type == TOP)
    {
        cameraOffset = TOP_CAMERA_OFFSET;
        cameraHeight = TOP_CAMERA_HEIGHT;
    }
    else
    {
        cameraOffset = BOTTOM_CAMERA_OFFSET;
        cameraHeight = BOTTOM_CAMERA_HEIGHT;
    }

    // Set up a corner vector
    for(int i = 0; i < NUM_CORNERS; i++)
    {
        allCorners.push_back(VisionCorner(ALL_CORNERS[i]));
    }

    // Set up a line vector
    for(int i = 0; i < NUM_LINES; i++)
    {
        allLines.push_back(VisionLine(ALL_LINES[i],
                                      getCorner(ALL_LINES[i].corner1),
                                      getCorner(ALL_LINES[i].corner2)));
    }

    // Set up a post vector
    for(int i = 0; i < NUM_POSTS; i++)
    {
        allPosts.push_back(VisionPost(ALL_POSTS[i]));
    }
}

/*
 * Used to get a VisionCorner out of the vector based on its concrete
 * FieldCorner.
 *
 * @param type -- which concrete corner you want
 * @see Constructor for usage
 */

VisionCorner* Image::getCorner(FieldCorner type)
{
    for(int i = 0; i < NUM_CORNERS; i++)
    {
        if (allCorners[i].concrete == type) return &allCorners[i];
    }

    std::cout << "WARNING: Invalid corner type requested." << std::endl;
    return &allCorners[0];
}

/*
 * The main update method for this class. Runs all of the methods that
 * update specific parts, and is called by the model when something in the
 * world changes.
 */

void Image::update()
{
    updateCorners();
    updateLines();
    updateBall();
    updatePosts();
}

/*
 * Loops through all of the corners and recalculates their data based on
 * the current world situation.
 */

void Image::updateCorners()
{
    // Corners that were visible last frame might not be visible any more...
    // we need to start fresh with visibleCorners
    visibleCorners.clear();

    for(int i = 0; i < NUM_CORNERS; i++)
    {
        // Compute the 3D coordinates of the corner in the camera frame
        allCorners[i].cameraCoordinates =
            fieldToCameraCoords(allCorners[i].concrete);
        // Project the 3D point into the image
        allCorners[i].imageCoordinates =
            cameraToImageCoords(allCorners[i].cameraCoordinates);

        // Add this corner to visibleCorners if it's actually visible
        if (isInImage(allCorners[i].imageCoordinates))
            visibleCorners.push_back(allCorners[i]);
    }
}

/*
 * Loops through all of the lines to fix their two visual points. Their
 * corners are updated through updateCorners, but the visual points specify
 * how to actually draw the lines--ie, avoid connecting to a corner that's
 * behind the image plane.
 * @see fixVisualPoints
 */

void Image::updateLines()
{
    for(int i = 0; i < NUM_LINES; i++)
    {
        fixVisualPoints(allLines[i]);
    }
}

/*
 * Computes the ball's center point and visual radius based on its current
 * location.
 */

void Image::updateBall()
{
    // Compute and store its 3D camera-frame coordinates
    ball.cameraCoordinates = fieldToCameraCoords(world.ballX(),
                                                 world.ballY(),
                                                 world.ballZ());
    // Project the center of the ball into the image
    ball.center = cameraToImageCoords(ball.cameraCoordinates);

    // Compute the distance from the camera to the ball
    float ballDistance = sqrt(pow(ball.cameraCoordinates[X_VALUE], 2) +
                              pow(ball.cameraCoordinates[Y_VALUE], 2) +
                              pow(ball.cameraCoordinates[Z_VALUE], 2));

    /*
     * Compute the visual radius (in PIXELS) of the ball in the image
     * This is accomplished with a simple "similar triangles" ratio based
     * on the ball's actual radius, its distance, and the focal distance
     */
    float visualRadius = (((FOCAL_LENGTH_CM/ballDistance)*BALL_RADIUS) *
                          CM_TO_PIX);
    ball.visualRadius = int(visualRadius);
}

/*
 * Loop through the four posts and update their visual information based
 * on the robot's location.
 */

void Image::updatePosts()
{
    for(int i = 0; i < NUM_POSTS; i++)
    {
        // The center of the base of the post in camera coordinates
        CameraPoint bottomPt = fieldToCameraCoords(allPosts[i].concrete);
        // The top left point of the post, ie the one that the regular
        // vision system typically uses when describing blobs
        // Found in camera space from the post's dimensions
        CameraPoint topPt(bottomPt[X_VALUE] - (0.5*GOALPOST_WIDTH),
                          bottomPt[Y_VALUE] - GOALPOST_HEIGHT,
                          bottomPt[Z_VALUE]);

        // Projecting these points into the image
        allPosts[i].topLeftCorner = cameraToImageCoords(topPt);
        allPosts[i].bottomPoint = cameraToImageCoords(bottomPt);
        allPosts[i].cameraCoordinates = bottomPt;

        // Compute the distance from the camera to the post
        float postDistance = sqrt(pow(bottomPt[X_VALUE], 2) +
                                  pow(bottomPt[Y_VALUE], 2) +
                                  pow(bottomPt[Z_VALUE], 2));

        // Compute the approximate visual dimensions in PIXELS
        // @see updateBall which uses the same technique for the radius
        allPosts[i].visualWidth = (((FOCAL_LENGTH_CM/postDistance) *
                                    GOALPOST_WIDTH) * CM_TO_PIX);

        allPosts[i].visualHeight = (((FOCAL_LENGTH_CM/postDistance) *
                               GOALPOST_HEIGHT) * CM_TO_PIX);
    }
}

/*
 * A simple check of whether a 2D point in the image plane is acutally in
 * the 640 by 480 image.
 *
 * @param point -- the pixel in the image plane
 * @returns true if point is in the image, false if not
 */

bool Image::isInImage(ImagePoint point)
{
    return (point[X_VALUE] <= IMAGE_WIDTH && point[X_VALUE] >= 0 &&
            point[Y_VALUE] <= IMAGE_HEIGHT && point[Y_VALUE] >= 0);
}

/* Takes a point (x, y, z) from the field coordinate system to another
 * 3D point in the camera coordinate system
 *
 * The ROBOT's coordinate system is, from the robot's perspective
 * +X = RIGHT
 * +Y = FORWARD
 * +Z = UP
 *
 * @returns a 3D point in camera coordinates that corresponds to the original
 *          world point
 */
CameraPoint Image::fieldToCameraCoords(int x, int y, int z)
{
    // Establish viewpoint, yaw, pitch matrices based on data
    // Viewpoint matrix is the translation matrix for the robot's current
    // position and the camera's height, moving the origin from (0, 0)
    // on the field to the camera itself
    Matrix4f viewpoint;
    viewpoint <<
        1, 0, 0, -world.robotX(),
        0, 1, 0, -world.robotY(),
        0, 0, 1, -cameraHeight,
        0, 0, 0, 1;

    // Yaw matrix takes into account the robot moving its head side
    // to side
    Matrix4f yaw;
    float yw = (world.headYaw() + world.robotH())*TO_RAD;
    yaw <<
         cos(yw), -sin(yw), 0, 0,
         sin(yw),  cos(yw), 0, 0,
               0,        0, 1, 0,
               0,        0, 0, 1;

    // Pitch matrix takes into account the head's up/down motion
    Matrix4f pitch;
    float p = (world.headPitch() + cameraOffset)*TO_RAD;
    pitch <<
        1,       0,      0, 0,
        0,  cos(p),-sin(p), 0,
        0,  sin(p), cos(p), 0,
        0,       0,      0, 1;

    // Note: there is no ROLL matrix, roll is assumed to be constant
    // This isn't entirely accurate but it is close enough to be useful

    // Multiply the three together to get the overall transformation
    // Note that roll matrix would be the identity and so was left out
    Matrix4f trans = ((pitch*yaw)*viewpoint);

    // Homogeneous coordinates version of the point
    Vector4f hPoint; hPoint << x, y, z, 1;

    // Multiplying the trans matrix by the homogeneous point gives us the
    // transformed camera coordinates point
    Vector4f ccPoint = trans*hPoint;

    return CameraPoint(ccPoint[X_VALUE], ccPoint[Y_VALUE], ccPoint[Z_VALUE]);
}

/*
 * Overloaded the above method for flexibility
 *
 * @param worldPoint -- the same as x, y, and z above, but in a vector
 */

CameraPoint Image::fieldToCameraCoords(Vector3f worldPoint)
{
    return fieldToCameraCoords(worldPoint[X_VALUE],
                               worldPoint[Y_VALUE],
                               worldPoint[Z_VALUE]);
}

/*
 * Takes a point (x, y, z) from the camera coordinate system and projects
 * it onto the image plane.
 *
 * @returns a 2D point in the image (ie a pixel) where the 3D point appears
 */
ImagePoint Image::cameraToImageCoords(float x, float y, float z)
{
    int px = ERROR;
    int py = ERROR;

    // Make sure we don't divide by zero first--this shouldn't happen
    if (z != 0)
    {
        px = (x/z)*FOCAL_LENGTH;
        py = (y/z)*FOCAL_LENGTH;
    } else
    {
        std::cout << "Problem: z value was zero!" << std::endl;
    }

    // Translate so (0, 0) in image coords is at top left like usual
    px = px + (0.5f*IMAGE_WIDTH);
    py = py + (0.5f*IMAGE_HEIGHT);

    return ImagePoint(px, py);
}

/*
 * Overload of the above function.
 *
 * @param ccPoint the same as x, y, z in the above, but in a vector
 */
ImagePoint Image::cameraToImageCoords(CameraPoint ccPoint)
{
    return cameraToImageCoords(ccPoint[X_VALUE],
                               ccPoint[Y_VALUE],
                               ccPoint[Z_VALUE]);
}

/*
 * Given a line, determines where it intercepts the plane of the image if
 * one of its corners is actually behind the image plane. Otherwise, corners
 * behind the image plane project back into the image and the lines go
 * a little crazy.
 *
 * @param line -- the visual line that needs updating
 */
void Image::fixVisualPoints(VisionLine& line)
{
    // If both of the corners are visible, correction isn't needed
    if(!line.corner1->behind() && !line.corner2->behind())
    {
        line.visualPoint1 = line.corner1->imageCoordinates;
        line.visualPoint2 = line.corner2->imageCoordinates;
        return;
    }

    // If both of the corners are behind, we're not drawing this line anyways
    if(line.corner1->behind() && line.corner2->behind())
    {
        line.visualPoint1 = ImagePoint(ERROR, ERROR);
        line.visualPoint2 = ImagePoint(ERROR, ERROR);
        return;
    }

    // Compute the intersection of the line and the image plane
    CameraPoint la = line.corner1->cameraCoordinates;
    CameraPoint lb = line.corner2->cameraCoordinates;
    CameraPoint intersection3D;
    intersection3D = (((FOCAL_LENGTH_CM - la[Z_VALUE]) /
                     (lb[Z_VALUE] - la[Z_VALUE]))*(lb-la)) + la;

    // If corner1 is the one behind, replace its visual data with the
    // intersection
    if(line.corner1->behind())
    {
        line.visualPoint1 = cameraToImageCoords(intersection3D);
        line.visualPoint2 = line.corner2->imageCoordinates;
        return;
    }

    // else corner2 is the one behind, so replace its visual data instead
    line.visualPoint1 = cameraToImageCoords(intersection3D);
    line.visualPoint2 = line.corner1->imageCoordinates;
}

}
}
