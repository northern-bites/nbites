//package nbtool.gui.logviews.sound.whistle;
//
//import java.awt.BorderLayout;
//import java.awt.Color;
//import java.awt.Font;
//
//import javax.swing.JLabel;
//import javax.swing.JScrollPane;
//
//import nbtool.data._log._Log;
//import nbtool.gui.logviews.misc.ViewParent;
//import nbtool.gui.logviews.sound2.FFTWBuffer;
//import nbtool.gui.logviews.sound2.ShortBuffer;
//import nbtool.gui.logviews.sound2.SoundPane;
//import nbtool.gui.logviews.sound2.WhistleDetector;
//
//public class FrequencyStream extends ViewParent {
//	
//	FloatBuffer buffer = null;
//
//	@Override
//	public void setLog(_Log newlog) {
//		buffer = new FloatBuffer();
//		buffer.parse(newlog);
//		
//		boolean heard = false;
//		
//		double sum = 0;
//		for (int i = 10; i < 5000; ++i) {
//			sum += buffer.index(0, i);
//		}
//		
//		sum /= 4990;
//		double accum = 0.0;
//		
//		for (int i = 10; i < 5000; ++i) {
//			double val = buffer.index(0, i);
//			accum += (val - sum) * (val - sum);
//		}
//		
//		accum /= 4990;
//		double sdev = Math.sqrt(accum);
//		heard = sdev > 1000;
//		
////		double sum = 0;
////		for (int i = 1600; i < 1800; ++i) {
////			sum += buffer.index(0, i);
////		}
//
//		
//		final String prefix = String.format("%f :: %B :: ", sdev, heard);
//		
//		JScrollPane scroll = new JScrollPane();
//		final JLabel topLabel = new JLabel(prefix);
//		
//		SoundPane sp = new SoundPane(buffer.channels, buffer.frames) {
//			@Override
//			public int pixels(int c, int f, int radius) {
//				return (int) ((buffer.get(f, c) / buffer.max) * radius);
//			}
//
//			@Override
//			public String peakString() {
//				return "max abs val = " + buffer.max;
//			}
//
//			@Override
//			public String selectionString(int c, int f) {
//				topLabel.setText(prefix + " " +  String.format("c%d f%d val=%f", c, f, buffer.get(f, c) ) );
//				return String.format("c%d f%d val=%f", c, f, buffer.get(f, c));
//			}
//		};
//		
//		scroll.setViewportView(sp);
////		scroll.setPreferredSize(this.getSize());
////		scroll.setSize(this.getSize());
//		this.add(topLabel, BorderLayout.NORTH);
//		this.add(scroll, BorderLayout.CENTER);
//		
////		String ftext = "";
////		for (int i = 0; i < buffer.channels; ++i) {
////			if (WhistleDetector.detect(buffer, i)) {
////				ftext += "[found in " + i + "]";
////			}
////		}
//		
////		if (!ftext.isEmpty()) {
////			JLabel ft = new JLabel(ftext);
////			Font font = ft.getFont();
////			ft.setForeground(Color.RED);
////			ft.setFont(font.deriveFont(Font.BOLD));
////			
////			this.add(ft, BorderLayout.SOUTH);
////		}
//		
//		/*
//		if (newlog.sexprForContentItem(0).find("whistle").exists()) {
//			this.add(new JLabel("found!"), BorderLayout.SOUTH);
//		} */
//	}
//	
//	public FrequencyStream() {
//		super();
//		this.setLayout(new BorderLayout());
//	}
//	
//}
