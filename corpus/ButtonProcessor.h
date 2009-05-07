#ifndef _ButtonProcessor_h 
#define _ButtonProcessor_h

#include <boost/shared_ptr.hpp>
#include "ClickableButton.h"


enum  ButtonID {
    CHEST_BUTTON = 0,
    LEFT_FOOT_BUTTON,
    RIGHT_FOOT_BUTTON
};

class ButtonProcessor{
public:
    ButtonProcessor(){}

    virtual ~ButtonProcessor(){}

    virtual boost::shared_ptr<ClickableButton> getButton(ButtonID)const = 0;

};



#endif
