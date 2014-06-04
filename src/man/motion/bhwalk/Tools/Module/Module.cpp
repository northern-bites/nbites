/**
* @file ModuleBH.cpp
* The class attributes of the module handling schema.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "Module.h"
#include "Tools/Streams/InStreams.h"

std::list<Requirements::Entry>* Requirements::entries = 0;
std::list<Representations::Entry>* Representations::entries = 0;
ModuleBase* ModuleBase::first = 0;
std::string ModuleBase::config_path;

void Requirements::add(const char* name, void (*create)(), void (*free)(), void (*in)(In&))
{
  if(entries)
    entries->push_back(Entry(name, create, free, in));
}

void Representations::add(const char* name, void (*update)(Blackboard&), void (*create)(), void (*free)(), void (*out)(Out&))
{
  if(entries)
    entries->push_back(Entry(name, update, create, free, out));
}

void loadModuleParameters(Streamable& parameters, const char* moduleName, const char* fileName)
{
  std::string name;
  if(!fileName)
  {
    name = ModuleBase::config_path;
    name += moduleName;
    name[0] = (char) tolower(name[0]);
    if(name.size() > 1 && isupper(name[1]))
      for(int i = 1; i + 1 < (int) name.size() && isupper(name[i + 1]); ++i)
        name[i] = (char) tolower(name[i]);
    name += ".cfg";
  }
  else
  {
    name = fileName;
  }
  //std::cout << name << std::endl;
  InMapFile stream(name);
  ASSERT(stream.exists());
  stream >> parameters;
}
