#ifndef _defined_ClickableButton_h
#define _defined_ClickableButton_h

/**
 * Class to track when a button is pushed or not, and report clicks accordingly
 *
 * Note that getAndClearNumClicks() should be called if any action is taken
 * based on the number of clicks, since otherwise the click will still be
 * visible on the next iteration.
 *
 * Also, if two behavior modules in different threads are both fighting
 * for to perform an action on a given click number, then
 * the behavior of this class is undetermined
 *
 */

class ClickableButton {
public:
    ClickableButton(int fps); //Assumes 100 FPS
    ~ClickableButton();

    void updateFrame(bool buttonValue);

    int peekNumClicks() const { return numClicks; };
    int getAndClearNumClicks() const;
    float getClickLength() const  //in seconds
        {return static_cast<float>(buttonOnCounter) / static_cast<float>(fps);}

private:
    void setNumClicks(int _numClicks) { numClicks = _numClicks; };

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
    static const int NO_CLICKS;
    static const int SINGLE_CLICK_ACTIVE_MIN;
    static const int SINGLE_CLICK_ACTIVE_MAX;
    static const int SINGLE_CLICK_INACTIVE_MIN;
    static const int SINGLE_CLICK_INACTIVE_MAX;
    static const int SHUTDOWN_THRESH;

    bool registeredClickThisTime;
};

#endif
