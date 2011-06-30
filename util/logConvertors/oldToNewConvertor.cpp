
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "man/memory/RoboImage.h"
#include "man/memory/MObject.h"
#include "man/memory/log/ImageLogger.h"
#include "man/include/io/FileFDProvider.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace man::memory;
using namespace man::memory::log;
using namespace man::include::io;
using namespace boost;

//HACK! but the image logger expects to have this declared in Memory.cpp
namespace man {
	namespace memory {
		unsigned long long birth_time = 0;
	}
}

int main(int argc, char** argv) {

  if (argc < 2) {
    printf("Usage: %s /path/to/log/folder\n", argv[0]);
	return -1;
  }

  string folder_path(argv[1]);

  shared_ptr<RoboImage> roboImage(new RoboImage());
  FileFDProvider::ptr fdp(
	  new FileFDProvider(folder_path + "/Image.log", NEW));

  if (fdp->getFileDescriptor() < 0) {
	  printf("Sorry, could not open the folder");
	  return -1;
  }

  ImageLogger imageLogger(fdp, MIMAGE_ID, roboImage);

  unsigned char* buffer = new unsigned char[roboImage->getByteSize()];
  roboImage->updateImage(buffer);

  for (int i = 1; i < 3000; i++) {
	  string number = boost::lexical_cast<string>(i);
	  string frame_path = folder_path + "/" + number  + ".FRM";
	  int fd = open(frame_path.c_str(), O_RDONLY);
	  if (fd > 0) {
		  printf("%s\n", frame_path.c_str());
		  read(fd, buffer, roboImage->getByteSize());
		  imageLogger.writeToLog();
		  close(fd);
	  }
  }
  return 0;
}
