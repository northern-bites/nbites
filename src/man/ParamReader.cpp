#include "ParamReader.h"
#include <iostream>
#include <sys/stat.h>

namespace man{

ParamReader::ParamReader(std::string filename)
{
	if(FILE *file = fopen(filename.c_str(),"r")) { //check for file existence
		fclose(file);
		boost::property_tree::read_json(filename, tree);
	}
}
}
