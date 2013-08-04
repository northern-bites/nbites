/*
 * Defines constants relating the Nao's camera and the resulting images.
 *
 * @author Lizzie Mamantov
 */

#pragma once

namespace tool{
namespace visionsim{

// Angles in the model are in degrees; this is useful for the math
static const float PI                   = 3.14159265;
static const float TO_RAD               = PI/180;
static const float TO_DEG               = 180/PI;

// Nao image size
static const int IMAGE_WIDTH            = 640;
static const int IMAGE_HEIGHT           = 480;

// Based on a Nao standing in a typical position
static const float BOTTOM_CAMERA_HEIGHT = 44.f;
static const float TOP_CAMERA_HEIGHT    = 48.5f;

// How far each camera is angled away from vertical
static const int TOP_CAMERA_OFFSET      = 1.2f + 90.f;
static const int BOTTOM_CAMERA_OFFSET   = 39.7f + 90.f;

// About the camera's physical focal length
// @see Image cameraToImageCoords
static const int FOCAL_LENGTH           = 590.f;
static const float FOCAL_LENGTH_CM      = 0.02f;
static const float CM_TO_PIX            = FOCAL_LENGTH/FOCAL_LENGTH_CM;

// Used to specify which image is simulating which Nao camera
enum Camera{
    TOP = 0,
    BOTTOM
};

}
}
