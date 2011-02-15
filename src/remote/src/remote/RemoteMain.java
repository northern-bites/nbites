/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package remote;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

/**
 *
 * @author jgmorris
 */
public class RemoteMain {

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        try {
            // Set System L&F
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (ClassNotFoundException excp) {
        } catch (InstantiationException exc) {
        } catch (IllegalAccessException exc) {
        } catch (UnsupportedLookAndFeelException exc) {
        }
        new RemoteController();
    }
}
