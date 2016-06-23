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
import java.util.Timer;
import java.util.TimerTask;

import data.GameControlData;
import data.SPLStandardMessage;
import messages.TeamPacket;
import messages.WorldModel;
import nbtool.data.NBitesTeamBroadcast;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CommonIO.IOState;
import nbtool.util.Debug;
import nbtool.util.Robots;
import nbtool.util.ToolSettings;

public class BroadcastIO {

	public static int teamPort(int teamNum) {return 10000 + teamNum;}
	public static final String BROADCAST_ADDRESS = "10.0.255.255";
	public static final int NBITES_TEAM_PORT = teamPort(ToolSettings.NBITES_TEAM_NUM);

	public static final int GAME_CONTROL_PORT = GameControlData.GAMECONTROLLER_GAMEDATA_PORT;
	public static final int GAME_CONTROL_RECV_PORT = GameControlData.GAMECONTROLLER_RETURNDATA_PORT;

	public static TeamBroadcastInstance createTBI(TeamBroadcastListener list, int prt) {
		TeamBroadcastInstance ret = new TeamBroadcastInstance(prt);
		ret.ifr = list;

		Thread t = new Thread(ret, String.format("nbtool-%s", ret.name()));
		t.setDaemon(true);
		t.start();

		return ret;
	}

	public static class TeamBroadcastInstance extends IOInstance {

		public TeamBroadcastInstance(int port) {
			this.port = port;
		}

		private DatagramSocket datagramSocket;

		@Override
		public void run() {
			try {
	            datagramSocket = new DatagramSocket(null);
	            datagramSocket.setReuseAddress(true);
	            datagramSocket.setSoTimeout(500);
	            datagramSocket.bind(new InetSocketAddress(port));
	        } catch (SocketException e) {
	           Debug.error("TeamBroadcastListener could not bind to port %d!", port);
	           this.state = IOState.FINISHED;
	           return;
	        }

			Debug.print("%s bound and running.", name());
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
	                String name = Robots.IP_TO_ROBOT.get(address).name;

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
	            	Debug.error("TeamBroadcastListener got exception: %s", e.getMessage());
	            	e.printStackTrace();
	            }
			}
		}

		@Override
		public String name() {
			return "TeamBroadcastListener(port=" + port + ")";
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
		@Override
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
							Debug.info("BroadcastTask sending [%d] bytes from [%s].", data.length,
									provider.name());
							DatagramPacket packet = new DatagramPacket(data, data.length,
									destination, destPort);
							socket.send(packet);

						} catch (Exception e) {
							e.printStackTrace();
							Debug.error("Broadcaster exiting...");
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
