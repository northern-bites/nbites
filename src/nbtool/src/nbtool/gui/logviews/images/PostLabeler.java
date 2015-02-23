package nbtool.gui.logviews.images;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.File;
import java.io.IOException;
import java.util.Map;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JToggleButton;

import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.FileIO;
import nbtool.util.U;

public class PostLabeler extends ViewParent implements MouseListener, ItemListener, ActionListener {

	ImagePanel imgPanel;
	JToggleButton jtb;
	JButton save;
	boolean rightPost;
	
	public PostLabeler() {
		super();
				
		addMouseListener(this);
		
		imgPanel = new ImagePanel();
		
		jtb = new JToggleButton("Label right post");
		jtb.addItemListener(this);
		
		save = new JButton("save");
		save.addActionListener(this);
				
		add(imgPanel);
		add(jtb);
		add(save);
	}
	
	@Override
	public void setLog(Log newlog) {
		log = newlog;
		imgPanel.setLog(U.biFromLog(newlog));
		
		repaint();
	}
	
	@Override
	public void mouseClicked(MouseEvent e) {
		String name = rightPost ? "rightPost" : "singlePost";
		log.addAttribute(name, Integer.toString(e.getX()));
		Map<String, String> map = log.getAttributes();
		System.out.println(map.get("singlePost"));
		System.out.println(map.get("rightPost"));
	}

	@Override
	public void mouseEntered(MouseEvent e) {}

	@Override
	public void mouseExited(MouseEvent e) {}

	@Override
	public void mousePressed(MouseEvent e) {}

	@Override
	public void mouseReleased(MouseEvent e) {}

	@Override
	public void itemStateChanged(ItemEvent e) {
		rightPost = e.getStateChange() == ItemEvent.SELECTED ? true : false;
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		int rVal = FileIO.chooser.showSaveDialog(this);
		
		if (rVal == JFileChooser.APPROVE_OPTION) {
			File f = FileIO.chooser.getSelectedFile();
			if (f.isDirectory()) {
				U.w("Cannot overwrite directory with log.");
			}
			
			String aPath = f.getAbsolutePath();
			if (!aPath.endsWith(".nblog"))
				aPath = aPath + ".nblog";
			
			U.w("Writing log to: " + aPath);
			
			try {
				FileIO.writeLogToPath(log, aPath);
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}		
	}
}