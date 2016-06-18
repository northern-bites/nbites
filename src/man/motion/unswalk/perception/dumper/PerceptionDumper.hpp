#pragma once

#include <string>
#include <fstream>

#include <boost/archive/binary_oarchive.hpp>

class Blackboard;

class PerceptionDumper
{
   public:
      PerceptionDumper(const char *path);
      virtual ~PerceptionDumper();

      void dump(Blackboard *blackboard);

      const std::string &getPath() const;

   private:
      std::string path;
      std::ofstream of;
      boost::archive::binary_oarchive ar;

};
