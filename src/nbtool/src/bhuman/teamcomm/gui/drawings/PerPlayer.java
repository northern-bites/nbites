package teamcomm.gui.drawings;

import com.jogamp.opengl.GL2;
import teamcomm.data.RobotState;
import teamcomm.gui.Camera;

/**
 * Abstract base class for drawings that are drawn for each robot individually.
 *
 * @author Felix Thielke
 */
public abstract class PerPlayer extends Drawing {

    /**
     * Draws this drawing.
     *
     * @param gl OpenGL context
     * @param player robot state of the robot for which this drawing is drawn
     * @param camera the camera of the scene
     */
    public abstract void draw(final GL2 gl, final RobotState player, final Camera camera);
}
