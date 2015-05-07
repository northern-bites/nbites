package nbtool.gui;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import nbtool.data.*;
import nbtool.util.*;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;
import nbtool.io.CommandIO;

import nbtool.util.NBConstants.*;


public class StreamingPanel extends JPanel implements ActionListener, NListener {
	
	static final long serialVersionUID = 1L;

	public StreamingPanel() {
		/*
		 GUI is done below. 
		 TODO: transfer to actual layout manager
		 */
		super();
		//setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		canvas = new JPanel();
		//canvas.setLayout(new BoxLayout(canvas,BoxLayout.Y_AXIS));
		canvas.setLayout(null);
		
		topCameraPrefs = new CameraPrefs("Top Camera");
		bottomCameraPrefs = new CameraPrefs("Bottom Camera");
		
		canvas.add(topCameraPrefs);
		canvas.add(bottomCameraPrefs);
		
		startStreaming = new JButton("Start");
		startStreaming.addActionListener(this);
		startStreaming.setPreferredSize(new Dimension(60,20));
		canvas.add(startStreaming);
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		sp.getVerticalScrollBar().setUnitIncrement(16);
		sp.setViewportView(canvas);

		add(sp);
		
	}
	
	private void useSize(Dimension s) {
		sp.setBounds(0, 0, s.width, s.height);
		Dimension d1, d2, d3;
		int y=0;
		int x = 0;
		d1 = topCameraPrefs.getPreferredSize();
		topCameraPrefs.setBounds(x,y,d1.width,d1.height);
		y += d1.height+3;
		
		d2 = bottomCameraPrefs.getPreferredSize();
		bottomCameraPrefs.setBounds(x,y,d2.width,d2.height);
		y += d2.height+3;
		
		d3 = startStreaming.getPreferredSize();
		startStreaming.setBounds(2,y,d3.width,d3.height);
		y += d3.height+3;
		
		canvas.setPreferredSize(new Dimension(250,y));
	}
	
	private JScrollPane sp;
	private JPanel canvas;
	
	private JButton startStreaming;
	
	private CameraPrefs topCameraPrefs;
	private CameraPrefs bottomCameraPrefs;
	
	public Integer[] topCameraParams;
	public Integer[] bottomCameraParams;

	@Override
	public void actionPerformed(ActionEvent e) {
		// TODO Auto-generated method stub
		if(e.getSource() == startStreaming) {
			/*gets all params except for BackLight-Compensation
			 0x00 for both cameras
			 */
			topCameraParams = topCameraPrefs.getParameterValues();
			bottomCameraParams = bottomCameraPrefs.getParameterValues();
			tryStart(topCameraParams, bottomCameraParams);
		}
	}

	@Override
	public void notified(EVENT e, Object src, Object... args) {
		// TODO Auto-generated method stub
		
	}
	
	private void tryStart(Integer[] topCamera, Integer[] bottomCamera) {
		
	}
	
	private void useStatus(STATUS s) {
		switch (s) {
		case IDLE:
			startStreaming.setText("Start");
			startStreaming.setEnabled(true);
			break;
		case RUNNING:
			startStreaming.setText("Running");
			startStreaming.setEnabled(false);
			break;
		default:
			break;
		}
	}
	
}
