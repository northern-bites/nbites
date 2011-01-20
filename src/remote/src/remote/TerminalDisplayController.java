/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package remote;

import java.io.IOException;
import java.util.Timer;
import java.util.logging.Level;
import java.util.logging.Logger;
import ch.ethz.ssh2.StreamGobbler;

/**
 * Controls interaction between stream and terminal display view.
 */
public class TerminalDisplayController extends Thread{

    StreamGobbler stream;
    TerminalDisplayView view;

    public TerminalDisplayController(StreamGobbler stream, String host){
        this.stream = stream;
        view = new TerminalDisplayView(stream, host);
        view.setVisible(true);
        this.start();
    }

    public void run(){
        try {
            while(true){
                if (stream != null &&
                    stream.available() > 0) {
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
