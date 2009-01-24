package TOOL.ColorEdit;

import javax.swing.JPanel;
import javax.swing.JCheckBox;
import javax.swing.JTextArea;
import javax.swing.JRadioButton;
import javax.swing.JTextArea;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import javax.swing.JSlider;
import javax.swing.JButton;
import javax.swing.ButtonGroup;
import javax.swing.BoxLayout;
import TOOL.Calibrate.ColorSwatchPanel;
import TOOL.GUI.IncrementalSlider;


public class ColorEditPanel extends JPanel implements KeyListener {
       
    private JTextArea echo;
    private final ColorEdit editor;

    private final JCheckBox edit, allView;

    private ColorSwatchPanel buttons;
    private JRadioButton yView, uView, vView, colorView;
    private JButton refresh, undo, redo, fillHoles;
    private IncrementalSlider slider;

    public static final String Y_STRING = "Y value";
    public static final String U_STRING = "U value";
    public static final String V_STRING = "V value";
    

    public static final int DEFAULT_BUTTON_SIZE = 40;

    /**
     * Initialize the panel, create components.
     * We set focusable false so that clicking on a button doesn't consume
     * keyboard focus; if it does then our hotkeys cease to work.
     */
    public ColorEditPanel(ColorEdit editor) {
	super();
	this.editor = editor;

	echo = new JTextArea();
        echo.setEditable(false);
        echo.setFocusable(false);
        echo.setBackground(getBackground());

	edit = new JCheckBox("Edit Mode: ");
        edit.setFocusable(false);
        allView = new JCheckBox("All view: ");

        slider = new IncrementalSlider(editor, Y_STRING, 0, 255);
        
       	buttons = new ColorSwatchPanel(editor, DEFAULT_BUTTON_SIZE);

	ButtonGroup views = new ButtonGroup();
	yView = new JRadioButton("Y view");
        yView.setFocusable(false);
	uView = new JRadioButton("U view");
        uView.setFocusable(false);
	vView = new JRadioButton("V view");
        vView.setFocusable(false);
        colorView = new JRadioButton("Color entry view");
        colorView.setFocusable(false);

        refresh = new JButton("Refresh");
        refresh.setFocusable(false);
        undo = new JButton("Undo");
        undo.setFocusable(false);
        redo = new JButton("Redo");
        undo.setFocusable(false);
        fillHoles = new JButton("Fills holes");
        fillHoles.setFocusable(false);

        yView.setSelected(true);

        JPanel editButtons = new JPanel();
        editButtons.add(fillHoles);
        editButtons.add(refresh);
        editButtons.add(undo);
        editButtons.add(redo);

	
	addListeners();


	views.add(yView);
	views.add(uView);
	views.add(vView);
        views.add(colorView);

        JPanel theViews = new JPanel();
        theViews.add(yView);
        theViews.add(uView);
        theViews.add(vView);
        theViews.add(allView);
        theViews.add(colorView);
        
        JPanel master = new JPanel();
        master.setLayout(new BoxLayout(master, BoxLayout.PAGE_AXIS));
        master.add(echo);
        master.add(edit);
        master.add(allView);
        master.add(editButtons);
        master.add(theViews);
        master.add(slider);
        master.add(buttons);
        add(master);
	
    }


    public void setColor(byte color) {
        buttons.setSelected(color);
    }

    private void addListeners() {

        // Checkbox listeners
	edit.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.setEditing(edit.isSelected());
		}
	    });

        allView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.setAllView(allView.isSelected());
                    
                }
                
	    });

	// Add action listeners..
	yView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.setView(ColorEdit.View.YVIEW);
                    slider.setEnabled(true);
                    slider.setText(Y_STRING);
                    allView.setEnabled(true);
                    edit.setEnabled(true);
		}
	    });
	uView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.setView(ColorEdit.View.UVIEW);
                    slider.setEnabled(true);
                    slider.setText(U_STRING);
                    allView.setEnabled(true);
                    edit.setEnabled(true);
		}
	    });
	vView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.setView(ColorEdit.View.VVIEW);
                    slider.setEnabled(true);
                    slider.setText(V_STRING);
                    allView.setEnabled(true);
                    edit.setEnabled(true);
		}
	    });
	
        
	colorView.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.setView(ColorEdit.View.COLORVIEW);
                    slider.setEnabled(false);
                    allView.setEnabled(false);
                    edit.setEnabled(false);
		}
	    });
        fillHoles.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    editor.fillHoles();
                }
            });

        refresh.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.refresh();
                    
		}
	    });
        undo.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.undo();
                    
		}
	    });
        redo.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    editor.redo();
                    
		}
            });
    }

    public void keyTyped(KeyEvent e) {}
    public void keyPressed(KeyEvent e) {
        // Only act if we're currently visible on the screen
        if (!isShowing()) { return; }
        buttons.keyPressed(e);

        if (e.getKeyCode() == KeyEvent.VK_E) {
            edit.setSelected(!edit.isSelected());
            editor.setEditing(edit.isSelected());
        }
        
        else if (e.getKeyCode() == KeyEvent.VK_R) {
            editor.refresh();
        }

        else if (e.getKeyCode() == KeyEvent.VK_A) {
            allView.setSelected(!allView.isSelected());
            editor.setAllView(allView.isSelected());
        }

	else if (e.getKeyCode() == KeyEvent.VK_Z) {
            editor.undo();
        }

	else if (e.getKeyCode() == KeyEvent.VK_Y) {
	    editor.redo();
	}
    }
    public void keyReleased(KeyEvent e) {}

    
    /**
     * Updates the range of the slider; after the change we notify
     * the editor of the new slider value.
     */
    public void setRange(int min, int max){
        slider.setRange(min, max);
        editor.setValue(slider.getValue());
    }

    public void setSliderValue(int val) {
        slider.setValue(val);
    }
    
    public void setText(String s) {
	echo.setText(s);
    }

    public ColorSwatchPanel getSwatches() {
        return buttons;
    }

    public void pressMinus() {
        slider.pressMinus();
    }
    
    public void pressPlus() {
        slider.pressPlus();
    }



}
