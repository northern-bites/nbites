#pragma once

template<class Archive>
void Pose::save(Archive &ar, const unsigned int file_version) const
{
   /* The output versions of serializeMembers are const */
   Pose *unconsted = const_cast<Pose *>(this);
   unconsted->serializeMembers(ar, file_version);
}

template<class Archive>
void Pose::load(Archive &ar, const unsigned int file_version)
{
   serializeMembers(ar, file_version);
   makeConstants();
}

template<class Archive>
void Pose::serializeMembers(Archive &ar, const unsigned int file_version)
{
   ar & topCameraToWorldTransform;
   ar & botCameraToWorldTransform;

   if (file_version < 2) {
/*
      // throw away a container of 16 floats
      Garbage<16, float> c2w;
      ar & c2w;
      // the tvmet vectors were containers of 4 floats each and serialized as such
      Garbage<4, float> v;
      ar & v;
      origin = vec4(v.garbage[0], v.garbage[1], v.garbage[2], v.garbage[3]);
      ar & v;
      zunit = vec4(v.garbage[0], v.garbage[1], v.garbage[2], v.garbage[3]);
      ar & v;
      corigin = vec4(v.garbage[0], v.garbage[1], v.garbage[2], v.garbage[3]);
*/
   } else {
      ar & origin;
      ar & zunit;
      ar & topCOrigin;
      ar & botCOrigin;
   }
   ar & horizon;
   ar & topExclusionArray;
   ar & botExclusionArray;
}

