package nbtool.gui.logviews.sound.whistle;

import java.awt.BorderLayout;

import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.gui.ToolMessage;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Debug;

public class DetectView extends ViewParent implements IOFirstResponder {

	static boolean play = false;
	static final int SPECTRUM_OUTPUT_LENGTH = 2049;

	FloatBuffer buff = null;
	SoundPane pane = null;
	JScrollPane scroll = new JScrollPane();

	JCheckBox containsBox = new JCheckBox("annotate:");
	JLabel detection = new JLabel();

	@Override
	public void setupDisplay() {
		Debug.info("view!");
		this.setLayout(new BorderLayout());

		final JCheckBox playBox = new JCheckBox();
		playBox.setText("play sound on selection");
		playBox.setSelected(play);
		playBox.addChangeListener(new ChangeListener(){
			@Override
			public void stateChanged(ChangeEvent e) {
				play = playBox.isSelected();
			}
		});

		this.add(playBox, BorderLayout.SOUTH);

		if (play) {
			final byte[] dataToPlay = displayedLog.blocks.get(0).data;
			PlaySound.play(dataToPlay);
		}

		if (displayedLog.logClass.equals("DetectAmplitude")) {
			CrossInstance ci = CrossServer.instanceByIndex(0);
			if (ci == null) return;
			ci.tryAddCall(this, "whistle_detect", this.displayedLog);
			this.add(scroll, BorderLayout.CENTER);
			scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
			scroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

			JPanel container = new JPanel(new BorderLayout());
			container.add(detection, BorderLayout.CENTER);
			container.add(containsBox, BorderLayout.WEST);

			this.add(container, BorderLayout.NORTH);
//			this.add(heardBox, BorderLayout.NORTH);

			if (displayedLog.topLevelDictionary.get("WhistleHeard") != null)
				containsBox.setSelected(displayedLog.topLevelDictionary.get("WhistleHeard").asBoolean().bool());

			containsBox.addChangeListener(new ChangeListener() {
				@Override
				public void stateChanged(ChangeEvent e) {
					displayedLog.topLevelDictionary.put("WhistleHeard", containsBox.isSelected());
					if (!displayedLog.temporary()) {
						try {
							displayedLog.saveChangesToLoadFile();
						} catch (Exception e1) {
							e1.printStackTrace();
							ToolMessage.displayError("could not write log!!");
						}
					}
				}
			});
		}
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{"DetectAmplitude", "soundStuff"};
	}

	@Override
	public void ioFinished(IOInstance instance) { }

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		assert(out.length == 1);
		int total = 0;
		for (Block b : out[0].blocks) {
			total += b.data.length;
		}

		byte[] all = new byte[total];
		int offset = 0;

		for (Block b : out[0].blocks) {
			System.arraycopy(b.data, 0, all, offset, b.data.length);
			offset += b.data.length;
		}

		buff = new FloatBuffer(new Block(all, ""), out[0].blocks.size(),
				SPECTRUM_OUTPUT_LENGTH);

		if (pane != null) {
			scroll.remove(pane);
		}

		detection.setText("detect: " + out[0].topLevelDictionary.get("WhistleHeard").asBoolean().bool());

		pane = new SoundPane(out[0].blocks.size(), 1024) {

			@Override
			public int pixels(int c, int f, int radius) {
				return (int) ((buff.get(f, c) / buff.max()) * radius * -1);
			}

			@Override
			public String peakString() {
				return "max = " + buff.max;
			}

			@Override
			public String selectionString(int c, int f) {
				String fmat = String.format("c%df%d: %f", c, f, buff.get(f, c));
				Debug.print("%s", fmat);
				return fmat;
			}
		};

		scroll.setViewportView(pane);
		repaint();
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		// TODO Auto-generated method stub
		return false;
	}
}
