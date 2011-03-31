package TOOL.Image;

import TOOL.TOOL;


import TOOL.PlayBookEditor.*;
import TOOL.Data.Field;
import java.awt.geom.*;
import java.awt.geom.Ellipse2D.Float;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.Math;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Stack;
import javax.swing.event.*;
import javax.swing.undo.*;

/**
 * GUI for the PlaybookEditor.
 * Aims to emulate the drawing interface of Microsoft Word or similar programs.
 *
 * Current undoable/redoable actions:
 * Adding zone(s)
 * Copying/pasting zone(s)
 * Moving zone(s)
 * Deleting zone(s)
 * Resizing a zone
 * Clearing entire screen
 *
 * To come:
 * Relabeling zone(s)
 *
 * @author Nicholas Dunn
 * @date   April 10, 2008
 */
public class PlayBookEditorViewer extends ImagePanel implements
                                                         MouseListener,
                                                         MouseMotionListener,
                                                         KeyListener,
                                                         KeyEventDispatcher
{

    // Keep track of xy locations to draw
    protected int x1,y1,x2,y2;
    protected boolean drawing, movingGroup, mousePressed;
    protected ArrayList<PlayBookObject> curZones;
    protected LinkedList<PlayBookObject> clipBoard;
    protected LinkedList<PlayBookObject> curSelections;

    protected int index;

    protected int startX, startY;
    protected Point oldResizePoint;

    protected PlayBookObject curResizing;
    protected Direction dir;

    // Any slope whose absolute value is < .3 we will make a straight horizontal
    // line if shift is held down while drawing
    protected static final double NEAR_HORIZONTAL_SLOPE = 0.3;

    protected static final AlphaComposite NORMAL_ALPHA =
        AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .4F);
    protected static final AlphaComposite SELECTED_ALPHA =
        AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .7F);
    protected static final AlphaComposite OPAQUE =
        AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0F);
    protected static final AlphaComposite HANDLE_ALPHA = OPAQUE;

    protected static final Color ATTACK_COLOR = Color.WHITE;
    protected static final Color DEFEND_COLOR = Color.BLACK;
    protected static final Color TEXT_COLOR = Color.BLACK;

    protected static final Color HANDLE_FILL_COLOR = Color.WHITE;
    protected static final Color HANDLE_OUTLINE_COLOR = Color.BLACK;
    protected static final Color BALL_COLOR = new Color(255, 160, 0);

    protected static final BasicStroke BORDER_STROKE = new BasicStroke(2);
    protected static final BasicStroke NORMAL_STROKE = new BasicStroke(1);

    protected static final int HANDLE_RADIUS = 4;
    protected static final int HANDLE_WIDTH = 2 * HANDLE_RADIUS;
    protected static final int ARROW_MOVE_AMT = 1;

    protected static final int MINIMUM_ZONE_WIDTH = 5;
    protected static final int MINIMUM_ZONE_HEIGHT = MINIMUM_ZONE_WIDTH;

    protected static final Cursor HAND_CURSOR = Cursor.
        getPredefinedCursor(Cursor.HAND_CURSOR);
    protected static final Cursor MOVE_CURSOR = Cursor.
        getPredefinedCursor(Cursor.MOVE_CURSOR);
    protected static final Cursor DEFAULT_CURSOR = Cursor.
        getDefaultCursor();
    protected static final Cursor CROSSHAIR_CURSOR = Cursor.
        getPredefinedCursor(Cursor.CROSSHAIR_CURSOR);

    // Move down and to the right
    protected static final int PASTE_X_OFFSET = 10;
    protected static final int PASTE_Y_OFFSET = -PASTE_X_OFFSET;

    protected static final Font LABEL_FONT =
        new Font("SansSerif", Font.PLAIN, 8);

    protected UndoManager undoManager;         // history list
    protected UndoableEditSupport undoSupport; // event support

    protected int BALL_RADIUS = 12, POINT_RADIUS = 12;

    protected boolean ballVisible = false, testMode = false;
    protected Point[] robotPos;
    protected Color ROBOT_COLOR = Color.BLUE;
    protected int ROBOT_RADIUS = 6;

    /**
     * An enumeration that holds both a cursor and an integer representing that
     * direction.  Used for resizing our zones.
     */
    public enum Direction {
        N (Cursor.N_RESIZE_CURSOR),
            NE (Cursor.NE_RESIZE_CURSOR),
            E (Cursor.E_RESIZE_CURSOR),
            SE (Cursor.SE_RESIZE_CURSOR),
            S (Cursor.S_RESIZE_CURSOR),
            SW (Cursor.SW_RESIZE_CURSOR),
            W (Cursor.W_RESIZE_CURSOR),
            NW (Cursor.NW_RESIZE_CURSOR);
        protected Cursor cursor;
        protected int dir;
        Direction(int dir) {
            this.dir = dir;
            cursor = Cursor.getPredefinedCursor(dir);
        }
        public int getDirection() { return dir; }
        public Cursor getCursor() { return cursor; }

    };

    public enum DrawingMode { LINE, ZONE, POINT };
    protected DrawingMode mode;
    protected Field field;

    protected TOOL tool;
    protected PlayBookEditor editor;

    // Number of pixels away from a corner or edge in order to start resize mode
    public static final int CHECKERBOARD_DIST_FOR_RESIZE = 10;

    /**
     * Constructs a viewer for the given field, within the given TOOL program.
     * @param tool the instance of the TOOL program in which we do all our
     *        drawing.  Necessary because we must change the cursor when
     *        resizing.
     * @param field the Field object we are drawing on the screen.
     */
    public PlayBookEditorViewer(TOOL tool, Field field, PlayBookEditor editor) {
        super();
        this.tool = tool;
        this.editor = editor;
        setField(field);
        curZones = new ArrayList<PlayBookObject>();
        drawing = false;
        curSelections = new LinkedList<PlayBookObject>();
        clipBoard = new LinkedList<PlayBookObject>();
        curResizing = null;
        dir = null;
        addMouseMotionListener(this);
        x2 = -1;
        mode = DrawingMode.ZONE;
        // We already have the mouse listener inherited from the super class


        undoManager = new UndoManager();         // history list
        undoSupport = new UndoableEditSupport();
        undoSupport.addUndoableEditListener(new UndoAdapter());

	robotPos = new Point[4];
	for(int i = 0; i < 4; i++) {
	    robotPos[i] = null;
	}
    }

    /**
     * Paints the field, any zones previously classified, as well as the zone
     * the user is currently defining.
     */
    public void paint(Graphics g) {
        // Paint the field
        super.paint(g);

        Graphics2D g2 = (Graphics2D) g;
        // Paint the zones previously defined
        // draw as semi transparent

        g2.scale(xImageScale, yImageScale);
        for (PlayBookObject unselected: curZones) {
            // Don't paint the currently selected ones until afterwards so that
            // they appear on top of these unselected ones
            if (!curSelections.contains(unselected)) {

                g2.setComposite(NORMAL_ALPHA);
                Shape shape = unselected.getShape();
                g2.setColor(ATTACK_COLOR);
                g2.fill(shape);
                // Draw the border thick and in a constrasting color
                g2.setStroke(BORDER_STROKE);
                g2.setComposite(OPAQUE);
                g2.setColor(new Color(ATTACK_COLOR.getRGB() ^ 0xFFFFFF));
                g2.draw(shape);
            }

        }

        // The currently selected zones should always be drawn on top of
        // anything else, hence drawn AFTER the unselected ones
        for (PlayBookObject z : curSelections) {
            Shape shape = z.getShape();

            g2.setColor(ATTACK_COLOR);
            g2.setStroke(NORMAL_STROKE);
            g2.setComposite(SELECTED_ALPHA);
            g2.fill(shape);

            // Draw the border thick and in a constrasting color
            g2.setStroke(BORDER_STROKE);
            g2.setComposite(OPAQUE);
            g2.setColor(new Color(ATTACK_COLOR.getRGB() ^ 0xFFFFFF));
            g2.draw(shape);

            g2.setStroke(NORMAL_STROKE);
            drawHandles(g2, shape);


            // Draw the label in the center of the image
            FontMetrics fontInfo = g2.getFontMetrics(LABEL_FONT);
            String label = "" + z.getID();
            Rectangle2D stringBounds = fontInfo.getStringBounds(label, g2);

            int centerX = (int) shape.getBounds().getX() +
                (int) (shape.getBounds().getWidth() / 2) -
                (int) (stringBounds.getWidth() / 2);
            int centerY = (int) shape.getBounds().getY() +
                (int) (shape.getBounds().getHeight() / 2) +
                (int) (stringBounds.getHeight()/2);

            g2.setColor(TEXT_COLOR);
            g2.drawString(label, centerX, centerY);
        }

	if (ballVisible) {
	    g2.setColor(BALL_COLOR);
	    int ballX = field.fieldToScreenX(editor.getBall().getX()),
		ballY = field.fieldToScreenY(editor.getBall().getY());
	    g2.fillOval(ballX-BALL_RADIUS/2,ballY-BALL_RADIUS/2,
			BALL_RADIUS, BALL_RADIUS);
	}

	for(int i = 0; i < robotPos.length; i++) {
	    if (robotPos[i] == null)
		continue;
	    drawRobot(g2, robotPos[i]);
	}

        // Paint the zone user is currently defining
        if (!drawing) { return; }

        g2.setStroke(BORDER_STROKE);
        g2.setColor(ATTACK_COLOR);
        // Put the scale back to normal for drawing what user is defining
        g2.setTransform(new AffineTransform());

        int leftX = Math.min(x1, x2);
        int topY = Math.min(y1, y2);
        int width = Math.abs(x2 - x1);
        int height = Math.abs(y2 - y1);

        switch (mode) {
        case ZONE:
            g2.drawRect(leftX, topY, width, height);
	    g2.setColor(TEXT_COLOR);
	    g2.drawString("" + curZones.size(), leftX + width/2,
			  topY + height/2);
            break;
        case LINE:
            g2.drawLine(x1, y1, x2, y2);
	    g2.setColor(TEXT_COLOR);
	    g2.drawString("" + curZones.size(), leftX + width/2,
			  topY + height/2);
            break;
	case POINT:
	    g2.draw(new Ellipse2D.Float(x2-POINT_RADIUS/2, y2-POINT_RADIUS/2,
					POINT_RADIUS, POINT_RADIUS));
	    g2.setColor(TEXT_COLOR);
	    g2.drawString("" + curZones.size(), x2+POINT_RADIUS, y2+POINT_RADIUS);
	    break;
        default:
            TOOL.CONSOLE.error("Error, invalid mode in PlayBookEditorViewer");
	}

    }

    public void drawHandles(Graphics2D g2, Shape s) {
	String ellString = "java.awt.geom.Ellipse2D";
	try {
	    //System.out.println(s.getClass().getEnclosingClass().getName());
	    //System.out.println(Class.forName(ellString).getName());
	    if(s.getClass().getEnclosingClass().getName(
               ).equals(Class.forName(ellString).getName())) {
		return;
	    }
	} catch (Exception e) {
	    //System.out.println("Class not found...");
	    // We don't have a line, instead something else...
	}
        Color prevColor = g2.getColor();
        Composite prevAlpha = g2.getComposite();

        // Bounding box for the shape
        Rectangle r = s.getBounds();
        int width = (int) r.getWidth();
        int height = (int) r.getHeight();
        int leftX = (int) r.getX();
        int rightX = leftX + width;
        int topY = (int) r.getY();
        int bottomY = topY + height;

        g2.setComposite(HANDLE_ALPHA);

        // Top
        drawHandle(g2, (rightX + leftX) / 2, topY);
        // upper right
        drawHandle(g2, rightX, topY);
        // right
        drawHandle(g2, rightX, (topY + bottomY) / 2);
        // lower right
        drawHandle(g2, rightX, bottomY);
        // bottom
        drawHandle(g2, (rightX + leftX) / 2, bottomY);
        // lower left
        drawHandle(g2, leftX, bottomY);
        // left
        drawHandle(g2, leftX, (topY + bottomY) / 2);
        // upper left
        drawHandle(g2, leftX, topY);


        g2.setColor(prevColor);
        g2.setComposite(prevAlpha);
    }

    public void updateRobotPos(Point p, int i)
    {
	robotPos[i] = p;
	repaint();
    }

    public void drawRobot(Graphics2D g2, Point p)
    {
	int sX, sY;
	sX = field.fieldToScreenX(p.x);
	sY = field.fieldToScreenY(p.y);
	g2.setColor(ROBOT_COLOR);
	g2.fillOval(sX - ROBOT_RADIUS, sY - ROBOT_RADIUS,
		    ROBOT_RADIUS*2, ROBOT_RADIUS*2);
	g2.setColor(Color.BLACK);
	g2.drawOval(sX - ROBOT_RADIUS, sY - ROBOT_RADIUS,
		    ROBOT_RADIUS*2, ROBOT_RADIUS*2);
    }

    /** Draws a handle centered at x and y */
    protected void drawHandle(Graphics2D g2, int x, int y) {
        g2.setColor(HANDLE_FILL_COLOR);
        g2.fillRect(x - HANDLE_RADIUS, y - HANDLE_RADIUS,
                    HANDLE_WIDTH, HANDLE_WIDTH);
        g2.setColor(HANDLE_OUTLINE_COLOR);
        g2.drawRect(x - HANDLE_RADIUS, y - HANDLE_RADIUS,
        HANDLE_WIDTH, HANDLE_WIDTH);
    }

    /**
     * Intercept a user's mouse press within the window.
     * Does one of three things:
     * If you are resizing a zone (defined as being close enough to the edge
     * or corner of a zone), returns without doing anything
     * If you press within the bounds of a previously defined zone, sets a
     * global variable for the currently selected zone and returns.
     * Else, all it does is update global variables for the location of the
     * cursor and signals the paint method that we are drawing a shape
     */
    public void mousePressed(MouseEvent e) {
        mousePressed = true;

	if (testMode) {
	    int x = (int)(e.getX()/xImageScale),
		y = (int)(e.getY()/yImageScale);
	    x = field.screenToFieldX(x);
	    y = field.screenToFieldY(y);
	    editor.getTestPanel().changeBallLoc(new Point(x, y));
	    return;
	}

        // We're resizing something, don't start drawing a new one
        if (curResizing != null) {
            tool.setCursor(CROSSHAIR_CURSOR);
            return;
        }

        x1 = (int)e.getX();
        y1 = (int)e.getY();
        x2 = x1; y2 = y1;

        int j = 0;

        // Check to see if they clicked within any of the zones
        for (PlayBookObject cur: curZones) {

            if (cur.contains((int) (e.getX()/xImageScale),
                             (int) (e.getY()/yImageScale))) {
                tool.setCursor(HAND_CURSOR);
                movingGroup = true;

                // Shift was down
                if (e.isShiftDown()) {

                    // They already had it selected; unselect it
                    if (curSelections.contains(cur)) {
                        curSelections.remove(cur);
                        repaint(cur.getShape());
                    }
                    // They didn't have it selected; add it to selection
                    else {
                        curSelections.addLast(cur);
                        repaint(cur.getShape());
                    }
                }
                // Shift was NOT down
                else {
                    // They already had it selected; they're going to drag
                    if (curSelections.contains(cur)) {

                    }
                    // They did not have it selected; unselect everything
                    // and make the new thing the only one selected
                    else {
                        for (int i = curSelections.size() - 1; i > -1; i--) {
                            PlayBookObject z = curSelections.remove(i);
                            repaint(z.getShape());
                        }
                        index = j;
                        curSelections.addLast(cur);
                        repaint(cur.getShape());

                    }
                }

                if (!curSelections.isEmpty()) {
                    startX = curSelections.getFirst().getX1();
                    startY = curSelections.getFirst().getY1();
                }

                editor.refreshButtons();
                editor.setLabelText("Current Selections: " + curSelections);
                return;
            }
            j++;
        }

        editor.setLabelText("Current Selections: " + curSelections);
        tool.setCursor(DEFAULT_CURSOR);

        // We didn't click in anything else, so we're drawing a new zone
        movingGroup = false;
        drawing = true;

        editor.refreshButtons();
        repaint();
    }


    /**
     * Deals with the user releasing the mouse within the confines of this
     * panel.
     *
     * Again, three possibilities:
     * If we were resizing something, stops resizing it, and fixes the
     * coordinates of the zone to ensure that (x1, y1) refers to the upper
     * left corner of the rectangle
     * If we were moving a zone around, just stops doing so and changes the
     * global variable associated with the one being moved
     * Else if we were drawing a zone, this method determines the upper left
     * and lower right coordinates of the rectangle drawn and makes a new zone.
     */
    public void mouseReleased(MouseEvent e) {
        mousePressed = false;
	if (testMode) {
	    int x = (int)(e.getX()/xImageScale),
		y = (int)(e.getY()/yImageScale);
	    x = field.screenToFieldX(x);
	    y = field.screenToFieldY(y);
	    editor.getTestPanel().changeBallLoc(new Point(x, y));
	    return;
	}

        x2 = clipX(e.getX());
        y2 = clipY(e.getY());

        if (e.isShiftDown()) {
            constrainProportions();
        }


        drawing = false;
        // We just finished resizing a zone
        if (curResizing != null) {

            //System.out.println("Finished resizing");

            curResizing.fixCoords();
            int fieldX = field.fixXCoord(e.getX() / xImageScale);
            int fieldY = field.fixYCoord(e.getY() / yImageScale);
            Point curPoint = new Point(fieldX, fieldY);
            Direction d = getDirection(curResizing, curPoint);
            if (d != null) { tool.setCursor(d.getCursor()); }

            resize(curResizing, dir.getDirection(), oldResizePoint, curPoint);
        }

        else if (movingGroup) {

            // Calculate how far they moved the group.
            int endX = curSelections.getFirst().getX1();
            int endY = curSelections.getFirst().getY1();
            int dx = endX - startX;
            int dy = endY - startY;

            if (dx == 0 && dy == 0) {
                if (!contains(curSelections, (int) (x2/xImageScale),
                              (int) (y2/yImageScale))) {
                    curSelections.clear();
                    repaint();
                    updateLabel();
                    return;
                }
                return;
            }
            else {
                //System.out.println(curSelections + ":creating a move undo command for moving " + dx + " x and " + dy + " dy.");
                move(curSelections, dx, dy);
            }

        }
        // We just finished defining/drawing a zone
        else {
            //System.out.println("Finished drawing a zone");

            x1 = field.fixXCoord(x1 / xImageScale);
            x2 = field.fixXCoord(x2 / xImageScale);
            y1 = field.fixYCoord(y1 / yImageScale);
            y2 = field.fixYCoord(y2 / yImageScale);
            PlayBookObject newZone = null;
            int leftX = Math.min(x1, x2);
            int rightX = Math.max(x1, x2);
            int topY = Math.max(y1, y2);
            int bottomY = Math.min(y1, y2);
            int width = rightX - leftX;
            int height = topY - bottomY;
            boolean tooSmall = false;

            switch (mode) {
            case ZONE:
                if (Zone.validSize(width, height)) {
                    newZone = new Zone(Integer.toString(curZones.size()), leftX,
				       topY, rightX, bottomY, field);
                }
                else { tooSmall = true; }
                break;
            case LINE:
                if (Line.validSize(width, height)) {
                    newZone = new Line(Integer.toString(curZones.size()), x1,
				       y1, x2, y2, field);
                    break;
                } // too small
                else { tooSmall = true; }
                break;
	    case POINT:
		newZone = new PBEPoint(Integer.toString(curZones.size()), x2,
				       y2, field);
	     	break;
            default:
                return;
            }

            if (tooSmall) {
                curSelections.clear();
                repaint();
                editor.refreshButtons();
                updateLabel();
                return;
            }

            add(newZone);

            // We want our index to correspond to the index of this last thing
            // we added, which is at index size - 1.
            index = curZones.size() - 1;
            repaint(newZone.getShape());
        }

        editor.refreshButtons();
    }

    public void updateLabel() {
        editor.setLabelText("Current Selections: " + curSelections);
    }

    public void repaint(Shape s) {
        Rectangle r = s.getBounds();
        int x1 = (int) r.getX() - HANDLE_WIDTH;
        int y1 = (int) r.getY() - HANDLE_WIDTH;
        int width = (int) r.getWidth() + 4 * HANDLE_WIDTH;
        int height = (int) r.getHeight() + 4 * HANDLE_WIDTH;

        repaint((int) (xImageScale * x1),
                (int) (yImageScale * y1),
                (int) (xImageScale * width),
                (int) (yImageScale * height));

    }



    public boolean canCopy() {
        return !curSelections.isEmpty();
    }

    public boolean canPaste() {
        return !clipBoard.isEmpty();
    }

    /**
     * @return true if and only if initiating a clear action would change
     * the state of the system
     */
    public boolean canClear() {
        return !curZones.isEmpty();
    }

    /**
     * @return true if and only if initiating a remove/delete action would
     * change the state of the system (i.e. there are objects in selected
     * list)
     */
    public boolean canDelete() {
        return !curSelections.isEmpty();
    }

    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}

    /** Determines which direction handle the mouse is over.  Returns null if
     * it's not over a handle */
    public Direction getDirection(PlayBookObject curSelected, Point curPoint) {
        Direction d = null;
        int fieldX = (int) curPoint.getX();
        int fieldY = (int) curPoint.getY();
        // Upper left corner
        if (getCheckerboardDist(curPoint, curSelected.getUpperLeft()) <
            CHECKERBOARD_DIST_FOR_RESIZE) {
            d = Direction.NW;
        }
        // Upper right corner
        else if (getCheckerboardDist(curPoint, curSelected.getUpperRight()) <
            CHECKERBOARD_DIST_FOR_RESIZE) {
            d = Direction.NE;
        }
        // Lower left corner
        else if (getCheckerboardDist(curPoint, curSelected.getLowerLeft()) <
            CHECKERBOARD_DIST_FOR_RESIZE) {
            d = Direction.SW;
        }
        // Lower right corner
        else if (getCheckerboardDist(curPoint, curSelected.getLowerRight()) <
            CHECKERBOARD_DIST_FOR_RESIZE) {
            d = Direction.SE;
        }
        // Right side
        else if (Math.abs(fieldX - curSelected.getRightX()) <
                 CHECKERBOARD_DIST_FOR_RESIZE &&
                 isBetween(fieldY, curSelected.getTopY(),
                           curSelected.getBottomY())) {
            d = Direction.E;
        }
        // left side
        else if (Math.abs(fieldX - curSelected.getLeftX()) <
                 CHECKERBOARD_DIST_FOR_RESIZE &&
                 isBetween(fieldY, curSelected.getTopY(),
                           curSelected.getBottomY())) {
            d = Direction.W;
        }
        // top side
        else if (Math.abs(fieldY - curSelected.getTopY()) <
                 CHECKERBOARD_DIST_FOR_RESIZE &&
                 isBetween(fieldX, curSelected.getLeftX(),
                           curSelected.getRightX())){
            d = Direction.N;
        }
        // bottom side
        else if (Math.abs(fieldY - curSelected.getBottomY()) <
                 CHECKERBOARD_DIST_FOR_RESIZE &&
                 isBetween(fieldX, curSelected.getLeftX(),
                           curSelected.getRightX())) {
            d = Direction.S;
        }

        return d;


    }

    public boolean isBetween(int a, int b, int c) {
        return (a >= b && a <= c) || (a >= c && a <= b);
    }

    public double getSlope(int x1, int y1, int x2, int y2) {
        if (x2 == x1) { return 0; }
        return (y2 - y1) * 1.0 / (x2 - x1);
    }

    /**
     * Snaps the line being drawn to be either horizontal, vertical, or at a
     * +-45 degree angle to the x axis
     */
    public void constrainProportions() {
        // If we don't already have a perfectly vertical line...
            if (x1 != x2) {
                double slope = getSlope(x1,y1,x2,y2);

                if (Math.abs(slope) < NEAR_HORIZONTAL_SLOPE) {
                    y2 = y1;
                }
                else if (1.0 / Math.abs(slope) < NEAR_HORIZONTAL_SLOPE) {
                    x2 = x1;
                }
                else {
                    if (Math.abs(x2 - x1) > Math.abs(y2 - y1)) {
                        y2 = ((y2 > y1) ?
                              y1 + Math.abs(x2 - x1) : // if y2 is below y1, y2 = ...
                              y1 - Math.abs(x2 - x1)); // else y2 is above y1..
                    }
                    // Other way around, make x bigger to compensate
                    else {
                        x2 = ((x2 > x1) ?
                              x1 + Math.abs(y2-y1) : // x2 is to the right of x1..
                              x1 - Math.abs(y2-y1)); // x2 is to the left
                    }
                }
            }
    }

    /**
     * Determines if we are mousing over a previously defined zone; if so
     * it calculates which handle we're near and changes the cursor to match.
     * If we are outside of the current selection, cursor is set to default;
     * otherwise we change to a hand to indicate we can drag.
     */
    public void mouseMoved(MouseEvent e) {
	if (testMode) {

	    return;
	}

        if (curSelections.isEmpty()) { return; }

        PlayBookObject curSelected = curSelections.get(0);

        int fieldX = field.fixXCoord(e.getX() / xImageScale);
        int fieldY = field.fixYCoord(e.getY() / yImageScale);
        Point curPoint = new Point(fieldX, fieldY);
        dir = getDirection(curSelected, curPoint);

        if (dir != null) {
            // We now know which zone we're resizing
            curResizing = curSelected;
            tool.setCursor(dir.getCursor());
            oldResizePoint = curResizing.getCurLoc(dir.getDirection());
            return;
        }

        // We weren't near any resizing handles; if we've left confines of
        // the object, fix the cursor
        if (!curSelected.contains((int) (e.getX()/xImageScale),
                                  (int) (e.getY()/yImageScale))) {
            tool.setCursor(DEFAULT_CURSOR);
        }
        else {
            tool.setCursor(HAND_CURSOR);
        }
        curResizing = null;
    }

    public void mouseDragged(MouseEvent e) {
	if (testMode) {
	    int x = (int)(e.getX()/xImageScale),
		y = (int)(e.getY()/yImageScale);
	    x = field.screenToFieldX(x);
	    y = field.screenToFieldY(y);
	    editor.getTestPanel().changeBallLoc(new Point(x, y));
	    return;
	}

        x2 = clipX(e.getX());
        y2 = clipY(e.getY());

        if (e.isShiftDown()) {
            constrainProportions();
        }

        // Resizing the zone, not drawing a new one or moving the zone
        if (curResizing != null) {
            Shape old = curResizing.getShape();

            curResizing.resize(new Point(field.fixXCoord(x2/xImageScale),
                                         field.fixYCoord(y2/yImageScale)),
                               dir.getDirection());
            // Take the union of the bounding boxes of the previous shape
            // and the new shape, then repaint that area
            Shape union = old.getBounds().union(curResizing.getShape().getBounds());
            editor.setLabelText("Current Selection: " + curResizing);
            repaint(union);
        }

        // Dragging a zone or group of zones around
        else if (movingGroup) {
            editor.setLabelText("Current Selection: " + curSelections);

            // Due to the reversal of the axes, we take the negative of the
            // y difference.
            moveGroup(curSelections, (int)((x2 - x1) / xImageScale),
                      - (int)((y2 - y1) / yImageScale));
            repaint();
            // Update coords so that we calculate the differences correctly
            x1 = x2; y1 = y2;
        }

        // Defining the shape
        else {
            // Calculate the current field coordinates to display
            int x1_ = field.fixXCoord(x1 / xImageScale);
            int x2_ = field.fixXCoord(x2 / xImageScale);
            int y1_ = field.fixYCoord(y1 / yImageScale);
            int y2_ = field.fixYCoord(y2 / yImageScale);
            int leftX = Math.min(x1_, x2_);
            int rightX = Math.max(x1_, x2_);
            int topY = Math.max(y1_, y2_);
            int bottomY = Math.min(y1_, y2_);

            editor.setLabelText("Creating new selection: [ID:" +
                                curZones.size() +
                                " (" + leftX + ", " + topY + ")" +
                                " (" + rightX + ", " + bottomY + ")]");
            repaint();
        }
    }

    /**
     * Clips a screen x coordinate to lie firmly between the left and right
     * sides of the Field graphic
     */
    public int clipX(double x) {
        if (x < 0) { return 0; }
        else if (x > field.FIELD_WIDTH * xImageScale) {
            return (int) (field.FIELD_WIDTH * xImageScale);
        }
        else {
            return (int) x;
        }
    }

    /**
     * Clips a screen y coordinate to lie firmly between the left and right
     * sides of the Field graphic.
     */
    public int clipY(double y) {
        if (y < 0) { return 0; }
        else if (y > field.FIELD_HEIGHT * yImageScale) {
            return (int) (field.FIELD_HEIGHT * yImageScale);
        }
        else {
            return (int) y;
        }
    }

    /**
     * Calculates the checkerboard distance between two points, or simply the
     * difference in x + the difference in y.  Used to ascertain whether or not
     * the mouse is close enough to the corners of a zone to enable resize mode.
     * @param x1 x coord of point one
     * @param y1 y coord of point one
     * @param x2 x coord of point two
     * @param y2 y coord of point two
     */
    public int getCheckerboardDist(int x1, int y1, int x2, int y2) {
        return Math.abs(x1 - x2) + Math.abs(y1 - y2);
    }

    /**
     * Calculates the checkerboard distance between two points, or simply the
     * difference in x + the difference in y.  Used to ascertain whether or not
     * the mouse is close enough to the corners of a zone to enable resize mode.
     * This is a convenience method that takes two Points and then breaks them
     * into constituent (x,y) coords to pass into getCheckerboardDist(x1,y1..)
     * @param one the first Point
     * @param two the second Point
     */
    public int getCheckerboardDist(Point one, Point two) {
        return getCheckerboardDist((int)one.getX(), (int)one.getY(),
                                   (int)two.getX(), (int)two.getY());
    }



    /**
     * Replaces the current field with the field passed in and changes the
     * buffered image being displayed to reflect the field's graphics.
     * @param field the Field with which to replace current field.
     */
    public void setField(Field field) {
        this.field = field;
        setImage(field.getFieldImage());
        repaint();
    }

    /** @return the Field associated with this viewer */
    public Field getField() { return field; }

    /**
     * Handles the use of the tab key to cycle through zones; without this
     * the focus system automatically consumes tab keys and prevents us from
     * getting them in the keyReleased method.
     * @return true if we are handling the event ourself, false if the
     * focus system should handle it
     */
    public boolean dispatchKeyEvent(KeyEvent e) {
        if (isShowing() && e.getKeyCode() == KeyEvent.VK_TAB &&
            e.getID() == KeyEvent.KEY_RELEASED) {
            keyReleased(e);
            return true;
        }
        else {
            return false;
        }
    }
    /**
     * Handles keyboard events.  Arrow keys move the currently selected zone
     * around, tab or shift+tab cycles through the zones.
     */
    public void keyReleased(KeyEvent e) {
        switch (e.getKeyCode()) {
        case KeyEvent.VK_TAB:
            cycleThroughZones(e.isShiftDown());
            break;
        }
        repaint();
    }

    public void keyPressed(KeyEvent e) {}
    public void keyTyped(KeyEvent e) { }


    public void moveUp() {
        moveGroup(curSelections, 0, ARROW_MOVE_AMT);
        repaint();
    }

    public void moveLeft() {
        moveGroup(curSelections, -ARROW_MOVE_AMT, 0);
        repaint();
    }

    public void moveRight() {
        moveGroup(curSelections, ARROW_MOVE_AMT, 0);
        repaint();
    }

    public void moveDown() {
        moveGroup(curSelections, 0, -ARROW_MOVE_AMT);
        repaint();
    }


    /**
     * Changes the focus to be on the next Zone in the array list.
     *
     * @param backwards if true, then we go to the previous zone, not the next
     */
    public void cycleThroughZones(boolean backwards) {
        if (curZones.isEmpty()) { return; }
        int change = 1;
        if (backwards) { change *= -1; }
        index = (index + change + curZones.size()) % curZones.size();

        // Repaint all the previous selections to be their new "unselected"
        // shape
        for (int i = curSelections.size() - 1; i > -1; i--) {
            Shape s = curSelections.remove(i).getShape();
            repaint(s);
        }

        curSelections.add(curZones.get(index));
        editor.setLabelText("Current Selections: " + curSelections);
        repaint(curZones.get(index).getShape());
    }


    /**
     * Calculates the bounding box around all the elements in toMove; ensures
     * that all elements in the bounding box stay within the bounds of the
     * field.
     */
    void moveGroup(LinkedList<PlayBookObject> toMove, int dx, int dy) {
        int leftX = (int) field.FIELD_WIDTH;
        int rightX = 0;
        int topY = 0;
        int bottomY = (int) field.FIELD_HEIGHT;
        for (PlayBookObject z : toMove) {
            if (z.getLeftX() < leftX) {
                leftX = z.getLeftX();
            }
            if (z.getRightX() > rightX) {
                rightX = z.getRightX();
            }
            if (z.getTopY() > topY) {
                topY = z.getTopY();
            }
            if (z.getBottomY() < bottomY) {
                bottomY = z.getBottomY();
            }
        }

        // Our bounding box around all the rectangular shapes is now defined
        // by the four variables previously declared.

        if (rightX + dx > field.FIELD_WIDTH) {
            dx = (int) field.FIELD_WIDTH - rightX;
        }
        else if (leftX + dx < 0) {
            dx = -leftX;
        }

        if (topY + dx > field.FIELD_HEIGHT) {
            dy = (int) field.FIELD_HEIGHT - topY;
        }
        else if (bottomY + dx < 0) {
            dy = -bottomY;
        }

        for (PlayBookObject z : toMove) {
            z.move(dx, dy);
        }
    }

    public void setMode(DrawingMode mode) {
        this.mode = mode;
        repaint();
    }

    public DrawingMode getMode() { return mode; }



    public boolean contains(LinkedList<PlayBookObject> zones, int x, int y) {
        for (PlayBookObject z : zones) {
            if (z.contains(x, y)) { return true; }
        }
        return false;
    }

    /**
     * Creates a deep copy of the current selections and stores it in the
     * clipBoard.
     */
    public void copy() {
        clipBoard = deepCopy(curSelections);
        editor.refreshButtons();
    }


    /**
     * Moves each element in the list by a small amount and then copies each
     * modified element into a new list, which is then returned.  Note that
     * this method has the side effect (intentional in our case) of modifying
     * the toCopy list
     */
    protected LinkedList<PlayBookObject>
        deepCopyAndMove(LinkedList<PlayBookObject> toCopy) {

        LinkedList <PlayBookObject> copy = new LinkedList<PlayBookObject>();
        for (PlayBookObject z : toCopy) {
            z.move(PASTE_X_OFFSET, PASTE_Y_OFFSET);
            copy.add(z.clone());
        }
        return copy;
    }


    /**
     * Performs a deep copy of a linked list, which is to say that each element
     * of the list is cloned, added to a new linked list, then the linked
     * list is returned.
     */
    protected LinkedList<PlayBookObject> deepCopy(LinkedList<PlayBookObject>
                                                  toCopy) {
        LinkedList <PlayBookObject> copy = new LinkedList<PlayBookObject>();
        for (PlayBookObject z : toCopy) {
            copy.add(z.clone());
        }
        return copy;

    }


    /**
     * Undo the last action that had been done.  Will do nothing if no moves
     * can be undone.
     */
    public void undo() {
        // Only allow redo if we're not dragging
        if (mousePressed) { return; }

        if (undoManager.canUndo()) {
            undoManager.undo();

            if (curSelections.isEmpty()) {
                curResizing = null;
                tool.setCursor(DEFAULT_CURSOR);
            }
        }
        repaint();
        editor.refreshButtons();
    }

    /**
     * Repeat the last action that had been undone.  Will do nothing if there
     * are no redoable actions.
     */
    public void redo() {
        // Only allow redo if we're not dragging
        if (mousePressed) { return; }

        if (undoManager.canRedo()) {
            undoManager.redo();
            if (curSelections.isEmpty()) {
                curResizing = null;
                tool.setCursor(DEFAULT_CURSOR);
            }
        }
        repaint();
        editor.refreshButtons();
    }



    /*
     * Undoable actions
     */
    public void paste() {
        if (clipBoard.isEmpty()) { return; }

        int startIndex = curZones.size() - 1;
        UndoableEdit edit = new PasteEdit(curZones, deepCopyAndMove(clipBoard),
                                          curSelections,
                                          startIndex);
        undoSupport.postEdit(edit);
    }

    public void clear(ArrayList<PlayBookObject> zones,
                      LinkedList<PlayBookObject> curSelections) {
        //System.out.println("Clearing");

        UndoableEdit edit = new ClearEdit(zones, curSelections);
        undoSupport.postEdit(edit);
    }

    /**
     * Creates an AddEdit object which encapsulates the action of adding z to
     * the curZones and curSelections at the given index (we add to the end
     * of our array list).
     * Note that it is the AddEdit object that literally adds the object to
     * the list and the arraylist.
     */
    public void add(PlayBookObject z) {
        int numElements = curZones.size();

        //System.out.println("adding " + z + " curSelections: " + curSelections);

        UndoableEdit edit = new AddEdit(curZones, curSelections, z,
                                        numElements);
        undoSupport.postEdit(edit);
        repaint();
    }

    /**
     * Convenience method which calls clear on the currently selected objects
     * and master list (takes care of undo/redo)
     */
    public void clear() {
        clear(curZones, curSelections);
        repaint();
        curResizing = null;
        movingGroup = false;
        tool.setCursor(Cursor.getDefaultCursor());
    }

    /**
     * Convenience method which calls remove on the currently selected
     * objects and the master list.
     */
    public void delete() {
        if (curSelections.isEmpty()) { return; }
        remove(curSelections, curZones);
        repaint();
        curResizing = null;
        tool.setCursor(Cursor.getDefaultCursor());
        editor.setLabelText("Current Selections: " + curSelections);
    }

    /**
     * Creates a RemoveEdit object which encapsulates the action of removing
     * all objects in the toDelete list from the masterList and from the
     * curSelections.  Again, note that the actual deletion takes place
     * in the constructor of the RemoveEdit object.
     */
    protected void remove(LinkedList<PlayBookObject> toDelete,
                          ArrayList<PlayBookObject> masterList) {

        // Find the indices of the elements in the toDelete list in the
        // masterList; in this way we can ensure that when we undo and redo
        // this action, we keep the order of the elements correctly.
        int indices[] = new int[toDelete.size()];
        int i = 0;
        for (PlayBookObject z : toDelete) {
            int index = masterList.indexOf(z);
            if (index == -1) {
                TOOL.CONSOLE.error("Error, tried to delete an object that " +
                                   "was not in the master list");
                return;
            }
            else {
                indices[i++] = index;
            }
        }
        // We sort the indices because we need to add and delete them in the
        // sorted order when we undo/redo this move.
        Arrays.sort(indices);
        curSelections.clear();
        for (int j = 0; j < indices.length; j++) {
            curSelections.add(curZones.get(indices[j]));
        }

        UndoableEdit edit = new RemoveEdit(curZones, indices, curSelections);
        undoSupport.postEdit(edit);
    }


    /**
     * Called after a zone or group of zones has been moved by the user;
     * encapsulates the amount that the zone was just moved by so that we
     * can undo and redo the action.
     */
    protected void move(LinkedList<PlayBookObject> z, int dx, int dy) {
        UndoableEdit edit = new MoveEdit(z, dx, dy);
        undoSupport.postEdit(edit);
    }

    protected void resize(PlayBookObject z, int dir, Point oldPoint, Point newPoint) {
        UndoableEdit edit = new ResizeEdit(z, dir, oldPoint, newPoint);
        undoSupport.postEdit(edit);
    }

    public UndoManager getUndoManager() {
        return undoManager;
    }

    /**
     * An undo/redo adapter.  This is notified whenever an undoable action
     * occurs.
     * @see http://www.javaworld.com/javaworld/jw-06-1998/undoredo/UndoPanel.java
     */
    private class UndoAdapter implements UndoableEditListener {
        public void undoableEditHappened (UndoableEditEvent evt) {
            UndoableEdit edit = evt.getEdit();
            undoManager.addEdit( edit );
            editor.refreshButtons();
        }
    }

    public LinkedList<PlayBookObject> getSelections()
    {
	return curSelections;
    }

    public void setBallVisible(boolean isVisible)
    {
	ballVisible = isVisible;
	repaint();
    }

    public void setTestMode(boolean mode)
    {
	testMode = mode;
    }
}
