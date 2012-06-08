#ifndef _SOUND_PATHS_H_
#define _SOUND_PATHS_H_

#include <string>

namespace man {
namespace corpus {
namespace guardian {
namespace sound_paths {

static const std::string quiet = " -q ";
static const std::string sout = "aplay"+quiet;
static const std::string sdir = "/usr/share/naoqi/wav/";
static const std::string nbsdir = "/home/nao/nbites/audio/";
static const std::string wav = ".wav";
static const std::string shutdown_wav = sdir + "shutdown" + wav;
static const std::string heat_wav = sdir + "heat" + wav;
static const std::string energy_wav = sdir + "energy" + wav;
static const std::string mynameis_wav = nbsdir + "mynameis" + wav;
static const std::string my_address_is_wav = sdir + "my_internet_address_is" + wav;
static const std::string stiffness_removed_wav = sdir + "emergency_stiffness"+wav;
static const std::string stiffness_enabled_wav = nbsdir + "stiffness_enabled"+wav;
static const std::string warning_wav = sdir + "warning"+wav;
static const std::string falling_wav = nbsdir +"falling"+wav;
static const std::string wifi_restart_wav = nbsdir +"wifi_restart"+wav;
static const std::string dot = ".";

}
}
}
}

#endif //_SOUND_PATHS_H_
