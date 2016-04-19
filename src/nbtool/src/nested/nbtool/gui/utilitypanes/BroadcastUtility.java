package nbtool.gui.utilitypanes;

import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.SocketException;
import java.net.UnknownHostException;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.io.BroadcastIO;
import nbtool.io.BroadcastIO.BroadcastDataProvider;
import nbtool.io.BroadcastIO.Broadcaster;

public class BroadcastUtility extends UtilityParent {

	private BU_Frame display = null;
	@Override
	public JFrame supplyDisplay() {
		return (display == null) ? display = new BU_Frame() : display;
	}

	@Override
	public String purpose() {
		return "repeatedly generating broadcast packets";
	}

	@Override
	public char preferredMemnonic() {
		// TODO Auto-generated method stub
		return 'b';
	}
	
	private BroadcastDataProvider[] providers = {
			new BroadcastDataProvider() {

				@Override
				public byte[] provideBroadcast() {
					return new byte[]{0, 1, 2, 3, 4};
				}

				@Override
				public String name() {
					return "sequential 5";
				}
				
			}, 
			
			new BroadcastDataProvider() {

				@Override
				public byte[] provideBroadcast() {
					return new byte[1000];
				}

				@Override
				public String name() {
					return "empty 1000";
				}
				
			}
	};
	
	private class BU_Frame extends JFrame {
		
		Broadcaster broadcaster;
		JButton selected = null;
		
		private Border unsel = BorderFactory.createEmptyBorder();
		private Border sel = BorderFactory.createBevelBorder(BevelBorder.RAISED);
		
		public BU_Frame() {
			super("BroadcastUtility");
			
			try {
				broadcaster = new Broadcaster(BroadcastIO.BROADCAST_ADDRESS, BroadcastIO.NBITES_TEAM_PORT);
			} catch (SocketException e) {
				e.printStackTrace();
				return;
			} catch (UnknownHostException e) {
				e.printStackTrace();
				return;
			}
			
			Container content = this.getContentPane();
			content.setLayout(new GridLayout(providers.length + 1, 1));
			
			for (int i = 0; i < providers.length; ++i) {
				JButton button = new JButton(providers[i].name());
				button.setName("" + i);
				button.setBorder(unsel);
				button.addActionListener(new ActionListener(){

					@Override
					public void actionPerformed(ActionEvent e) {
						JButton btn = (JButton) e.getSource();
						int j = Integer.parseInt(btn.getName());
						
						broadcaster.running = true;
						broadcaster.provider = providers[j];
						
						if (selected != null) {
							selected.setBorder(unsel);
						}
						
						btn.setBorder(sel);
						selected = btn;	
					}
					
				});
				content.add(button);
			}
			
			final JSlider slider = new JSlider(0, 5000);
			slider.setMajorTickSpacing(500);
			slider.setPaintLabels(true);
			slider.addChangeListener(new ChangeListener() {

				@Override
				public void stateChanged(ChangeEvent e) {
					broadcaster.interim = slider.getValue();
				}
				
			});
			content.add(slider);
			
			this.setSize(300,600);
		}
		
	}

}
