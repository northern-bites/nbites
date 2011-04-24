/*
 * file NaoPaths
 *
 * @author Octavian Neamtu
 *
 * Centralized file for the paths we use on the robot
 *
 */

#pragma once

#ifdef OFFLINE

#define NAO_HOME_DIR "~/"
#define NAO_NAOQI_DIR NAO_HOME_DIR "/naoqi"
#define NAO_LOG_DIR NAO_NAOQI_DIR "/log"

#else

#define NAO_HOME_DIR "/home/nao"
#define NAO_NAOQI_DIR NAO_HOME_DIR "/naoqi"
#define NAO_LOG_DIR NAO_NAOQI_DIR "/log"

#endif
