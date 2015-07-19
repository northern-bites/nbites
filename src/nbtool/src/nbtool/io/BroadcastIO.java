package nbtool.io;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import nbtool.data.TeamBroadcast;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.util.Logger;
import messages.TeamPacket;
import messages.WorldModel;
import data.SPLStandardMessage;

public class BroadcastIO {
	
	//public static final int NBITES_TEAM_PORT = 4500;
	public static final int NBITES_TEAM_PORT = 10014;
	public static final Map<String, String> ROBOT_TO_IP;
	public static final Map<String, String> IP_TO_ROBOT;
	static {
		ROBOT_TO_IP = new HashMap<>();
		IP_TO_ROBOT = new HashMap<>();
		ROBOT_TO_IP.put("wash"   , "139.140.192.9" );
		ROBOT_TO_IP.put("river"  , "139.140.192.10");
		ROBOT_TO_IP.put("jayne"  , "139.140.192.11");
		ROBOT_TO_IP.put("simon"  , "139.140.192.12");
		ROBOT_TO_IP.put("inara"  , "139.140.192.13");
		ROBOT_TO_IP.put("kaylee" , "139.140.192.14");
		ROBOT_TO_IP.put("vera"   , "139.140.192.15");
		ROBOT_TO_IP.put("mal"    , "139.140.192.16");
		ROBOT_TO_IP.put("zoe"    , "139.140.192.17");

		ROBOT_TO_IP.put("ringo"  , "139.140.192.18");
		ROBOT_TO_IP.put("beyonce", "139.140.192.19");
		ROBOT_TO_IP.put("ozzy"   , "139.140.192.20");
		ROBOT_TO_IP.put("avril"  , "139.140.192.21");
		ROBOT_TO_IP.put("batman" , "139.140.192.22");
		ROBOT_TO_IP.put("she-hulk" , "139.140.192.23");
		
		for (Entry<String, String> entry : ROBOT_TO_IP.entrySet()) {
			IP_TO_ROBOT.put(entry.getValue(), entry.getKey());
		}
	}
	
	public static TeamBroadcastInstance createTBI(TeamBroadcastListener list) {
		TeamBroadcastInstance ret = new TeamBroadcastInstance();
		ret.ifr = list;
		
		Thread t = new Thread(ret, String.format("nbtool-%s", ret.name()));
		t.setDaemon(true);
		t.start();
		
		return ret;
	}
	
	public static class TeamBroadcastInstance extends IOInstance {
		
		private DatagramSocket datagramSocket;

		@Override
		public void run() {
			try {
	            datagramSocket = new DatagramSocket(null);
	            datagramSocket.setReuseAddress(true);
	            datagramSocket.setSoTimeout(500);
	            datagramSocket.bind(new InetSocketAddress(NBITES_TEAM_PORT));
	        } catch (SocketException e) {
	           Logger.errorf("TeamBroadcastListener could not bind to port %d!", NBITES_TEAM_PORT);
	           this.state = IOState.FINISHED;
	           return;
	        }
			
			Logger.printf("%s bound and running.", name());
			synchronized(this) {
				if (this.state != IOState.STARTING)
					return;
				this.state = IOState.RUNNING;
			}
			
			while (state() == IOState.RUNNING) {
				final ByteBuffer buffer = ByteBuffer.wrap(new byte[SPLStandardMessage.SIZE]);
	            final SPLStandardMessage data = new SPLStandardMessage();
	            final DatagramPacket packet = new DatagramPacket(buffer.array(), buffer.array().length);
	            
	            try {
	                datagramSocket.receive(packet);
	                buffer.rewind();
	                
	                String address = packet.getAddress().getHostAddress();
	                String name = IP_TO_ROBOT.get(address);
	                
	                data.fromByteArray(buffer);
	                TeamPacket tp = TeamPacket.parseFrom(data.data);
	                WorldModel wm = tp.getPayload();
	                
	                TeamBroadcast tb = new TeamBroadcast(name, address,
	                		data, tp, wm);
	                
	                if (this.ifr instanceof TeamBroadcastListener) {
	                	((TeamBroadcastListener) this.ifr).acceptTeamBroadcast(tb);
	                } else {
	                	ifr.ioReceived(this, 0, tb.toLog());
	                }
	                
	            } catch (SocketTimeoutException e) { // ignore, because we set a timeout
	            } catch (IOException e) {
	            	Logger.errorf("TeamBroadcastListener got exception: %s", e.getMessage());
	            	e.printStackTrace();
	            }
			}
		}

		@Override
		public String name() {
			return "TeamBroadcastListener(port=" + NBITES_TEAM_PORT + ")";
		}
	}
	
	public static interface TeamBroadcastListener extends IOFirstResponder {
		public void acceptTeamBroadcast(TeamBroadcast tb);
	}
}
