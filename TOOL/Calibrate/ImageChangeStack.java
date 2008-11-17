package edu.bowdoin.robocup.TOOL.Calibrate;

import edu.bowdoin.robocup.TOOL.Image.ImageOverlayAction;
import java.util.Stack;
import java.util.Vector;

import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Calibrate.Pair;

/**
 * A stack which holds the updates to both the color table and the image 
 * overlay.  This allows any action to be undone or redone as necessary.
 * Because two different things are kept track of, the color table and
 * the overlay, we return Pairs of these objects whenever a peek or pop
 * is necessary.  For convenience, we allow adding either a Pair of the
 * objects or each object as individual parameters.
 * @author Nicholas Dunn
 * @version 1.0 11/26/2007
 */

public class ImageChangeStack {
    // Holds all the ColorTable updates
    private Stack <Vector <ColorTableUpdate> > colorTableChanges;
    private Stack <ImageOverlayAction> overlayChanges;

    private int size;

    /** 
     *Initializes empty stacks, sets size to 0.
     */
    public ImageChangeStack() {
	colorTableChanges = new Stack <Vector <ColorTableUpdate> >();
	overlayChanges = new Stack<ImageOverlayAction>();
	size = 0;
    }

    /** @return size number of elements in the stacks (each stack must
     * always be the same size) */
    public int getSize() {
	return size;
    }

    /** @return true if and only if both stacks are empty */
    public boolean empty() {
	return size == 0;
    }

    /** Empties out all elements, sets size to 0 */
    public void clear() {
	colorTableChanges.clear();
	overlayChanges.clear();
	size = 0;
    }

    /**
     * Pushes multiple changes to the table at once.
     * @param toAdd the Vector of ColorTableUpdates to add
     * @precondition none of the elements in toAdd are null
     */
    public void push(Vector <ColorTableUpdate> toAdd, 
		     ImageOverlayAction overlayChange) {
	if (toAdd == null)
            TOOL.CONSOLE.error("Cannot add a null ColorTableUpdate to Stack!");

	colorTableChanges.push(toAdd);
	overlayChanges.add(overlayChange);
	size++;
    }
	
    /**
     * Push a single update to the stack.
     * @param toAdd the single ColorTableUpdate to add
     */
    public void push(ColorTableUpdate toAdd, 
		     ImageOverlayAction overlayChange) {
	if (toAdd == null)
            TOOL.CONSOLE.error("Cannot add a null ColorTableUpdate to Stack!");

	Vector <ColorTableUpdate> addition = new Vector<ColorTableUpdate>();
	addition.add(toAdd);
	colorTableChanges.push(addition);
	overlayChanges.add(overlayChange);
	size++;
    }

    /**
     * Convenience method that pushes a Pair onto this stack; since
     * a pop returns such a Pair, it is easy to push the result of
     * a pop off of one type of stack onto the opposite (from redo to undo,
     * for instance)
     */
    public void push (Pair<Vector <ColorTableUpdate>,
		      ImageOverlayAction> toAdd) {
	
	Vector <ColorTableUpdate> addition = toAdd.getFirst();
	ImageOverlayAction overlayChange = toAdd.getSecond();
	colorTableChanges.push(addition);
	overlayChanges.push(overlayChange);
	size++;
	
    }


    /**
     * @return a Pair of the elements at the top of both stacks.  Does not
     * remove either element (size is unaffected)
     * @precondition size > 0
     */
    public Pair<Vector <ColorTableUpdate>, ImageOverlayAction> peek() {
	Pair <Vector <ColorTableUpdate>, ImageOverlayAction> result =
	    new Pair <Vector <ColorTableUpdate>, ImageOverlayAction>
	    (colorTableChanges.peek(), overlayChanges.peek());
	return result;
    }

    /**
     * @return a Pair of the top two elements, removing them from the stacks
     * (size is one less after this call)
     * @precondition size > 0
     */
    public Pair<Vector <ColorTableUpdate>, ImageOverlayAction> pop() {
	size--;
	Pair <Vector <ColorTableUpdate>, ImageOverlayAction> result =
	    new Pair <Vector <ColorTableUpdate>, ImageOverlayAction>
	    (colorTableChanges.pop(), overlayChanges.pop());
	return result;
    }
}
