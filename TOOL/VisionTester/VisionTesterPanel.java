
package TOOL.VisionTester;

import TOOL.TOOL;
import TOOL.TOOLModule;

import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JTextPane;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.AbstractAction;
import javax.swing.ActionMap;
import javax.swing.text.*;

import java.awt.GridLayout;

public class VisionTesterPanel extends JPanel {

	private VisionTester visionTester;

	private JButton runSetObjCount, runVisionEval;

	private JTextPane frameInfo;

	public VisionTesterPanel(VisionTester vt){
		super();
		visionTester = vt;

		setupWindow();
		setupButtons();
		setupListeners();
	}

	private void setupWindow(){
		setLayout(new GridLayout(0,2));

		frameInfo = new JTextPane();
		frameInfo.setText("Click to run test");
		frameInfo.setBackground(this.getBackground());
		frameInfo.setEditable(false);

		SimpleAttributeSet set = new SimpleAttributeSet();
		StyleConstants.setAlignment(set,StyleConstants.ALIGN_CENTER);
		frameInfo.setParagraphAttributes(set, true);

		add(frameInfo);
	}

	private void setupButtons(){

		JPanel buttonGrid = new JPanel(new GridLayout(2,1));

		runSetObjCount = new JButton("Total Object Counts");
		runSetObjCount.setFocusable(false);


		runVisionEval = new JButton("Object Correctness Counts");
		runVisionEval.setFocusable(false);
		runVisionEval.setEnabled(false);

		buttonGrid.add(runSetObjCount);
		buttonGrid.add(runVisionEval);

		add(buttonGrid);
		fixButtons();
	}

	private void setupListeners(){

		runSetObjCount.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e) {
					visionTester.runSetObjCount();
				}
			});
	}

	public void fixButtons(){
		runSetObjCount.setEnabled( visionTester.canRunCount());
	}
}