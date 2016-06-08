/**
* @file OdometryData.h
* Contains the OdometryDataBH class.
* @author Max Risler
*/

#pragma once

#include "Tools/Math/Pose2D.h"

/**
* @class OdometryDataBH
* OdometryDataBH contains an approximation of overall movement the robot has done.
* @attention Only use differences of OdometryDataBH at different times.
* Position in mm
*/
class OdometryDataBH : public Pose2DBH {};

/**
* @class GroundTruthOdometryDataBH
* Contains an observed overall movement the robot has done.
*/
class GroundTruthOdometryDataBH : public OdometryDataBH {};
