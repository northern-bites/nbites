package nbtool.gui;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import nbtool.data.*;
import nbtool.util.*;
import nbtool.io.ControlIO;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.util.NBConstants.*;
import nbtool.util.Logger;
import messages.CameraParamsOuterClass;
import messages.CameraParamsOuterClass.CameraParams;

import java.io.FileWriter;
import java.io.IOException;
//import org.json.simple.JSONArray;
//import org.json.simple.JSONObject;


public class StreamingPanel extends JPanel implements ActionListener {
	
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
		
		startStreaming = new JButton("Stream");
		startStreaming.addActionListener(this);
		startStreaming.setPreferredSize(new Dimension(80,20));
		canvas.add(startStreaming);

		saveParams = new JButton("Save");
		saveParams.addActionListener(this);
		saveParams.setPreferredSize(new Dimension(60,20));
		canvas.add(saveParams);
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		sp.getVerticalScrollBar().setUnitIncrement(16);
		sp.setViewportView(canvas);

		add(sp);
		
	}
	
	private void useSize(Dimension s) {
		sp.setBounds(0, 0, s.width, s.height);
		Dimension d1, d2, d3, d4;
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

		d4 = saveParams.getPreferredSize();
		saveParams.setBounds(84,y-3-d3.height,d4.width,d4.height);
		y += d4.height+3;
		
		canvas.setPreferredSize(new Dimension(250,y));
	}
	
	private JScrollPane sp;
	private JPanel canvas;
	
	private JButton startStreaming;
	private JButton saveParams;
	
	private CameraPrefs topCameraPrefs;
	private CameraPrefs bottomCameraPrefs;
	
	public Integer[] topCameraParams;
	public Integer[] bottomCameraParams;

	@Override
	public void actionPerformed(ActionEvent e) {
		
		topCameraParams = topCameraPrefs.getParameterValues();
		bottomCameraParams = bottomCameraPrefs.getParameterValues();

		if(e.getSource() == startStreaming) {
			tryStream(topCameraParams, bottomCameraParams);
		} else if(e.getSource() == saveParams) {
			trySave(topCameraParams, bottomCameraParams);
		}
	}
	
	private void tryStream(Integer[] topCameraParams, Integer[] bottomCameraParams) {
		CameraParams topCamera = CameraParams.newBuilder()
				.setWhichCamera("TOP")
				.setHFLIP(topCameraParams[0])
				.setVFLIP(topCameraParams[1])
				.setAutoExposure(topCameraParams[2])
				.setBrightness(topCameraParams[3])
				.setContrast(topCameraParams[4])
				.setSaturation(topCameraParams[5])
				.setHue(topCameraParams[6])
				.setSharpness(topCameraParams[7])
				.setGamma(topCameraParams[8])
				.setAutoWhiteBalance(topCameraParams[9])
				.setExposure(topCameraParams[10])
				.setGain(topCameraParams[11])
				.setWhiteBalance(topCameraParams[12])
				.setFadeToBlack(topCameraParams[13])
				.build();
		
		CameraParams bottomCamera = CameraParams.newBuilder()
				.setWhichCamera("BOTTOM")
				.setHFLIP(bottomCameraParams[0])
				.setVFLIP(bottomCameraParams[1])
				.setAutoExposure(bottomCameraParams[2])
				.setBrightness(bottomCameraParams[3])
				.setContrast(bottomCameraParams[4])
				.setSaturation(bottomCameraParams[5])
				.setHue(bottomCameraParams[6])
				.setSharpness(bottomCameraParams[7])
				.setGamma(bottomCameraParams[8])
				.setAutoWhiteBalance(bottomCameraParams[9])
				.setExposure(bottomCameraParams[10])
				.setGain(bottomCameraParams[11])
				.setWhiteBalance(bottomCameraParams[12])
				.setFadeToBlack(bottomCameraParams[13])
				.build();
		
		ControlInstance second = ControlIO.getByIndex(0);
		if (second == null) {
			Logger.log(Logger.WARN, "CameraParams clicked when no ControlIO instance available");
			return;
		}
		second.tryAddCmnd(ControlIO.createCmndSetCameraParams(topCamera));
		second.tryAddCmnd(ControlIO.createCmndSetCameraParams(bottomCamera));
	}

	private void trySave(Integer[] topCameraPar, Integer[] bottomCameraPar) {
		JSONObject topCameraParams = new JSONObject();
		JSONObject bottomCameraParams = new JSONObject();

		topCameraParams.put("whichcamera","TOP");
		bottomCameraParams.put("whichcamera","BOTTOM");

		topCameraParams.put("hflip",topCameraPar[0]);
		topCameraParams.put("vflip",topCameraPar[1]);
		topCameraParams.put("autoexposure",topCameraPar[2]);
		topCameraParams.put("brightness",topCameraPar[3]);
		topCameraParams.put("contrast",topCameraPar[4]);
		topCameraParams.put("saturation",topCameraPar[5]);
		topCameraParams.put("hue",topCameraPar[6]);
		topCameraParams.put("sharpness",topCameraPar[7]);
		topCameraParams.put("gamma",topCameraPar[8]);
		topCameraParams.put("auto_whitebalance",topCameraPar[9]);
		topCameraParams.put("exposure",topCameraPar[10]);
		topCameraParams.put("gain",topCameraPar[11]);
		topCameraParams.put("white_balance",topCameraPar[12]);
		topCameraParams.put("fade_to_black",topCameraPar[13]);

		bottomCameraParams.put("hflip",bottomCameraPar[0]);
		bottomCameraParams.put("vflip",bottomCameraPar[1]);
		bottomCameraParams.put("autoexposure",bottomCameraPar[2]);
		bottomCameraParams.put("brightness",bottomCameraPar[3]);
		bottomCameraParams.put("contrast",bottomCameraPar[4]);
		bottomCameraParams.put("saturation",bottomCameraPar[5]);
		bottomCameraParams.put("hue",bottomCameraPar[6]);
		bottomCameraParams.put("sharpness",bottomCameraPar[7]);
		bottomCameraParams.put("gamma",bottomCameraPar[8]);
		bottomCameraParams.put("auto_whitebalance",bottomCameraPar[9]);
		bottomCameraParams.put("exposure",bottomCameraPar[10]);
		bottomCameraParams.put("gain",bottomCameraPar[11]);
		bottomCameraParams.put("white_balance",bottomCameraPar[12]);
		bottomCameraParams.put("fade_to_black",bottomCameraPar[13]);

		try {
			FileWriter topFile = new FileWriter("~/nbites/src/man/config/topCameraParams.json");
			FileWriter bottomFile = new FileWriter("~/nbites/src/man/config/bottomCameraParams.json");

			topFile.write(topCameraParams.toJSONString());
			topFile.flush();
			topFile.close();

			bottomFile.write(bottomCameraParams.toJSONString());
			bottomFile.flush();
			bottomFile.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
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
