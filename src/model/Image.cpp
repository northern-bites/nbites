#include "Image.h"
#include <vector>
#include <iostream>

using namespace Eigen;

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
}

void Image::update()
{
    updateCorners();
}

void Image::updateCorners()
{
    visibleCorners.clear();

    for(int i = 0; i < NUM_CORNERS; i++)
    {
        allCorners[i].cameraCoordinates =
            fieldToCameraCoords(ALL_CORNERS[i]);
        allCorners[i].imageCoordinates =
            cameraToImageCoords(allCorners[i].cameraCoordinates);

        if (isInImage(allCorners[i].imageCoordinates))
            visibleCorners.push_back(allCorners[i]);
    }
}

bool Image::isInImage(Vector2i point)
{
    return (point[X_VALUE] <= IMAGE_WIDTH && point[X_VALUE] >= 0 &&
            point[Y_VALUE] <= IMAGE_HEIGHT && point[Y_VALUE] >= 0);
}

// Takes a point (x, y, z) from the field coordinate system to another
// 3D point in the camera coordinate system
/*
 * From the robot's perspective:
 * +X = RIGHT
 * +Y = FORWARD
 * +Z = UP
 */
Vector3f Image::fieldToCameraCoords(int x, int y, int z)
{
   // Establish viewpoint, yaw, pitch matrices based on data
    Matrix4f viewpoint;
    viewpoint <<
        1, 0, 0, -world.robotX(),
        0, 1, 0, -world.robotY(),
        0, 0, 1, -cameraHeight,
        0, 0, 0, 1;

    Matrix4f yaw;
    float yw = (world.headYaw() + world.robotH())*TO_RAD;
    yaw <<
         cos(yw), -sin(yw), 0, 0,
         sin(yw),  cos(yw), 0, 0,
               0,        0, 1, 0,
               0,        0, 0, 1;

    Matrix4f pitch;
    float p = (world.headPitch() + cameraOffset)*TO_RAD;
    pitch <<
        1,       0,      0, 0,
        0,  cos(p),-sin(p), 0,
        0,  sin(p), cos(p), 0,
        0,       0,      0, 1;

    // Multiply the three together to get the overall transformation
    // Note that roll matrix would be the identity and so was left out
    Matrix4f trans = ((pitch*yaw)*viewpoint);

    // Homogeneous coordinates; z always 0 b/c points are on the field
    Vector4f hPoint; hPoint << x, y, z, 1;

    Vector4f ccPoint = trans*hPoint;

    return Vector3f(ccPoint[X_VALUE], ccPoint[Y_VALUE], ccPoint[Z_VALUE]);
}

// Overload for flexibility
Vector3f Image::fieldToCameraCoords(Vector3f worldPoint)
{
    return fieldToCameraCoords(worldPoint[X_VALUE],
                               worldPoint[Y_VALUE],
                               worldPoint[Z_VALUE]);
}

// Takes a point (x, y, z) from the camera coordinate system and projects
// it onto the image plane
Vector2i Image::cameraToImageCoords(float x, float y, float z)
{
    int px = ERROR;
    int py = ERROR;

    // Make sure we don't divide by zero first
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

    return Vector2i(px, py);
}

// Overloaded function that takes a Point3D instead of three floats and
// projects the point onto the image plane
Vector2i Image::cameraToImageCoords(Vector3f ccPoint)
{
    return cameraToImageCoords(ccPoint[X_VALUE],
                               ccPoint[Y_VALUE],
                               ccPoint[Z_VALUE]);
}
