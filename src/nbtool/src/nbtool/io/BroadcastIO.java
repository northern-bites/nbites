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

import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.util.Logger;
import messages.TeamPacket;
import messages.WorldModel;
import data.SPLStandardMessage;

public class BroadcastIO {
	
	public static final int NBITES_TEAM_PORT = 4500;
	public static final Map<String, String> ROBOT_IPS;
	static {
		ROBOT_IPS = new HashMap<>();
		ROBOT_IPS.put("wash"   , "139.140.192.9" );
		ROBOT_IPS.put("river"  , "139.140.192.10");
		ROBOT_IPS.put("jayne"  , "139.140.192.11");
		ROBOT_IPS.put("simon"  , "139.140.192.12");
		ROBOT_IPS.put("inara"  , "139.140.192.13");
		ROBOT_IPS.put("kaylee" , "139.140.192.14");
		ROBOT_IPS.put("vera"   , "139.140.192.15");
		ROBOT_IPS.put("mal"    , "139.140.192.16");
		ROBOT_IPS.put("zoe"    , "139.140.192.17");

		ROBOT_IPS.put("ringo"  , "139.140.192.18");
		ROBOT_IPS.put("beyonce", "139.140.192.19");
		ROBOT_IPS.put("ozzy"   , "139.140.192.20");
		ROBOT_IPS.put("avril"  , "139.140.192.21");
		ROBOT_IPS.put("batman" , "139.140.192.22");
		ROBOT_IPS.put("she-hulk" , "139.140.192.23");
	}
	
	public static class TeamBroadcastListener extends IOInstance {
		
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
			
			Logger.infof("%s bound and running.", name());
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
	                
	                if (data.fromByteArray(buffer)) {
	                    //gui.update(data);
	                	Logger.printf("got packet: %d %d ", data.playerNum, data.teamNum);
	                } else {
	                	Logger.printf("got BAD packet: %d %d ", data.playerNum, data.teamNum);
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
			
	
	SPLStandardMessage message;
	TeamPacket packet;
	WorldModel model;
}
