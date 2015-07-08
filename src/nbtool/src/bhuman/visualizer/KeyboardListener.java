package visualizer;

import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;


/**
 * @author Michel Bartsch
 * 
 * This class listens to the keyboard.
 */
public class KeyboardListener implements KeyEventDispatcher
{
    /** The instance of the visualizer´s gui. */
    GUI gui;
    /** The key that is actually pressed, 0 if no key is pressed. */
    private int pressing = 0;
    
    /**
     * Creates a new KeyboardListener and sets himself to listening.
     */
    public KeyboardListener(GUI gui)
    {
        this.gui = gui;
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(this);
    }
    
    /**
     * This is called every time a key is pressed or released.
     * 
     * @param e     The key that has been pressed or released.
     * 
     * @return If false, the key will be consumed.
     */
    @Override
    public boolean dispatchKeyEvent(KeyEvent e) {
        if (e.getID() == KeyEvent.KEY_RELEASED) {
            pressing = 0;
        } else if (e.getID() == KeyEvent.KEY_PRESSED) {
            int key = e.getKeyCode();
        
            if ((key == 0) || (key == pressing)) {
                return false;
            }
            pressing = key;
            return pressed(key);
        }
        
        return false;
    }
    
    /**
     * This is called once every time a key is pressed. It is called once and
     * not as long as the key is pressed.
     * You can easily set the keys for each action here. The actions are
     * to be performed via the actionPerformed method as they are in the
     * GUI.
     * 
     * @param key  The key that has just been pressed.
     * 
     * @return If false, the key was used and should be consumed.
     */
    private boolean pressed(int key)
    {
        switch (key) {
                case KeyEvent.VK_F10:
                    GameStateVisualizer.exit();
                    break;
                case KeyEvent.VK_F11:
                    gui.toggleTestmode();
                    break;
                default:
                    return false;
            }
        return true;
    }
}