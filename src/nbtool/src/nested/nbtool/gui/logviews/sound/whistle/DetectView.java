package nbtool.gui.logviews.sound.whistle;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.util.LinkedList;

import javax.swing.AbstractAction;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.KeyStroke;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.data.log.LogSorting;
import nbtool.gui.ToolMessage;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.nio.FileIO;
import nbtool.util.Debug;

public class DetectView extends ViewParent implements IOFirstResponder {

	static boolean play = false;
	static final int SPECTRUM_OUTPUT_LENGTH = 2048;

	FloatBuffer buff = null;
	SoundPane pane = null;
	JScrollPane scroll = new JScrollPane();

	JCheckBox containsBox = new JCheckBox("annotate:");
	JLabel detection = new JLabel();
	JLabel channel = new JLabel();

	int channel_index = 0;
	int max_index = 0;

	@Override
	public void setupDisplay() {
		Debug.info("view!");
		this.setLayout(new BorderLayout());

		if (alsoSelected.size() > 0) {
			Debug.warn("starting merge of %d logs", alsoSelected.size() + 1);
			LinkedList<LogReference> logRefs = new LinkedList<>();

			if (this.displayedLog.getReference() != null)
				logRefs.add(this.displayedLog.getReference());

			for (Log lg : this.alsoSelected) {
				if (lg.getReference() != null)
					logRefs.add(lg.getReference());
			}

			LogSorting.sort(LogSorting.Sort.BY_FILENAME, logRefs);

			int total = 0;
			byte[][] all = new byte[logRefs.size()][];

			for (int i = 0; i <logRefs.size(); ++i) {
				LogReference ref = logRefs.get(i);

				all[i] = ref.get().blocks.get(0).data;
				total += all[i].length;

				Debug.info("\t%d bytes from", all[i].length, ref.toString());
			}

			byte[] all_bytes = new byte[total];

			int pos = 0;
			for (int i = 0; i < all.length; ++i) {
				System.arraycopy(all[i], 0, all_bytes, pos, all[i].length);
				pos += all[i].length;
			}

			Log out = Log.emptyLog();
			out.logClass = "DetectAmplitude";
			out.blocks.add(Block.explicit(all_bytes, "SoundAmplitude"));

			String name = String.format("merged_%dlogs_%d_%d.nblog",
					logRefs.size(), logRefs.get(0).thisID, logRefs.getLast().thisID);


			try {
				FileIO.writeLogToPath(logRefs.get(0).loadPath().getParent().resolve(name), out);
				ToolMessage.displayWarn("merged %d logs to %s", logRefs.size(), name);
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}

		if (this.displayedLog.getReference() != null && this.displayedLog.getReference().loadPath() != null)
			Debug.print("load: %d from %s", this.displayedLog.getReference().thisID, this.displayedLog.getReference().loadPath().getFileName());

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
			container.add(channel, BorderLayout.EAST);

			this.add(container, BorderLayout.NORTH);

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

			channel.setText(String.format(" (%d / %d)", channel_index, max_index ));
			this.getActionMap().put("NextIndex", new AbstractAction(){
				@Override
				public void actionPerformed(ActionEvent e) {
					channel_index = Math.min(channel_index + 1, max_index);
					Debug.print("channel index: %d", channel_index);
					channel.setText(String.format(" (%d / %d)", channel_index, max_index ));
					repaint();
				}
			});

			this.getActionMap().put("PrevIndex", new AbstractAction(){
				@Override
				public void actionPerformed(ActionEvent e) {
					channel_index = Math.max(channel_index - 1, 0);
					Debug.print("channel index: %d", channel_index);
					channel.setText(String.format(" (%d / %d)", channel_index, max_index ));
					repaint();
				}
			});

			this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
					KeyStroke.getKeyStroke(KeyEvent.VK_CLOSE_BRACKET, 0)
					, "NextIndex");
			this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
					KeyStroke.getKeyStroke(KeyEvent.VK_OPEN_BRACKET, 0)
					, "PrevIndex");
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

		max_index = out[0].blocks.size() - 1;
		buff = new FloatBuffer(new Block(all, ""), out[0].blocks.size(),
				SPECTRUM_OUTPUT_LENGTH);

		if (pane != null) {
			scroll.remove(pane);
		}

		detection.setText("detect: " + out[0].topLevelDictionary.get("WhistleHeard").asBoolean().bool());

		pane = new SoundPane(1, 2048) {

			@Override
			public int pixels(int c, int f, int radius) {
				return (int) ((buff.get(f, channel_index) / buff.max()) * radius * -1);
			}

			@Override
			public String peakString() {
				return "max = " + buff.max;
			}

			@Override
			public String selectionString(int c, int f) {
				String fmat = String.format("c%df%d: %f", channel_index, f, buff.get(f, channel_index));
				Debug.print("%s", fmat);
				return fmat;
			}
		};

		scroll.setViewportView(pane);
		repaint();
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}
}
