
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _defined_ClickableButton_h
#define _defined_ClickableButton_h


#include "pthread.h"
#include "Common.h"

/**
 * Class to track when a button is pushed or not, and report clicks accordingly
 *
 * Note that getAndClearNumClicks() should be called if any action is taken
 * based on the number of clicks, since otherwise the click will still be
 * visible on the next iteration.
 *
 * Also, if two behavior modules in different threads are both fighting
 * for to perform an action on a given click number, then
 * the behavior is undetermined
 *
 */

class ClickableButton {
public:
    ClickableButton(int fps); //Assumes 100 FPS
    ~ClickableButton();

    void updateFrame(float buttonValue);

    int peekNumClicks() const { return numClicks; };
    int getAndClearNumClicks() const;
    float getClickLength() const  //in seconds
        {return static_cast<float>(buttonOnCounter) / static_cast<float>(fps);}

private:
    void setNumClicks(int _numClicks) ;

public:
    static const int NO_CLICKS;

private:
    int fps;

    int buttonOnCounter;
    int buttonOffCounter;
    int lastButtonOnCounter;
    int lastButtonOffCounter;
    //Stores a running count of how many clicks we think we may have gotten:
    int buttonClicks;

    mutable int numClicks; //Stores the finalized 'public' number of clicks

//These assume we're running 100 fps
    static const int SINGLE_CLICK_ACTIVE_MIN;
    static const int SINGLE_CLICK_ACTIVE_MAX;
    static const int SINGLE_CLICK_INACTIVE_MIN;
    static const int SINGLE_CLICK_INACTIVE_MAX;
    static const float PUSHED;
    static const int SHUTDOWN_THRESH;

    mutable pthread_mutex_t button_mutex;

    bool registeredClickThisTime;
};

#endif
