package teamcomm.gui;

import com.jogamp.opengl.GL2;
import com.jogamp.opengl.glu.GLU;

/**
 * Class for modeling the viewer position in a 3D environment.
 *
 * @author Felix Thielke
 */
public class Camera {

    private static final float NEAR_PLANE = 1;
    private static final float FAR_PLANE = 20;

    private float theta = 45;
    private float phi = 0;
    private float radius = 9;
    private boolean flipped = false;

    /**
     * Rotate the camera by 180 degrees around the z axis.
     *
     * @param gl OpenGL context
     */
    public void flip(final GL2 gl) {
        flipped = !flipped;
        gl.glRotatef(180, 0, 0, 1);
    }

    /**
     * Adds the given angle to the theta angle (for rotation around the x axis).
     * theta is clipped to the range [0,90].
     *
     * @param amount angle
     */
    public void addTheta(final float amount) {
        theta += amount;
        if (theta < 0) {
            theta = 0;
        } else if (theta > 90) {
            theta = 90;
        }
    }

    /**
     * Adds the given angle to the phi angle (for rotation around the z axis).
     * phi is clipped to the range [-90,90].
     *
     * @param amount angle
     */
    public void addPhi(final float amount) {
        phi += amount;
        if (phi < -90) {
            phi = -90;
        } else if (phi > 90) {
            phi = 90;
        }
    }

    /**
     * Adds the given distance to the camera radius. The radius is clipped so
     * that the field is always within the viewing frustum.
     *
     * @param amount distance
     */
    public void addRadius(final float amount) {
        radius += amount;
        if (radius < NEAR_PLANE) {
            radius = NEAR_PLANE;
        } else if (radius > FAR_PLANE - 5) {
            radius = FAR_PLANE - 5;
        }
    }

    /**
     * Sets up the viewing frustum of the camera on the given GL context. To be
     * called once whenever the ratio of width and height changes.
     *
     * @param gl OpenGL context
     * @param displayRatio ratio of width to height
     */
    public void setupFrustum(final GL2 gl, final double displayRatio) {
        final GLU glu = GLU.createGLU(gl);
        gl.glMatrixMode(GL2.GL_PROJECTION);
        gl.glLoadIdentity();
        glu.gluPerspective(40, displayRatio, NEAR_PLANE, FAR_PLANE);
        gl.glMatrixMode(GL2.GL_MODELVIEW);
    }

    /**
     * Performs transformations on the given OpenGL context which correspond to
     * the position of the camera.
     *
     * @param gl OpenGL context
     */
    public void positionCamera(final GL2 gl) {
        gl.glTranslatef(0, 0, -radius);
        gl.glRotatef(-theta, 1, 0, 0);
        gl.glRotatef(phi, 0, 0, 1);
    }

    /**
     * Performs transformations on the given OpenGl context so that the Z axis
     * points towards the position of the camera. Useful for rendering text.
     *
     * @param gl OpenGL context
     */
    public void turnTowardsCamera(final GL2 gl) {
        gl.glRotatef((flipped ? 180 : 0) - phi, 0, 0, 1);
        gl.glRotatef(theta, 1, 0, 0);
    }
}
