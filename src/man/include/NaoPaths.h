/*
 * file NaoPaths
 *
 * @author Octavian Neamtu
 *
 * Centralized file for the paths we use on the robot
 *
 */

#ifndef _NAOPATHS_H_
#define _NAOPATHS_H_

#include <string>

namespace common {
namespace paths {

#ifdef OFFLINE

static const std::string NAO_HOME_DIR = "~/";

#else

static const std::string NAO_HOME_DIR = "/home/nao/";

#endif

static const std::string NAO_NAOQI_DIR = NAO_HOME_DIR + "naoqi/";
static const std::string NAO_LOG_DIR = NAO_NAOQI_DIR + "log/";

static const std::string NAO_LOG_EXTENSION = ".log";

#endif

#ifdef OFFLINE
#ifndef NBITES_DIR
#warning "NBITES_DIR not set, will reset to default!"
#define NBITES_DIR __FILE__  "/../.."
#endif
#endif

}
}
