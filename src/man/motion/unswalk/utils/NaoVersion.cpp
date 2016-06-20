#include "NaoVersion.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include "utils/Logger.hpp"

NaoVersion naoVersion = nao_unset;

NaoVersion determineNaoVersion(void)
{
   struct stat dummy;

   bool stat_video0 = stat("/dev/video0", &dummy);
   bool stat_video1 = stat("/dev/video1", &dummy);

   if (stat_video0 && stat_video1) {
      naoVersion = nao_v4;
   } else if (stat_video0) {
      naoVersion = nao_v3;
   } else {
      llog(WARNING) << "DetermineNaoVersion: Could not determine robot version."
                    << std::endl
                    << "DetermineNaoVersion: Assuming latest version "
                    << nao_latest << "." << std::endl;
      naoVersion = nao_latest;
   }
   return naoVersion;
}

