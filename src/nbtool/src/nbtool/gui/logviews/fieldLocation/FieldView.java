package nbtool.gui.logviews.fieldLocation;

import java.awt.*;

import javax.swing.*;

import com.google.protobuf.InvalidProtocolBufferException;

import messages.RobotLocationOuterClass.RobotLocation;
import nbtool.data.Log;
import nbtool.gui.logviews.misc.ViewParent;

public class FieldView extends ViewParent {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JScrollPane sp;
	private DrawPane dPane;
	
	public float robotLocX;
	public float robotLocY;
	
	public static final Dimension fieldDimension = new Dimension((int)FieldConstants.FIELD_WIDTH+(int)FieldConstants.GREEN_PAD_X,
			(int)FieldConstants.FIELD_HEIGHT+(int)FieldConstants.GREEN_PAD_X);
	
//	public void paintComponent(Graphics g) {
//		System.out.println("FieldView PaintComponent");
//		Graphics2D g2 = (Graphics2D) g;
//		if(log == null) {
//			System.out.println("Log is null");
//			return;
//		}
//		myField.drawField(g2);
//		myPlayer.draw(g2);
//	}

	public void setLog(Log newlog) {
		log = newlog;
		if(log.primaryIsProtobuf()) {
			assert(log.primaryType().equalsIgnoreCase(
					nbtool.util.NBConstants.PROTOBUF_TYPE_PREFIX +
					"RobotLocation"));
		}
		RobotLocation ploc = null;
		try {
			ploc = RobotLocation.parseFrom(log.bytes);
		} catch (InvalidProtocolBufferException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		float x = ploc.getX();
		float y = ploc.getY();
		robotLocX = x;
		robotLocY = y;
		dPane.myPlayer.moveTo(robotLocX, robotLocY);
		System.out.println("Player created at X: "+ dPane.myPlayer.getX()+" Y: "+ dPane.myPlayer.getY());
		repaint();
	}
	
	public FieldView() {
		super();
		
		dPane = new DrawPane();
		dPane.setPreferredSize(fieldDimension);
		
		sp = new JScrollPane(dPane);
		sp.setVisible(true);
		sp.setPreferredSize(new Dimension(800,800));
	
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);

		this.add(sp);
	}

	protected void useSize(Dimension s) {
		dPane.setBounds(0,0,s.width,s.height);
		sp.setBounds(0, 0, s.width, s.height);
	}
	
	private class DrawPane extends JPanel {
		
		public Field myField;
		public Player myPlayer;
		
		public DrawPane() {
			super();
			myField = new Field();
			myPlayer = new Player();
		}
		
		@Override
		public void paintComponent(Graphics g) {
			Graphics2D g2 = (Graphics2D) g;
			myField.drawField(g2);
			myPlayer.draw(g2);
		}
	}
	
}