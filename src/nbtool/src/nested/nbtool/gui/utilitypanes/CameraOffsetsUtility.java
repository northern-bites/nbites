package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import javax.swing.JFrame;
import javax.swing.table.DefaultTableModel;

import nbtool.data.calibration.CameraOffset;
import nbtool.data.group.AllGroups;
import nbtool.data.group.Group;
import nbtool.data.json.Json;
import nbtool.data.json.JsonParser.JsonParseException;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.gui.ToolMessage;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.nio.LogRPC;
import nbtool.nio.RobotConnection;
import nbtool.util.Debug;
import nbtool.util.Robots;
import nbtool.util.SharedConstants;

public class CameraOffsetsUtility extends UtilityParent {

	/* utility parent implementation code */
	/**************************************/


	private static Display display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display != null) {
			return display;
		} else {
			return (display = new Display());
		}
	}

	@Override
	public String purpose() {
		return "calibrate robot cameras to account for roll/tilt error";
	}

	@Override
	public char preferredMemnonic() {
		return 'o';
	}

	/* actual offset code */
	/***************************/

	private static class Row {
		String name = "n/a";

		boolean top = true;
		String getCamera() {return top ? "camera_TOP" : "camera_BOT";}

		boolean status = true;
		String getStatus() {return status ? "good" : "failed";}

		CameraOffset offset;

		String getRoll() {return offset.d_roll + " rads";}

		String getTilt() {return offset.d_tilt + " rads";}

		int given = -1;
		String getGiven() {return "" + given;}

		int used = -1;
		String getUsed() {return "" + used;}
	}

	private static Row[] rows;

	private static void insert(String name, LogReference ref, Map<String, Set<LogReference>> to) {
		if (to.containsKey(name)) {
			to.get(name).add(ref);
		} else {
			Set<LogReference> newSet = new HashSet<>();
			newSet.add(ref);
			to.put(name, newSet);
		}
	}

	private static class OffsetResponder implements IOFirstResponder {

		String name;
		boolean top;

		OffsetResponder(String n, boolean t) {
			name = n; top = t;
		}

		@Override
		public void ioFinished(IOInstance instance) { }

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			Row row = null;
			for (Row r : rows) {
				if (r.name.equals(name) && r.top == top) {
					row = r;
					break;
				}
			}

			if (row != null) {
				int successes = out[0].topLevelDictionary.get("CalibrationNumSuccess").asNumber().asInt();
				double droll = out[0].topLevelDictionary.get("CalibrationDeltaTilt").asNumber().asDouble();
				double dtilt = out[0].topLevelDictionary.get("CalibrationDeltaRoll").asNumber().asDouble();

				row.used = successes;

				if (successes >= 7) {
					row.status = true;
					row.offset = new CameraOffset(droll, dtilt);
				} else {
					row.status = false;
					row.offset = new CameraOffset(Double.NaN, Double.NaN);
				}

				if (display != null)
					display.model.reDisplay();
			}
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) { return false; }

	}

	private static void useFound(Map<String, Set<LogReference>> found, final boolean top) {

		CrossInstance ci = CrossServer.instanceByIndex(0);
		if (ci == null) {
			Debug.error("cannot calculate camera offsets without CrossInstance");
			return;
		}

		for (Entry<String, Set<LogReference>> entry : found.entrySet()) {

			String robotName = entry.getKey();
			Log[] logs = new Log[entry.getValue().size()];

			int i = 0;
			for (LogReference lr : entry.getValue()) {
				logs[i++] = lr.get();
			}

			ci.tryAddCall(new OffsetResponder(robotName, top), "CalculateCameraOffsets", logs);
		}
	}

	private static void calculate() {
		Map<String, Set<LogReference>> found_top = new HashMap<>();
		Map<String, Set<LogReference>> found_bot = new HashMap<>();

		for (Group group : AllGroups.allGroups) {
			for (LogReference ref : group.logs) {

				if (ref.logClass.equals(
						SharedConstants.LogClass_Tripoint())) {

					if (Robots.ROBOT_HOSTNAMES.contains(ref.host_name)) {

						String name = Robots.HOSTNAME_TO_ROBOT.get(ref.host_name).name;

						if (ref.description.contains("camera_TOP")) {
							insert(name, ref, found_top);
						} else {
							insert(name, ref, found_bot);
						}

					}

				}
			}
		}

		ToolMessage.displayWarn("found top logs from %d robots", found_top.size());
		ToolMessage.displayWarn("found bot logs from %d robots", found_bot.size());

		String[] topNames = found_top.keySet().toArray(new String[0]);
		String[] botNames = found_bot.keySet().toArray(new String[0]);

		List<Row> rowList = new LinkedList<>();
		rows = new Row[topNames.length + botNames.length];

		for (int i = 0; i < rows.length; ++i) {
			Row ltst = new Row();

			if (i >= topNames.length) {
				int j = i - topNames.length;
				ltst.top = false;
				ltst.name = botNames[j];
				ltst.given = found_bot.get(ltst.name).size();
			} else {
				ltst.top = true;
				ltst.name = topNames[i];
				ltst.given = found_top.get(ltst.name).size();
			}

			rowList.add(ltst);
		}

		Collections.sort(rowList, new Comparator<Row>(){

			@Override
			public int compare(Row o1, Row o2) {
				if (o1.name.equals(o2.name)) {
					return (o1.top == o2.top) ? 0 : 1;
				} else {
					return o1.name.compareTo(o2.name);
				}
			}

		});

		rows = rowList.toArray(new Row[0]);

		useFound(found_top, true);
		useFound(found_bot, false);

		if (display != null) {
			display.model.reDisplay();
		}
	}

	private static CameraOffset.Set fromFileSystem() {
		Path offsetsPath = CameraOffset.getPath();
        if(! (Files.exists(offsetsPath) && Files.isRegularFile(offsetsPath)) ) {
        	ToolMessage.displayError("no viable camera offsets file found at %s", offsetsPath);
        	return null;
        }

        CameraOffset.Set offsets = null;

        try {
        	String contents = new String(Files.readAllBytes(offsetsPath));
        	offsets = CameraOffset.Set.parse(Json.parse(contents).asObject());
        } catch (IOException ie) {
        	ie.printStackTrace();
        	Debug.error("IOException: %s", ie.getMessage());
        	ToolMessage.displayError("error reading %s!", offsetsPath);
        	return null;
        } catch (JsonParseException e1) {
			e1.printStackTrace();
			Debug.error("JsonParseException: %s", e1.getMessage());
        	ToolMessage.displayError("error parsing json file %s !", offsetsPath);
        	return null;
		}

        return offsets;
	}

	private static String updateSet(CameraOffset.Set offsets) {
        String written = "";

		for (Row r : rows) {
        	if (r != null) {
        		if (!r.status) {
        			Debug.info("%s %s failed.", r.name, r.getCamera());
        			continue;
        		}

        		Debug.info("replacing offsets for: %s %s", r.name, r.getCamera());

        		if (!offsets.containsKey(r.name)) {
        			offsets.put(r.name, new CameraOffset.Pair(
        					new CameraOffset(0,0),
        					new CameraOffset(0,0)
        					) );
        		}

        		if (r.top) {
        			offsets.get(r.name).top = r.offset;
        		} else {
        			offsets.get(r.name).bot = r.offset;
        		}

        		written += String.format("{%s, %s}", r.name, r.getCamera());
        	}
        }

		return written;
	}


	/* GUI and table model code */
	/****************************/

	private static class Display extends JFrame {
		private final CameraOffsetsPanel panel = new CameraOffsetsPanel();
		final Model model = new Model();

		Display() {
			super("camera offset utility");
			this.setContentPane(panel);
			this.panel.displayTable.setModel(model);
			this.panel.displayTable.getTableHeader().setFont(new Font("PT Serif", Font.BOLD, 14));

			this.setMinimumSize(new Dimension(600,200));

			this.panel.goButton.addActionListener(new ActionListener(){
				@Override
				public void actionPerformed(ActionEvent e) {
					calculate();
				}
			});

			this.panel.saveToConfigButton.addActionListener(new ActionListener(){

				@Override
				public void actionPerformed(ActionEvent e) {
					Debug.plain("saving parameters...");

		            CameraOffset.Set offsets = fromFileSystem();

		            if (offsets == null) return;

		            if (rows == null) {
		            	ToolMessage.displayError("calculate offsets before saving!");
		            	return;
		            }

		            String written = updateSet(offsets);

		            ToolMessage.displayInfo("camera offsets written: %s", written);
				}

			});

			this.panel.sentToRobotButton.addActionListener(new ActionListener(){

				@Override
				public void actionPerformed(ActionEvent e) {

					RobotConnection robot = RobotConnection.getByIndex(0);

					if (robot == null) {
						ToolMessage.displayError("COU: send: no robot connected!");
						return;
					}

		            CameraOffset.Set offsets = fromFileSystem();

		            if (rows == null) {
		            	ToolMessage.displayError("calculate offsets before saving!");
		            	return;
		            }

		            boolean found = false;
		            for (Row r : rows) {
		            	if (r != null && r.name.equals(robot.host())) {
		            		found = true; break;
		            	}
		            }

		            if (!found) ToolMessage.displayWarn("SENDING OFFSETS TO ROBOT WHICH WAS NOT CALIBRATED");

		            updateSet(offsets);

					LogRPC.setFileContents(LogRPC.NULL_RESPONDER, robot, "/home/nao/nbites/Config/cameraOffsets.json",
							offsets.serialize().print());

					ToolMessage.displayAndPrint("< params sent to %s >", robot.host());
				}

			});
		}
	}

	private static class Model extends DefaultTableModel {

		Model() {
			super(new String[] {
	                "robot", "camera", "status", "d-roll", "d-tilt", "given", "used"
	            }, Robots.ROBOTS.length);
		}

		@Override
		public boolean isCellEditable(int rowIndex, int columnIndex) {
            return false;
        }

		void reDisplay() {
			this.fireTableDataChanged();
		}

		@Override
		public int getRowCount() {
			return (rows == null) ? 0 : rows.length;
		}

		@Override
		public Object getValueAt(int _row, int col) {
			Row row = rows[_row];

			switch(col) {
			case 0: return row.name;
			case 1: return row.getCamera();
			case 2: return row.getStatus();
			case 3: return row.getRoll();
			case 4: return row.getTilt();
			case 5: return row.getGiven();
			case 6: return row.getUsed();

			default:
				Debug.error("%d column in COU!", col);
				throw new RuntimeException();
			}
		}

	}

}
