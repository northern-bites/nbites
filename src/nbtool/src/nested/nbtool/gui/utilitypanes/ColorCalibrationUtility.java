package nbtool.gui.utilitypanes;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.JTabbedPane;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.calibration.ColorParam;
import nbtool.data.json.Json;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.gui.ToolMessage;
import nbtool.gui.logdnd.LogDND;
import nbtool.gui.logdnd.LogDND.LogDNDTarget;
import nbtool.gui.logviews.images.ImageDisplay;
import nbtool.gui.logviews.misc.VisionView;
import nbtool.images.Y8Image;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.nio.LogRPC;
import nbtool.nio.RobotConnection;
import nbtool.util.Center;
import nbtool.util.Debug;
import nbtool.util.Events;
import nbtool.util.SharedConstants;
import nbtool.util.Utility;

public class ColorCalibrationUtility extends UtilityProvider<ColorParam.Set, ColorCalibrationListener> {

	private static ColorCalibrationUtility instance = null;
	private static Debug.DebugSettings debug = Debug.createSettings(Debug.INFO);

	ColorCalibrationUtility() {
		if (instance != null) {
			throw new RuntimeException("singeton class");
		} else {
			instance = this;
		}
	}

	@Override
	public ColorParam.Set getLatest() {
		return color_parameters;
	}

	public boolean appliedGlobally() {
		return (display == null) ? false : display.applyGlobally;
	}

	private static Display display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display != null) {
			return display;
		} else {
			try {
				return (display = new Display());
			} catch (java.lang.OutOfMemoryError err) {
				err.printStackTrace();
				Debug.print("%s", err.getMessage());
				return null;
			}
		}
	}

	@Override
	public String purpose() {
		return "calibrate the robot's vision color parameters";
	}

	@Override
	public char preferredMemnonic() {
		return 'c';
	}

	private static ColorParam.Set color_parameters = null;

	private static void colorParametersUpdated() {
		Debug.print("color parameters have updated.");

		if (display != null && display.applyGlobally) {
			instance.fireChanged();
		}
	}

	private static class Display extends JFrame implements Events.LogSelected {

		JTabbedPane tabs = new JTabbedPane();
		TabHandler[] handlers = new TabHandler[4];
		boolean applyGlobally = true;
		boolean takeSelection = false;

		private ActionListener saveListener = new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				Debug.warn("saving color parameters to NBITES_DIR !");

				try {
					Files.write(ColorParam.getPath(),
							color_parameters.serialize().print().getBytes(StandardCharsets.UTF_8));
					ToolMessage.displayWarn("wrote color parameters");
				} catch (IOException e1) {
					e1.printStackTrace();
					ToolMessage.displayError("could not write color parameters.");
				}
			}
		};

		private ActionListener sendListener = new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				RobotConnection robot = RobotConnection.getByIndex(0);

				if (robot == null) {
					ToolMessage.displayError("CCU: send: no robot connected!");
					return;
				}

				if (color_parameters == null) {
					ToolMessage.displayError("CCU: send: color params!");
					return;
				}

				LogRPC.setFileContents(LogRPC.NULL_RESPONDER, robot, "/home/nao/nbites/Config/colorParams.json",
						color_parameters.serialize().print());

				ToolMessage.displayAndPrint("< params sent to %s >", robot.host());
			}

		};

		private ChangeListener globalListener = new ChangeListener() {

			@Override
			public void stateChanged(ChangeEvent e) {
				boolean nv = ((JCheckBox) e.getSource()).isSelected();
				for (TabHandler th : handlers) {
					th.tab.globalCheckBox.setSelected(nv);
				}

				applyGlobally = nv;
			}
		};

		private ChangeListener takeListener = new ChangeListener() {

			@Override
			public void stateChanged(ChangeEvent e) {
				boolean nv = ((JCheckBox) e.getSource()).isSelected();
				for (TabHandler th : handlers) {
					th.tab.takeCheckBox.setSelected(nv);
				}

				takeSelection = nv;
			}
		};

		@Override
		public void logSelected(Object source, Log first, List<Log> alsoSelected) {
			if (takeSelection) {
				if (first.logClass.equals("tripoint")) {
					for (TabHandler handler : handlers) {
						if (handler.top == Utility.camera(first)) {
							handler.useLog(first);
						}
					}
				}
			}
		}

		private LogDNDTarget topCameraTarget = new LogDNDTarget() {
			@Override
			public void takeLogsFromDrop(Log[] log) {
				if (log.length > 0) {
					Log attempt = log[0];

					if (!attempt.logClass.equals("tripoint")) {
						ToolMessage.displayError("must use tripoint log for ColorCalibration, not %s!",
								attempt.logClass);
						return;
					}

					String reqFrom = "camera_TOP";

					if (!attempt.blocks.get(0).whereFrom.equals(reqFrom)) {
						ToolMessage.displayError("tab {%s} must have log from: %s",
								"TOP", reqFrom);
						return;
					}

					for (TabHandler handler : handlers) {
						if (handler.top) {
							handler.useLog(attempt);
						}
					}
				}
			}
		};

		private LogDNDTarget botCameraTarget = new LogDNDTarget() {
			@Override
			public void takeLogsFromDrop(Log[] log) {
				if (log.length > 0) {
					Log attempt = log[0];

					if (!attempt.logClass.equals("tripoint")) {
						ToolMessage.displayError("must use tripoint log for ColorCalibration, not %s!",
								attempt.logClass);
						return;
					}

					String reqFrom = "camera_BOT";

					if (!attempt.blocks.get(0).whereFrom.equals(reqFrom)) {
						ToolMessage.displayError("tab {%s} must have log from: %s",
								"BOT", reqFrom);
						return;
					}

					for (TabHandler handler : handlers) {
						if (!handler.top) {
							handler.useLog(attempt);
						}
					}
				}
			}
		};

		Display() {
			super("color calibration utility");

			Debug.plain("getting initial color parameters");
			Path colorPath = ColorParam.getPath();
			assert (Files.exists(colorPath) && Files.isRegularFile(colorPath));

			ColorParam.Set set = null;

			try {
				String contents = new String(Files.readAllBytes(colorPath));
				set = ColorParam.Set.parse(Json.parse(contents).asObject());
			} catch (IOException ie) {
				ie.printStackTrace();
				Debug.error("IOExceptionL %s", ie.getMessage());
				ToolMessage.displayError("error parsing json file %s", colorPath);
				return;
			} catch (JsonParseException e1) {
				e1.printStackTrace();
				Debug.error("JsonParseException: %s", e1.getMessage());
				ToolMessage.displayError("error parsing json file %s !", colorPath);
				return;
			}

			assert (set != null);

			color_parameters = set;

			/**
			 * THIS IS WHERE THE TABS ARE SET UP:
			 * true is top, false is bottom!
			 * Camera.Which enum is defined in nbtool.data.calibration.ColorParam.java
			 * */

			handlers[0] = new TabHandler(true, ColorParam.Camera.Which.white);
			handlers[1] = new TabHandler(false, ColorParam.Camera.Which.white);

			handlers[2] = new TabHandler(true, ColorParam.Camera.Which.green);
			handlers[3] = new TabHandler(false, ColorParam.Camera.Which.green);

			for (TabHandler th : handlers) {
				tabs.add(th.title(), th.tab);

				th.tab.SaveButton.addActionListener(saveListener);
				th.tab.SendButton.addActionListener(sendListener);
				th.tab.globalCheckBox.addChangeListener(globalListener);
				th.tab.takeCheckBox.addChangeListener(takeListener);
			}

			for (int i = 0; i < handlers.length; ++i) {
				handlers[i].takeFrom(set);
			}

			for (int i = 0; i < handlers.length; ++i) {
				handlers[i].installListeners();
			}

			Dimension minSize = new Dimension(tabs.getMinimumSize());
			minSize.width += 50;
			minSize.height += 50;

			Center.listen(Events.LogSelected.class, this, true);

			this.setContentPane(tabs);
			this.setMinimumSize(minSize);
		}

		private class TabHandler {
			boolean top;
			ColorParam.Camera.Which camera;
			ColorCalibrationTab tab;

			ImageDisplay imageDisplay;
			ImageDisplay imageDisplayNorm;

			Log dropped = null;
			int dropped_width, dropped_height;

			public String title() {
				return (top ? "top" : "bot") + " " + camera.toString();
			}

			class Group {
				JSlider slider;
				JSpinner spinner;
				ColorParam.Part part;

				Group (JSlider slider, JSpinner spinner, ColorParam.Part part) {
					this.slider = slider; this.spinner = spinner; this.part = part;
				}
			}

			class Change {
				Group group;
				int previousValue;
			}

			private ArrayList<Group> groups = new ArrayList<>();
			private ArrayList<Change> undoStack = new ArrayList<>();
			boolean ignoreChangeEvents = false;

			private void undoTheChange() {
				if (undoStack.isEmpty()) return;

				Debug.print("undoing!");
				ignoreChangeEvents = true;

				Change latest = undoStack.remove(undoStack.size() - 1);
				latest.group.slider.setValue(latest.previousValue);
				latest.group.spinner.setValue(latest.previousValue);

				SwingUtilities.invokeLater(new Runnable(){
					@Override
					public void run() {
						Debug.print("undo finished!");
						ignoreChangeEvents = false;
					}
				});

				wasUpdated();
			}

			private void addChange(Group group, int previous) {
				Change change = new Change();
				change.group = group;
				change.previousValue = previous;

				undoStack.add(change);
				while(undoStack.size() > 1000) {
					undoStack.remove(0);
				}
			}

			private void visionCall() {
				assert(dropped != null);
				assert(color_parameters != null);

				dropped.topLevelDictionary.put("ModifiedColorParams", color_parameters.get(top).serialize());
				debug.info("%s", dropped.topLevelDictionary.get("ModifiedColorParams").print());

				CrossInstance ci = CrossServer.instanceByIndex(0);
				if (ci == null) {
					ToolMessage.displayWarn("{%s} cannot call vision, no CrossInstance!", title());
					return;
				}

				assert(ci.tryAddCall(new IOFirstResponder(){

					@Override
					public void ioFinished(IOInstance instance) { }

					@Override
					public void ioReceived(IOInstance inst, int ret, Log... out) {
						assert(out.length == 1);
						assert(out[0].logClass.equals("VisionReturn"));

						String search = camera.toString() + "Ret";
						debug.info("TabHandler looking for: %s", search);

						Block block = out[0].find(search);
						Y8Image image = new Y8Image(dropped_width / 2, dropped_height / 2, block.data);

						if (imageDisplay != null)
							imageDisplay.setImage(image.toBufferedImage());
						else debug.error("{%s} null image display!", title());
					}

					@Override
					public boolean ioMayRespondOnCenterThread(IOInstance inst) {
						return false;
					}

				}, VisionView.DEFAULT_VISION_FUNCTION_NAME, dropped));
			}

			private void wasUpdated() {

				setTo(color_parameters);

				if (dropped != null) {
					debug.info("{%s} making vision call", title());
					visionCall();
				}

				colorParametersUpdated();
			}

			public void useLog(Log dropped) {
				Debug.print("{%s} using log: %s", title(), dropped);
				this.dropped = dropped;
				dropped_width = dropped.blocks.get(0).dict.get(SharedConstants.LOG_BLOCK_IMAGE_WIDTH_PIXELS())
						.asNumber().asInt();
				dropped_height = dropped.blocks.get(0).dict.get(SharedConstants.LOG_BLOCK_IMAGE_HEIGHT_PIXELS())
							.asNumber().asInt();


				if (imageDisplayNorm != null)
					imageDisplayNorm.setImage(dropped.blocks.get(0).parseAsYUVImage().toBufferedImage());
				else debug.error("{%s} null image display for normal pic!", title());

				visionCall();
			}

			public void takeFrom(ColorParam.Set set) {
				ColorParam.Camera cam = set.get(top);
				ColorParam param = cam.get(camera);
				for (Group g : groups) {
					int val = (int) param.get(g.part);
					g.slider.setValue(val);
					g.spinner.setValue(val);
				}
			}

			public void setTo(ColorParam.Set set) {
				ColorParam.Camera cam = set.get(top);
				ColorParam param = cam.get(camera);

				for (Group g : groups) {
					param.set(g.part, g.slider.getValue());
				}
			}

			private ChangeListener fromSpinner = new ChangeListener() {

				@Override
				public void stateChanged(ChangeEvent e) {
					if (ignoreChangeEvents) return;

					Group from = null;
					for (Group g : groups) {
						if (g.spinner == e.getSource()) {
							from = g;
							break;
						}
					}

					int previous = (Integer) from.spinner.getValue();
					from.slider.setValue(previous);

					addChange(from, previous);
					wasUpdated();
				}

			};

			private ChangeListener fromSlider = new ChangeListener() {

				@Override
				public void stateChanged(ChangeEvent e) {
					if (ignoreChangeEvents) return;

					Group from = null;
					for (Group g : groups) {
						if (g.slider == e.getSource()) {
							from = g;
							break;
						}
					}

					int previous = from.slider.getValue();
					from.spinner.setValue(previous);

					addChange(from, previous);
					wasUpdated();
				}
			};

			private ActionListener undoListener = new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					undoTheChange();
				}
			};

			protected void installListeners() {
				for (Group g : groups) {
					g.slider.addChangeListener(fromSlider);
					g.spinner.addChangeListener(fromSpinner);
				}

				tab.UndoButton.addActionListener(undoListener);
			}

			public TabHandler(final boolean top, ColorParam.Camera.Which camera) {
				debug.info("in constructor");

				this.top = top; this.camera = camera;
				this.tab = new ColorCalibrationTab();
				this.tab.tabTitle.setText(title());

				tab.SendButton.setText("send to robot");
				tab.SaveButton.setText("save to config");

				groups.add(new Group(tab.Y0USlider, tab.Y0USpinner, ColorParam.Part.uAtY0));
				groups.add(new Group(tab.Y255USlider, tab.Y255USpinner, ColorParam.Part.uAtY255));

				groups.add(new Group(tab.Y0VSlider, tab.Y0VSpinner, ColorParam.Part.vAtY0));
				groups.add(new Group(tab.Y255VSlider, tab.Y255VSpinner, ColorParam.Part.vAtY255));

				groups.add(new Group(tab.FuzzyUSlider, tab.FuzzyUSpinner, ColorParam.Part.u_fuzzy_range));
				groups.add(new Group(tab.FuzzyVSlider, tab.FuzzyVSpinner, ColorParam.Part.v_fuzzy_range));

				for (Group g : groups) {
					g.spinner.setModel(new SpinnerNumberModel(0,0,255,1));
					g.slider.setMinimum(0);
					g.slider.setMaximum(255);
				}

				Dimension ratio = new Dimension(320,240);

				imageDisplay = new ImageDisplay(ratio);
				tab.imageSplitPane.setLeftComponent(imageDisplay);

				imageDisplayNorm = new ImageDisplay(ratio);
				tab.imageSplitPane.setRightComponent(imageDisplayNorm);

				tab.imageSplitPane.setResizeWeight(0.5);
				tab.imageSplitPane.setEnabled(false);
				tab.imageSplitPane.setBackground(Color.BLACK);
				tab.imageSplitPane.setForeground(Color.BLACK);

				tab.imageSplitPane.setDividerLocation(0.5);

				LogDND.makeComponentTarget(tab, top ? topCameraTarget : botCameraTarget);

				debug.info("leaving constructor");
			}
		}

	}

}
