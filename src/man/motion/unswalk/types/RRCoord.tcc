template<class Archive>
void RRCoord::serialize(Archive &ar, const unsigned int file_version) {
   ar & vec;
   ar & var;
}

#include "types/boostSerializationEigenTypes.hpp"
