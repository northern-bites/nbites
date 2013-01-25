/*
 * Defines constants used for images.
 */

#pragma once

namespace tool{
namespace visionsim{

static const float PI                   = 3.14159265;

// Angles in the model are in degrees
static const float TO_RAD               = PI/180;
static const float TO_DEG               = 180/PI;

static const int IMAGE_WIDTH            = 640;
static const int IMAGE_HEIGHT           = 480;

static const float BOTTOM_CAMERA_HEIGHT = 44.f;
static const float TOP_CAMERA_HEIGHT    = 48.5f;

static const int TOP_CAMERA_OFFSET      = 1.2f + 90.f;
static const int BOTTOM_CAMERA_OFFSET   = 39.7f + 90.f;
static const int FOCAL_LENGTH           = 590.f;
static const float FOCAL_LENGTH_CM      = 0.02f;
static const float CM_TO_PIX            = FOCAL_LENGTH/FOCAL_LENGTH_CM;

enum Camera{
    TOP = 0,
    BOTTOM
};

}
}
