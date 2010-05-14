package TOOL.GUI;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.Component;
import java.awt.event.*;
public class PopupList extends Component {

    protected JDialog dialog;
    protected JOptionPane optionPane;
    protected JList list;
    protected Component parent;
    protected String title;

    public PopupList(Component parent, String title) {
        super();
        this.title = title;
        this.parent = parent;
        setupComponents(new String[] {"Empty"});
    }

    private void setupComponents(Object[] args) {
        optionPane = new JOptionPane(title, JOptionPane.PLAIN_MESSAGE,
                                    JOptionPane.DEFAULT_OPTION,
                                    null, null);
        list = new JList(args);
        list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        optionPane.setOptions(new Object[] {list});
    }

    public void setList(Object[] args) {
        list = new JList(args);
        optionPane.setOptions(new Object[] {list});
    }


    public void show(int x, int y) {
        dialog = optionPane.createDialog(parent, title);
        dialog.setLocation(x, y);
        dialog.setVisible(true);
    }

    class ListListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {

        }
    }


}
