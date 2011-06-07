#ifndef _Speech_h_DEFINED
#define _Speech_h_DEFINED

#include <string>

class Speech
{
public:
    virtual ~Speech() { };

public:
    virtual void say(std::string text) {  }

    virtual void  setVolume(float v) {           }
    virtual float getVolume()        { return 0; }

    virtual void enable()  { isEnabled = true;   }
    virtual void disable() { isEnabled = false;  }

protected:
    Speech() : isEnabled(true){ };
    bool isEnabled;
};

#endif /* _Speech_h_DEFINED */
