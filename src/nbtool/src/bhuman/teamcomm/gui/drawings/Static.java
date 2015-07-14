package teamcomm.gui.drawings;

import com.jogamp.opengl.GL2;
import teamcomm.gui.Camera;

/**
 * Abstract base class for drawings that are drawn once.
 *
 * @author Felix Thielke
 */
public abstract class Static extends Drawing {

    /**
     * Draws this drawing.
     *
     * @param gl OpenGL context
     * @param camera the camera of the scene
     */
    public abstract void draw(final GL2 gl, final Camera camera);
}
