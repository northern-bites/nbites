/*
 * @class VisionCorner
 * @class VisionLine
 * @class VisionPost
 * @class VisionBall
 *
 * Defines objects that are seen in the image, ie not in the real world. All
 * of these classes have Image as a friend so that it can update them easily
 * as the transformation methods run.
 *
 * @author Lizzie Mamantov
 */

#pragma once

#include "FieldCorners.h"
#include "FieldLines.h"
#include "Posts.h"

namespace tool{
namespace visionsim{

// Forward declaration for friending
class Image;

// We use two types of points: 3D points in camera coordinates and 2D points
// that represent a pixel in the image
typedef Eigen::Vector3f CameraPoint;
typedef Eigen::Vector2i ImagePoint;

class VisionCorner
{
    friend class Image;

public:
    VisionCorner(FieldCorner which);

    // X and Y locations IN THE IMAGE
    int x() { return imageCoordinates[X_VALUE]; }
    int y() { return imageCoordinates[Y_VALUE]; }
    bool green();

    // Is this corner BEHIND the image plane? This is important for drawing.
    // @see Image fixVisualPoints
    bool behind() { return (cameraCoordinates[Z_VALUE] < 0) ; }

private:
    // Specifies which corner this is
    FieldCorner concrete;
    // Stored for behind()
    CameraPoint cameraCoordinates;
    // Important for drawing obviously
    ImagePoint imageCoordinates;
};

class VisionLine
{
    friend class Image;

public:
    VisionLine(FieldLine which, VisionCorner* cor1, VisionCorner* cor2);

    // Checks if this is a field edge, ie a "green" line
    bool green() { return corner1->green(); }
    // Checks if both of its corners are behind the image plane, because in
    // this case we don't need to worry about the line at all
    bool bothPointsBehind() { return (corner1->behind() && corner2->behind()); }

    // Pixel points IN THE IMAGE
    ImagePoint& point1() { return visualPoint1; }
    ImagePoint& point2() { return visualPoint2; }

private:
    // Specifies which line
    FieldLine concrete;
    // These two points allow us to draw the line in the picture
    ImagePoint visualPoint1;
    ImagePoint visualPoint2;
    // Pointers to visual corners so we don't have to project them twice
    VisionCorner* corner1;
    VisionCorner* corner2;
};

class VisionPost
{
    friend class Image;

public:
    VisionPost(FieldPost which);

    // X and Y IN THE IMAGE of the TOP LEFT CORNER of the post "blob"
    int x() { return topLeftCorner[X_VALUE]; }
    int y() { return topLeftCorner[Y_VALUE]; }
    // Visual width and height for drawing
    int getVisualWidth() { return visualWidth; }
    int getVisualHeight() { return visualHeight; }
    // Checks if the post is behind the image plane--don't want to draw if so
    bool behind() { return cameraCoordinates[Z_VALUE] < 0; }

private:
    // Specifies which post
    FieldPost concrete;
    // Two points that give a good estimate of what the post would look like
    // in an image
    ImagePoint topLeftCorner;
    ImagePoint bottomPoint;
    // for behind()
    CameraPoint cameraCoordinates;
    // Necessary for drawing
    int visualWidth;
    int visualHeight;
};

class VisionBall
{
    friend class Image;

public:
    // X and Y of the CENTER of the ball IN THE IMAGE
    int x() { return center[X_VALUE]; }
    int y() { return center[Y_VALUE]; }
    // Visual radius of the ball
    int getVisualRadius() { return visualRadius; }
    // Checks if the ball is behind the image plane; we don't want it to
    // back-project into the image if so
    bool behind() { return cameraCoordinates[Z_VALUE] < 0; }

private:
    // Center point in the image
    ImagePoint center;
    // Stored for behind() check
    CameraPoint cameraCoordinates;
    // For drawing
    int visualRadius;
};

}
}
