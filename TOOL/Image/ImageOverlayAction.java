package TOOL.Image;

import java.util.Vector;
import java.util.Iterator;
import TOOL.Calibrate.Pair;

/**
 * A class that represents a single action by the user which changes
 * the underlying ImageOverlay.  For instance, a single click might
 * change 900 pixels on the ImageOverlay and it is more efficient to collect
 * all the changes into an object and iterate over them within the ImageOverlay
 * class rather than risking all the overhead of making hundreds of calls to 
 * methods within the ImageOverlay.
 * @author Nicholas Dunn
 * @version 1.0 11/26/2007
 */

public class ImageOverlayAction {
    // Holds (x,y) coordinates of pixel being changed
    private Vector <Pair <Integer, Integer>> locations;
    // Holds (old, new) values of the color at that coordinate
    private Vector <Pair <Byte, Byte>> colors;
    private int size;
    // Holds the hash of the String filename change took place in
    private int imageID;

    /** Constructor that initializes empty vectors, sets size = 0 */
    public ImageOverlayAction(int imageID) {
	locations = new Vector<Pair <Integer, Integer>>();
	colors = new Vector <Pair <Byte, Byte>> ();
	size = 0;
	this.imageID = imageID;

    }


    /** 
     * Adds a single pixel change to the Vector.  Size increments by 1.
     */ 
    public void add(int x, int y, byte oldColor, byte newColor) {
	Pair <Integer, Integer> coord = new Pair<Integer, Integer>(x, y);
	Pair <Byte, Byte> colors = new Pair<Byte, Byte>(oldColor, newColor);
	locations.add(coord);
	this.colors.add(colors);
	size++;

    }

    /**
     * @return the imageID of the image in which this change takes place,
     * a hash of the string filename
     */
    public int getID() {
	return imageID;
    }

    /** @return number of pixels that are in this action */
    public int getSize() {
	return size;
    }

    /** @return an iterator of the coordinate Pairs */
    public Iterator getCoordinateIterator() {
	return locations.iterator();
    }

    /** @return an iterator of the color pairs */
    public Iterator getColorsIterator() {
	return colors.iterator();
    }


}
