package nbtool.gui.logviews.images;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.lang.Math;

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
	public static final String BALL_IMG_X_COORD_KEY = "img_x_coord";
	public static final String BALL_IMG_Y_COORD_KEY = "img_y_coord";
	public static final String BALL_IMG_WIDTH_KEY = "img_width";
	public static final String BALL_IMG_HEIGHT_KEY = "img_height";
	
	// private static final int BALL_DISPLAY_WIDTH = 10;
	// private static final int BALL_DISPLAY_HEIGHT = 10;
	private static final int IMAGE_BUFFER = 100;

	private Ellipse2D current_ball = null;
	private boolean drawing = false;
	
	private Point pointOnImage(Point clicked) {
		if (display == null)
			return null;
		if (clicked.x < IMAGE_BUFFER || clicked.y < IMAGE_BUFFER)
			return null;
		if (clicked.x > (IMAGE_BUFFER + display.getWidth()))
			return null;
		if (clicked.y > (IMAGE_BUFFER + display.getHeight()))
			return null;
		
		return new Point(clicked.x - IMAGE_BUFFER, clicked.y - IMAGE_BUFFER);
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
			// @Override
			// public void mouseClicked(MouseEvent e) {
			// 	Point where = pointOnImage(e.getPoint());
			// 	if (where != null) {
			// 		JsonArray balls = new JsonArray();
			// 		JsonObject ball = new JsonObject();
					
			// 		ball.put(BALL_IMG_X_KEY, where.x);
			// 		ball.put(BALL_IMG_Y_KEY, where.y);
			// 		balls.add(ball);
					
			// 		displayedLog.topLevelDictionary.put(BALLS_KEY, balls);
			// 		repaint();
			// 	}
			// }

			@Override
			public void mousePressed(MouseEvent e) {
				if(!drawing) {
					Point where = pointOnImage(e.getPoint());
					if(where != null) {	
						System.out.println("Pressed: "+where.x+", "+where.y);

						current_ball = new Ellipse2D.Double(where.x, where.y, 0, 0);
						drawing = true;
						repaint();
					}
				}
			} 

			// @Override
			// public void mouseDragged(MouseEvent e) {
			// 	if(drawing) {
			// 		Point where = pointOnImage(e.getPoint());
			// 		if(where != null) {
			// 			double ball_width = where.x - current_ball.getX();
			// 			double ball_height = where.y - current_ball.getY();

			// 			current_ball.setFrame(current_ball.getX(), current_ball.getY(), ball_width, ball_height);
			// 			repaint();
			// 		}
			// 	}
			// }

			@Override
			public void mouseReleased(MouseEvent e) {
				if(drawing) {
					Point where = pointOnImage(e.getPoint());
					if(where != null) {
						System.out.println("Released: "+where.x+", "+where.y);

						int px = (int)current_ball.getX();
						int py = (int)current_ball.getY();

						int rx = (int)where.x;
						int ry = (int)where.y;

						Point origin = new Point();
						origin.x = Math.min(px,rx)-5;
						origin.y = Math.min(py,ry)-5;

						System.out.println("Origin: "+origin.x+", "+origin.y);

						double ball_width = Math.max(px,rx) - Math.min(px,rx) + 5;
						double ball_height = Math.max(py,ry) - Math.min(py,ry) + 5;

						current_ball = new Ellipse2D.Double(origin.x, origin.y, ball_width, ball_height);

						JsonArray balls = new JsonArray();
						JsonObject ball = new JsonObject();

						double ball_center_x = origin.x + ball_width/2;
						double ball_center_y = origin.y + ball_height/2;
						//double ball_radius   = current_ball.getX() 

						ball.put(BALL_IMG_X_COORD_KEY, (int)origin.x);
						ball.put(BALL_IMG_Y_COORD_KEY, (int)origin.y);

						ball.put(BALL_IMG_X_KEY, (int)ball_center_x);
						ball.put(BALL_IMG_Y_KEY, (int)ball_center_y);

						ball.put(BALL_IMG_WIDTH_KEY, (int)ball_width);
						ball.put(BALL_IMG_HEIGHT_KEY, (int)ball_height);
						balls.add(ball);

						displayedLog.topLevelDictionary.put(BALLS_KEY, balls);
						current_ball = null;
						drawing = false;
						repaint();
					}
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
		Graphics2D g2d = (Graphics2D)g;
		
		g.drawImage(display, IMAGE_BUFFER, IMAGE_BUFFER, null);
		g.setColor(Color.RED);
		
		if (this.displayedLog.topLevelDictionary.containsKey(BALLS_KEY)) {
			JsonArray balls = this.displayedLog.topLevelDictionary.get(BALLS_KEY).asArray();
			for (JsonValue ball : balls) {
				JsonObject theBall = ball.asObject();
				
				int ball_x = theBall.get(BALL_IMG_X_COORD_KEY).asNumber().asInt() + IMAGE_BUFFER;
				int ball_y = theBall.get(BALL_IMG_Y_COORD_KEY).asNumber().asInt() + IMAGE_BUFFER;
				int ball_width = theBall.get(BALL_IMG_WIDTH_KEY).asNumber().asInt();
				int ball_height = theBall.get(BALL_IMG_HEIGHT_KEY).asNumber().asInt();

				g.drawOval(ball_x, ball_y, ball_width, ball_height);
			}
		}
	}

	@Override
	public String[] displayableTypes() {
		return new String[]{SharedConstants.LogClass_Tripoint()};
	}

}
