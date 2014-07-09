#include "ParamReader.h"
#include <iostream>

namespace man{

ParamReader::ParamReader(std::string filename)
{
    boost::property_tree::read_json(filename, tree);
}
}
