#ifndef _Speech_h_DEFINED
#define _Speech_h_DEFINED

#include <string>

class Speech
{
public:
    Speech() : isEnabled(true){ };
    virtual ~Speech() { };

public:
    virtual void say(const std::string& text) = 0;
    virtual void  setVolume(float v) = 0;
    virtual float getVolume()      = 0;
    virtual void enable()  { isEnabled = true;  }
    virtual void disable() { isEnabled = false; }

protected:
    bool isEnabled;
};

#endif /* _Speech_h_DEFINED */
