package teamcomm.gui.drawings;

import com.jogamp.opengl.GL2;

/**
 * Abstract base class for drawings of the 3D field view.
 *
 * @author Felix Thielke
 */
public abstract class Drawing {

    private boolean active = true;

    private int teamNumber = -1;

    private boolean initialized = false;

    /**
     * Sets whether this drawing is drawn.
     *
     * @param active boolean
     */
    public void setActive(final boolean active) {
        this.active = active;
    }

    /**
     * Returns whether this drawing is drawn.
     *
     * @return boolean
     */
    public boolean isActive() {
        return active;
    }

    /**
     * Returns the number of the team for which thhis drawing is drawn.
     *
     * @return team number
     */
    public int getTeamNumber() {
        return teamNumber;
    }

    /**
     * Sets the number of the team for which thhis drawing is drawn.
     *
     * @param teamNumber team number
     */
    public void setTeamNumber(final int teamNumber) {
        this.teamNumber = teamNumber;
    }

    /**
     * Handles the initialization of the drawing.
     *
     * @param gl the OpenGL context this drawing will be drawn on
     */
    public final void initialize(final GL2 gl) {
        if (!initialized) {
            init(gl);
            initialized = true;
        }
    }

    /**
     * Performs some initialization, like creating display lists for objects to
     * be drawn etc. This method is meant to be overridden by subclasses that
     * need initialization. It is guaranteed to be called once before the first
     * call of the draw()-method.
     *
     * @param gl the OpenGL context this drawing will be drawn on
     */
    protected void init(final GL2 gl) {
    }

    /**
     * Indicates whether this drawing contains transparent objects.
     *
     * @return boolean
     */
    public abstract boolean hasAlpha();

    /**
     * The higher the priority, the earlier the drawing is drawn. This only
     * matters for transparent drawings so they overlap correctly. Drawings in
     * the foreground should have lower priority than drawings in the
     * background.
     *
     * @return priority
     */
    public abstract int getPriority();

}
