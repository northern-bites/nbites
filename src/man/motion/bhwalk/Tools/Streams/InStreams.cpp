/**
* @file InStreams.cpp
*
* Implementation of in stream classes.
*
* @author Thomas Röfer
* @author Martin Lötzsch
*/

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "InStreams.h"
#include "Platform/BHAssert.h"
#include "Platform/File.h"
#include "Tools/Debugging/Debugging.h"

void StreamReader::skipData(int size, PhysicalInStream& stream)
{
  // default implementation
  char* dummy = new char[size];
  readData(dummy, size, stream);
  delete [] dummy;
}

void PhysicalInStream::skipInStream(int size)
{
  // default implementation
  char* dummy = new char[size];
  readFromStream(dummy, size);
  delete [] dummy;
}

void InText::readString(std::string& value, PhysicalInStream& stream)
{
  value = "";
  skipWhitespace(stream);
  bool containsSpaces = theChar == '"';
  if(containsSpaces && !isEof(stream))
    nextChar(stream);
  while(!isEof(stream) && (containsSpaces || !isWhitespace()) && (!containsSpaces || theChar != '"'))
  {
    if(theChar == '\\')
    {
      nextChar(stream);
      if(theChar == 'n')
        theChar = '\n';
      else if(theChar == 'r')
        theChar = '\r';
      else if(theChar == 't')
        theChar = '\t';
    }
    value += theChar;
    if(!isEof(stream))
      nextChar(stream);
  }
  if(containsSpaces && !isEof(stream))
    nextChar(stream);
  skipWhitespace(stream);
}

void InText::readData(void* p, int size, PhysicalInStream& stream)
{
  for(int i = 0; i < size; ++i)
    readChar(*((char*&) p)++, stream);
}

bool InText::isWhitespace()
{
  return theChar == ' ' || theChar == '\n' || theChar == '\r' || theChar == '\t';
}

void InText::skipWhitespace(PhysicalInStream& stream)
{
  while(!isEof(stream) && isWhitespace())
    nextChar(stream);
}

void InText::readChar(char& d, PhysicalInStream& stream)
{
  int i;
  readInt(i, stream);
  d = (char) i;
}

void InText::readUChar(unsigned char& d, PhysicalInStream& stream)
{
  unsigned u;
  readUInt(u, stream);
  d = (unsigned char) u;
}

void InText::readShort(short& d, PhysicalInStream& stream)
{
  int i;
  readInt(i, stream);
  d = (short) i;
}

void InText::readUShort(unsigned short& d, PhysicalInStream& stream)
{
  unsigned u;
  readUInt(u, stream);
  d = (unsigned short) u;
}

void InText::readInt(int& d, PhysicalInStream& stream)
{
  skipWhitespace(stream);
  int sign = 1;
  if(!isEof(stream) && theChar == '-')
  {
    sign = -1;
    nextChar(stream);
  }
  unsigned u;
  readUInt(u, stream);
  d = sign * (int) u;
}

void InText::readUInt(unsigned int& d, PhysicalInStream& stream)
{
  buf = "";
  skipWhitespace(stream);
  while(!isEof(stream) && isdigit(theChar))
  {
    buf += theChar;
    nextChar(stream);
  }
  d = (unsigned) strtoul(buf.c_str(), (char**)NULL, 0);
  skipWhitespace(stream);
}

void InText::readFloat(float& d, PhysicalInStream& stream)
{
  double f;
  readDouble(f, stream);
  d = (float) f;
}

void InText::readDouble(double& d, PhysicalInStream& stream)
{
  buf = "";
  skipWhitespace(stream);
  if(!isEof(stream) && theChar == '-')
  {
    buf += theChar;
    nextChar(stream);
  }
  while(!isEof(stream) && isdigit(theChar))
  {
    buf += theChar;
    nextChar(stream);
  }
  if(!isEof(stream) && theChar == '.')
  {
    buf += theChar;
    nextChar(stream);
  }
  while(!isEof(stream) && isdigit(theChar))
  {
    buf += theChar;
    nextChar(stream);
  }
  if(!isEof(stream) && (theChar == 'e' || theChar == 'E'))
  {
    buf += theChar;
    nextChar(stream);
  }
  if(!isEof(stream) && theChar == '-')
  {
    buf += theChar;
    nextChar(stream);
  }
  while(!isEof(stream) && isdigit(theChar))
  {
    buf += theChar;
    nextChar(stream);
  }
  d = atof(buf.c_str());
  skipWhitespace(stream);
}

void InText::readBool(bool& value, PhysicalInStream& stream)
{
  skipWhitespace(stream);
  if(!isEof(stream))
  {
    if(theChar == '0' || theChar == '1')
    {
      value = theChar != '0';
      nextChar(stream);
    }
    else
    {
      value = theChar != 'f';
      static const char* falseString = "false";
      static const char* trueString = "true";
      const char* p = value ? trueString : falseString;
      while(!isEof(stream) && *p && theChar == *p)
      {
        ++p;
        nextChar(stream);
      }
    }
  }
}

void InConfig::create(const std::string& sectionName, PhysicalInStream& stream)
{
  if(stream.exists() && sectionName != "")
  {
    std::string fileEntry;
    std::string section = std::string("[") + sectionName + "]";

    while(!isEof(stream))
    {
      readString(fileEntry, stream);
      if(fileEntry == section)
      {
        if(theChar == '[') // handle empty section
          while(!isEof(stream))
            InText::nextChar(stream);
        break;
      }
    }
    readSection = true;
  }
}

bool InConfig::isWhitespace()
{
  return (theChar == '/' && (theNextChar == '*' || theNextChar == '/')) ||
         theChar == '#' || InText::isWhitespace();
}

void InConfig::skipWhitespace(PhysicalInStream& stream)
{
  while(!isEof(stream) && isWhitespace())
  {
    while(!isEof(stream) && InText::isWhitespace())
      nextChar(stream);
    if(!isEof(stream))
    {
      if(theChar == '/' && theNextChar == '/')
      {
        skipLine(stream);
      }
      else if(theChar == '/' && theNextChar == '*')
      {
        skipComment(stream);
      }
      else if(theChar == '#')
      {
        skipLine(stream);
      }
    }
  }
}

void InConfig::nextChar(PhysicalInStream& stream)
{
  InText::nextChar(stream);
  if(readSection && theChar == '[')
    while(!isEof(stream))
      InText::nextChar(stream);
}

void InConfig::skipLine(PhysicalInStream& stream)
{
  while(!isEof(stream) && theChar != '\n')
    nextChar(stream);
  if(!isEof(stream))
    nextChar(stream);
}

void InConfig::skipComment(PhysicalInStream& stream)
{
  // skip /*
  nextChar(stream);
  nextChar(stream);
  while(!isEof(stream) && (theChar != '*' || theNextChar != '/'))
    nextChar(stream);

  // skip */
  if(!isEof(stream))
    nextChar(stream);
  if(!isEof(stream))
    nextChar(stream);
}

InFile::~InFile()
{ if(stream != 0) delete stream; }
bool InFile::exists() const
{ return (stream != 0 ? stream->exists() : false); }
bool InFile::getEof() const
{ return (stream != 0 ? stream->eof() : false); }
void InFile::open(const std::string& name)
{ if(stream == 0) stream = new File(name, "rb"); }
void InFile::readFromStream(void* p, int size)
{ if(stream != 0) stream->read(p, size); }

void InMemory::readFromStream(void* p, int size)
{
  if(memory != 0)
  {
    memcpy(p, memory, size);
    memory += size;
  }
}

void InMap::parse(In& stream, const std::string& name)
{
  map = new SimpleMap(stream, name);
  this->name = name;
  stack.reserve(20);
}

void InMap::printError(const std::string& msg)
{
  std::string path = "";
  for(std::vector<Entry>::const_iterator i = stack.begin(); i != stack.end(); ++i)
  {
    if(i->key)
    {
      if(path != "")
        path += '.';
      path += i->key;
    }
    else
    {
      char buf[20];
      sprintf(buf, "[%d]", i->type);
      path += buf;
    }
  }
  OUTPUT_ERROR(name << (name == "" || path == "" ? "" : ", ") <<
               path << (name == "" && path == "" ? "" : ": ") << msg);
}

void InMap::inChar(char& value)
{
  Entry& e = stack.back();
  if(e.value)
  {
    const SimpleMap::Literal* literal = dynamic_cast<const SimpleMap::Literal*>(e.value);
    if(literal)
    {
      In& stream = *literal;
      int i;
      stream >> i;
      value = (char) i;
      if(!stream.eof())
        printError("wrong format");
    }
    else
      printError("literal expected");
  }
}

void InMap::inUChar(unsigned char& value)
{
  Entry& e = stack.back();
  if(e.value)
  {
    const SimpleMap::Literal* literal = dynamic_cast<const SimpleMap::Literal*>(e.value);
    if(literal)
    {
      In& stream = *literal;
      if(e.enumToString)
      {
        std::string s;
        stream >> s;
        for(int i = 0; e.enumToString(i); ++i)
          if(s == e.enumToString(i))
          {
            value = (unsigned char) i;
            return;
          }
        std::string t;
        for(int i = 0; e.enumToString(i); ++i)
          t += std::string("'") + e.enumToString(i) + "', ";
        printError("expected one of " + t + "found '" + s + "'");
        return;
      }
      else
      {
        unsigned i;
        stream >> i;
        value = (unsigned char) i;
        if(!stream.eof())
          printError("wrong format");
      }
    }
    else
      printError("literal expected");
  }
}

void InMap::inInt(int& value)
{
  Entry& e = stack.back();
  if(e.value)
  {
    const SimpleMap::Literal* literal = dynamic_cast<const SimpleMap::Literal*>(e.value);
    if(literal)
    {
      In& stream = *literal;
      stream >> value;
      if(!stream.eof())
        printError("wrong format");
    }
    else
      printError("literal expected");
  }
}

void InMap::inUInt(unsigned int& value)
{
  Entry& e = stack.back();
  if(e.type == -1)
  {
    if(e.value)
    {
      const SimpleMap::Array* array = dynamic_cast<const SimpleMap::Array*>(e.value);
      if(array)
        value = array->size();
      else
        printError("array expected");
    }
    else
      value = 0;
  }
  else if(e.value)
  {
    const SimpleMap::Literal* literal = dynamic_cast<const SimpleMap::Literal*>(e.value);
    if(literal)
    {
      In& stream = *literal;
      stream >> value;
      if(!stream.eof())
        printError("wrong format");
    }
    else
      printError("literal expected");
  }
}

void InMap::read(void* p, int size)
{
  ASSERT(false);
}

void InMap::skip(int size)
{
  ASSERT(false);
}

void InMap::select(const char* name, int type, const char * (*enumToString)(int))
{
  ASSERT(map);
  ASSERT(name || type >= 0);

  Streaming::trimName(name);
  const SimpleMap::Value* value = stack.empty() ? (const SimpleMap::Value*) *map : stack.back().value;
  if(!value) // invalid
    stack.push_back(Entry(name, 0, type, enumToString)); // add more invalid
  else if(type >= 0) // array element
  {
    const SimpleMap::Array* array = dynamic_cast<const SimpleMap::Array*>(value);
    if(array)
    {
      if(type < (int) array->size())
        stack.push_back(Entry(name, (*array)[type], type, enumToString));
      else
      {
        printError("array index out of range");
        stack.push_back(Entry(name, 0, type, enumToString)); // add invalid
      }
    }
    else
    {
      printError("array expected");
      stack.push_back(Entry(name, 0, type, enumToString)); // add invalid
    }
  }
  else // record element
  {
    const SimpleMap::Record* record = dynamic_cast<const SimpleMap::Record*>(value);
    if(record)
    {
      SimpleMap::Record::const_iterator i = record->find(name);
      if(i != record->end())
        stack.push_back(Entry(name, i->second, type, enumToString));
      else
      {
        printError(std::string("attribute '") + name + "' not found");
        stack.push_back(Entry(name, 0, type, enumToString)); // add invalid
      }
    }
    else
    {
      printError("record expected");
      stack.push_back(Entry(name, 0, type, enumToString)); // add invalid
    }
  }
}

void InMap::deselect()
{
  stack.pop_back();
}

InMapFile::InMapFile(const std::string& name) :
  stream(name)
{
  if(stream.exists())
    parse(stream, name);
}

InMapMemory::InMapMemory(const void* memory, unsigned size) :
  stream(memory, size)
{
  parse(stream);
}
