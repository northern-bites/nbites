/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package remote;

import java.io.IOException;
import java.util.Timer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author jack
 */
public class TerminalDisplayController extends Thread{
    RemoteModel model;
    TerminalDisplayView view;

    public TerminalDisplayController(RemoteModel model){
        this.model = model;
        view = new TerminalDisplayView(model);
        view.setVisible(true);
        this.start();
    }

    public void run(){
        try {
            while(true){
                if (model.isOutputAvailable() &&
                    model.getStdout().available() > 0) {
                            view.update();
                }
                sleep(100);
            }
        } catch (InterruptedException ex) {
            Logger.getLogger(TerminalDisplayController.
                             class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(TerminalDisplayController.
                             class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
