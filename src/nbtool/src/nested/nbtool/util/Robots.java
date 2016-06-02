package nbtool.util;

import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Set;

public class Robots {
	
	public static class Robot {
		public final String name;
		public final String wifiName;
		public final String ethrName;
		
		public final String wifiAddr;
		public final String ethrAddr;
		
		protected Robot(String n, String wa) {
			this.name = n; this.wifiName = n; this.ethrName = n + ".local";
			this.wifiAddr = wa;
			String[] parts = wifiAddr.split("\\.");
			assert(parts.length == 4);
			this.ethrAddr = "169.254.75." + parts[3];
		}
	}
		
	public static Robot[] ROBOTS = {
			new Robot("wash" , "139.140.192.9" ),
			new Robot("river" , "139.140.192.10"),
			new Robot("jayne" , "139.140.192.11"),
			new Robot("simon" , "139.140.192.12"),
			new Robot("inara" , "139.140.192.13"),
			new Robot("kaylee" , "139.140.192.14"),
			new Robot("vera" , "139.140.192.15"),
			new Robot("mal" , "139.140.192.16"),
			new Robot("zoe" , "139.140.192.17"),

			new Robot("ringo" , "139.140.192.18"),
			new Robot("beyonce", "139.140.192.19"),
			new Robot("ozzy", "139.140.192.20"),
			new Robot("avril" , "139.140.192.21"),
			new Robot("batman" , "139.140.192.22"),
			new Robot("shehulk" , "139.140.192.23"),
	};

	public static final Set<String> ROBOT_HOSTNAMES = new HashSet<>();
	public static final Map<String, Robot> NAME_TO_ROBOT = new HashMap<>();
	public static final Map<String, Robot> HOSTNAME_TO_ROBOT = new HashMap<>();
	public static final Map<String, Robot> IP_TO_ROBOT = new HashMap<>();
	
	static {
		for (Robot r : ROBOTS) {
			NAME_TO_ROBOT.put(r.name, r);
			
			HOSTNAME_TO_ROBOT.put(r.ethrName, r);
			HOSTNAME_TO_ROBOT.put(r.wifiName, r);
			
			ROBOT_HOSTNAMES.add(r.ethrName);
			ROBOT_HOSTNAMES.add(r.wifiName);
			
			IP_TO_ROBOT.put(r.wifiAddr, r);
			IP_TO_ROBOT.put(r.ethrAddr, r);
		}		
	}
	
	public static String[] sortedHostNames() {
		LinkedList<String> names = new LinkedList<String>(ROBOT_HOSTNAMES);
		Collections.sort(names);
		return names.toArray(new String[0]);
	}
	public static String[] sortedWifiNames() {
		LinkedList<String> names = new LinkedList<String>();
		for (Robot r : ROBOTS) {
			names.add(r.name);
		}
		Collections.sort(names);
		return names.toArray(new String[0]);
	}
	public static void main(String[] args) {
		for (String s : sortedHostNames()) {
			Debug.print("%s", s);
		}
	}
}
