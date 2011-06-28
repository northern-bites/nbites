
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "man/memory/RoboImage.h"
#include "man/memory/MObject.h"
#include "man/memory/parse/ImageParser.h"
#include "man/include/io/FileFDProvider.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace man::memory;
using namespace man::memory::parse;
using namespace man::include::io;
using namespace boost;

int main(int argc, char** argv) {

  if (argc < 2) {
    printf("Usage: %s /path/to/log/folder\n", argv[0]);
	return -1;
  }

  string folder_path(argv[1]);

  shared_ptr<RoboImage> roboImage(new RoboImage());
  FileFDProvider::ptr fdp(
	  new FileFDProvider(folder_path + "/Image.log"));

  if (fdp->getFileDescriptor() < 0) {
	  printf("Sorry, could not open the file");
	  return -1;
  }

  ImageParser imageParser(fdp, roboImage);

  int i = 1;

  while(!imageParser.getNext()) {
	  string number = boost::lexical_cast<string>(i);
	  string frame_path = folder_path + "/" + number  + ".FRM";
	  int fd = open(frame_path.c_str(),
					O_WRONLY | O_CREAT,
					S_IRWXU | S_IRWXG | S_IRWXO);
	  if (fd > 0) {
		  printf("%s\n", frame_path.c_str());
		  write(fd, roboImage->getImage(), roboImage->getByteSize());
		  close(fd);
		  i++;
	  }
  }
  return 0;
}
