package nbtool.gui.utilitypanes;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import com.jogamp.nativewindow.util.Dimension;

import nbtool.data.Log;
import nbtool.data.TeamBroadcast;
import nbtool.io.BroadcastIO;
import nbtool.io.BroadcastIO.TeamBroadcastInstance;
import nbtool.io.BroadcastIO.TeamBroadcastListener;
import nbtool.io.CommonIO.IOInstance;
import nbtool.util.Logger;

public class WorldViewUtility extends UtilityParent {
	
	private class WorldView extends JFrame implements TeamBroadcastListener, ActionListener
	{
		private TeamBroadcastInstance broadcastListener;
		
		protected WorldView() {
			super();
			Logger.println("WorldView creating broadcast listener...");
			this.broadcastListener = BroadcastIO.createTBI(this);
			this.setTitle("WorldView");
			initiate();
		}

		@Override
		public void ioFinished(IOInstance instance) {
			// listener died for some reason.
		}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			Logger.println("recvd...");
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return false;
		}

		@Override
		public void acceptTeamBroadcast(TeamBroadcast tb) {
			//use tb
			Logger.printf("got from {%s:%s}", tb.robotName, tb.robotIp);
			Logger.printf("Team: %s, Player: %s",tb.dataTeamPacket.getTeamNumber(), tb.dataTeamPacket.getPlayerNumber());
			if(runWorldview) {
				if(tb.dataTeamPacket.getTeamNumber() == teamNumber) {
					ar[tb.dataTeamPacket.getPlayerNumber()-1] = new NaoRobot();
					ar[tb.dataTeamPacket.getPlayerNumber()-1].wvNao(tb);
					this.repaint();
				}
			}
		}
		
		private void initiate() {
			flip = new JButton("flip");
			flip.addActionListener(this);
			flip.setBounds((int)FieldConstants.FIELD_GREEN_WIDTH,10,200,25);
			
			teamNumberInput = new JTextField("0",2);
			teamNumberInput.setBounds((int)FieldConstants.FIELD_GREEN_WIDTH, 40, 200, 25);
			
			startWorldView = new JButton("Start WorldView");
			startWorldView.addActionListener(this);
			startWorldView.setBounds((int)FieldConstants.FIELD_GREEN_WIDTH,70,200,25);
			
			panel = new JPanel() {
				@Override
				public void paintComponent(Graphics g) {
					super.paintComponent(g);
					Graphics2D g2 = (Graphics2D) g;
					f.drawField(g2, shouldFlip);
					for(int i=0; i<ar.length; i++) {
						if(ar[i] != null) {
							ar[i].drawNao(g2, shouldFlip);	
						}
					}
				}
			};

			add(flip);
			add(teamNumberInput);
			add(startWorldView);
			add(panel);

			this.validate();
			this.repaint();
		}

		Field f = new Field();
		JPanel panel;
		NaoRobot ar[] = new NaoRobot[5];
		private JButton flip;
		private JButton startWorldView;
		private boolean shouldFlip = false;
		private boolean runWorldview = false;
		private JTextField teamNumberInput;
		private int teamNumber;

		@Override
		public void actionPerformed(ActionEvent e) {
			if(e.getSource() == flip) {
				if(shouldFlip) {
					shouldFlip = false;
					flip.setText("flip");
				} else {
					shouldFlip = true;
					flip.setText("unflip");
				}
			}
			if(e.getActionCommand() == "Start WorldView") {
				runWorldview = true;
				if(teamNumberInput.getText() != null) {
					try {
						teamNumber = Integer.parseInt(teamNumberInput.getText());
						startWorldView.setText("Stop");
						Logger.infof("Now listening to team %s", teamNumber);
					} catch(NumberFormatException E) {
						E.printStackTrace();
					}
				}
			} else if(e.getActionCommand() == "Stop") {
				runWorldview = false;
				startWorldView.setText("Start WorldView");
				for(int i=0; i<ar.length; i++) {
					ar[i] = null;
				}
			}
		}
	}

	private WorldView display = null;
	@Override
	public JFrame supplyDisplay() {
		return (display != null) ? display : (display = new WorldView());
	}

	@Override
	public String purpose() {
		return "Listen to and display broadcast messages.";
	}

	@Override
	public char preferredMemnonic() {
		return 'w';
	}
}
