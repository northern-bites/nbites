package nbtool.gui.logviews.misc;

import java.awt.Dimension;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.JLabel;

import nbtool.data.Log;

public class TestView extends ViewParent{

	@Override
	public void setLog(Log newlog) {
		// TODO Auto-generated method stub
		
	}
	
	public TestView() {
		JLabel labela = new JLabel("THIS IS A NEW VIEW");
		
		add(labela);
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}

			private void useSize(Dimension size) {
				// TODO Auto-generated method stub
				Dimension b = labela.getPreferredSize();
				labela.setBounds(0,0, b.width, b.height);
			}
		});
		setLayout(null);
	}

}
