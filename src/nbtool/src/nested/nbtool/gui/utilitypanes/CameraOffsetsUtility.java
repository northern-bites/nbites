package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import javax.swing.JFrame;
import javax.swing.table.DefaultTableModel;

import nbtool.data.SExpr;
import nbtool.data.group.AllGroups;
import nbtool.data.group.Group;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.gui.ToolMessage;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
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
		return 'c';
	}
	
	/* actual calibration code */
	/***************************/

	private static class Row {
		String name = "n/a";
		
		boolean top = true;
		String getCamera() {return top ? "camera_TOP" : "camera_BOT";}
	
		boolean status = true;
		String getStatus() {return status ? "good" : "failed";}
		
		double d_roll = 0;
		String getRoll() {return d_roll + " rads";}
		
		double d_tilt = 0;
		String getTilt() {return d_tilt + " rads";}
		
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
					row.d_roll = droll;
					row.d_tilt = dtilt;
				} else {
					row.status = false;
					row.d_roll = Double.NaN;
					row.d_tilt = Double.NaN;
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
		
		rows = new Row[topNames.length + botNames.length];
		for (int i = 0; i < rows.length; ++i) {
			rows[i] = new Row();
			
			if (i >= topNames.length) {
				int j = i - topNames.length;
				rows[i].top = false;
				rows[i].name = botNames[j];
				rows[i].given = found_bot.get(rows[i].name).size();
			} else {
				rows[i].top = true;
				rows[i].name = topNames[i];
				rows[i].given = found_top.get(rows[i].name).size();
			}
		}
		
		useFound(found_top, true);
		useFound(found_bot, false);
		
		if (display != null) {
			display.model.reDisplay();
		}
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
			
//			this.addComponentListener(new ComponentAdapter(){
//				public void componentShown(ComponentEvent e) {
//					calculate();
//				}
//			});
			
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
					String filePath = System.getenv().get("NBITES_DIR");
		            filePath += "/src/man/config/calibrationParams.txt";
		            Path configPath = Paths.get(filePath);
		            assert(Files.exists(configPath) && Files.isRegularFile(configPath));
		            
		            SExpr config = null;
		            
		            try {
		            	String contents = new String(Files.readAllBytes(configPath));
		            	config = SExpr.deserializeFrom(contents);
		            } catch (IOException ie) {
		            	ie.printStackTrace();
		            	Debug.error("IOException: %s", ie.getMessage());
		            	ToolMessage.displayError("error reading src/man/config/CalibrationParams.txt !");
		            	return;
		            }
		            
		            assert(config != null);
		            
		            if (rows == null) {
		            	ToolMessage.displayError("calculate offsets before saving!");
		            	return;
		            }
		            
		           String written = "";
		            
		            for (Row r : rows) {
		            	if (r != null) {
		            		if (!r.status) { 
		            			Debug.info("%s %s failed.", r.name, r.getCamera());
		            			continue;
		            		}
		            		
		            		Debug.info("replacing offsets for: %s %s", r.name, r.getCamera());
		            		
		            		SExpr list = config.get(1);
		            		SExpr line = list.find(r.name);
		            		assert(line.exists());
		            		
		            		String cameraString = r.top ? "TOP" : "BOT";
		   
		            		SExpr part = line.find(cameraString);
		            		assert(part.exists());
		            		assert(part.get(0).value().equals(cameraString));
		            		
		            		String repString = String.format("(%s %f %f)", 
		            				cameraString, r.d_roll, r.d_tilt);
		            		SExpr repSp = SExpr.deserializeFrom(repString);
		            		
		            		line.replace(part, repSp);
		            		written += String.format("{%s, %s}", r.name, r.getCamera());
		            	}
		            }
		            
		            try {
						Files.write(configPath, config.print().getBytes(StandardCharsets.UTF_8));
					} catch (IOException e1) {
						e1.printStackTrace();
						ToolMessage.displayError("couldn't write to config file: %s", e1.getMessage());
						return;
					}
		            
		            ToolMessage.displayInfo("camera offsets written: %s", written);
				}
				
			});
			
			this.panel.sentToRobotButton.addActionListener(new ActionListener(){

				@Override
				public void actionPerformed(ActionEvent e) {
					//TODO do
					Debug.notRefactored();
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
