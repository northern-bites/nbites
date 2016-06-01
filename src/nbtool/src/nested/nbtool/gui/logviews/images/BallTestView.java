package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;

import javax.swing.AbstractAction;
import javax.swing.JComponent;
import javax.swing.KeyStroke;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.JsonArray;
import nbtool.data.json.JsonObject;
import nbtool.gui.logviews.misc.ViewParent;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;

public class BallTestView extends ViewParent {
	
	private static final Debug.DebugSettings debug = Debug.createSettings(Debug.INFO);
	
	public static final String BALLS_KEY = "BallTest_Balls";
	public static final String BALL_IMG_X_KEY = "img_x";
	public static final String BALL_IMG_Y_KEY = "img_y";
	
	private static final int BALL_DISPLAY_WIDTH = 10;
	private static final int BALL_DISPLAY_HEIGHT = 10;
	
	private Point pointOnImage(Point clicked) {
		if (display == null)
			return null;
		if (clicked.x < 100 || clicked.y < 100)
			return null;
		if (clicked.x > (100 + display.getWidth()))
			return null;
		if (clicked.y > (100 + display.getHeight()))
			return null;
		
		return new Point(clicked.x - 100, clicked.y - 100);
	}

	BufferedImage display = null;
	
	public BallTestView() {
		super();
	}
	
	@Override
	public void disappearing() {
		try {
			displayedLog.saveChangesToTempFile();
		} catch (Exception e1) {
			e1.printStackTrace();
		}
	}
	
	@Override
	public void setupDisplay() {
		this.display = this.displayedLog.blocks.get(0).parseAsYUVImage().toBufferedImage();
		
		this.addMouseListener(new MouseAdapter(){
			@Override
			public void mouseClicked(MouseEvent e) {
				Point where = pointOnImage(e.getPoint());
				if (where != null) {
					JsonArray balls = new JsonArray();
					JsonObject ball = new JsonObject();
					
					ball.put(BALL_IMG_X_KEY, where.x);
					ball.put(BALL_IMG_Y_KEY, where.y);
					balls.add(ball);
					
					displayedLog.topLevelDictionary.put(BALLS_KEY, balls);
					repaint();
				}
			}
		});
		
		this.getActionMap().put("BallTestClearBalls", new AbstractAction(){
			@Override
			public void actionPerformed(ActionEvent e) {
				debug.info("clearing balls...");
				displayedLog.topLevelDictionary.put(BALLS_KEY, new JsonArray());
				repaint();
			}
		});
		
		this.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
				KeyStroke.getKeyStroke(KeyEvent.VK_SPACE, 0)
				, "BallTestClearBalls");		
	}
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		
		g.drawImage(display, 100, 100, null);
		g.setColor(Color.RED);
		
		if (this.displayedLog.topLevelDictionary.containsKey(BALLS_KEY)) {
			JsonArray balls = this.displayedLog.topLevelDictionary.get(BALLS_KEY).asArray();
			for (JsonValue ball : balls) {
				JsonObject theBall = ball.asObject();
				
				int ball_x = theBall.get(BALL_IMG_X_KEY).asNumber().asInt() + 100;
				int ball_y = theBall.get(BALL_IMG_Y_KEY).asNumber().asInt() + 100;
				
				g.drawOval(ball_x - (BALL_DISPLAY_WIDTH / 2),
						ball_y - (BALL_DISPLAY_HEIGHT / 2),
						BALL_DISPLAY_WIDTH, BALL_DISPLAY_HEIGHT);
			}
		}
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint()};
	}

}
