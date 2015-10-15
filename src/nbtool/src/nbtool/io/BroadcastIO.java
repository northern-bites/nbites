package nbtool.io;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Timer;
import java.util.TimerTask;

import nbtool.data.NBitesTeamBroadcast;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import messages.TeamPacket;
import messages.WorldModel;
import data.GameControlData;
import data.SPLStandardMessage;

public class BroadcastIO {
	
	public static int teamPort(int teamNum) {return 10000 + teamNum;}
	public static final String BROADCAST_ADDRESS = "10.0.255.255";
	public static final int NBITES_TEAM_PORT = teamPort(NBConstants.NBITES_TEAM_NUM);
	
	public static final int GAME_CONTROL_PORT = GameControlData.GAMECONTROLLER_GAMEDATA_PORT;
	public static final int GAME_CONTROL_RECV_PORT = GameControlData.GAMECONTROLLER_RETURNDATA_PORT;
	
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
	                
	                NBitesTeamBroadcast tb = new NBitesTeamBroadcast(name, address,
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
		public void acceptTeamBroadcast(NBitesTeamBroadcast tb);
	}
	
	public static abstract class BroadcastDataProvider {
		/* somewhat arbitrary, based on underlying datagram protocol */
		public static final int MAX_BROADCAST_SIZE = 1200;
		
		public abstract byte[] provideBroadcast();
		public abstract String name();
	}
	
	public static abstract class SPLMessageProvider extends BroadcastDataProvider {
		public byte[] provideBroadcast() {
			return provideMessage().toByteArray();
		}
		
		public abstract SPLStandardMessage provideMessage();
	}
	
	public static class Broadcaster {
		//may NOT be set to null.
		public volatile BroadcastDataProvider provider;
		public volatile int interim;
		public volatile boolean running;
		
		public Broadcaster(String address, int port) throws SocketException, UnknownHostException {
			this.provider = null;
			this.interim = 1000;
			this.running = false;
			
			this.socket = new DatagramSocket();
			this.destination = InetAddress.getByName(address);
			this.destPort = port;
			
			this.timer = new Timer("Broadcaster-timer", true);
			this.timer.schedule(new BroadcastTask(), 1000);
		}
		
		private DatagramSocket socket;
		private InetAddress destination;
		private int destPort;
		
		private Timer timer;
		
		private class BroadcastTask extends TimerTask {
			@Override
			public void run() {
				if (running) {
					byte[] data = provider.provideBroadcast();
					if (data != null 
							&& data.length < BroadcastDataProvider.MAX_BROADCAST_SIZE) {
						try {
							Logger.infof("BroadcastTask sending [%d] bytes from [%s].", data.length,
									provider.name());
							DatagramPacket packet = new DatagramPacket(data, data.length,
									destination, destPort);
							socket.send(packet);
							
						} catch (Exception e) {
							e.printStackTrace();
							Logger.errorf("Broadcaster exiting...");
							return;
						}
						
						timer.schedule(new BroadcastTask(), interim);
						return;
					}
				}
				
				timer.schedule(new BroadcastTask(), 1000);
			}
			
		}
	}
	
}
