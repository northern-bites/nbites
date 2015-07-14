package teamcomm.gui.drawings.common;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.glu.GLU;
import com.jogamp.opengl.glu.GLUquadric;
import data.PlayerInfo;
import data.SPLStandardMessage;
import teamcomm.data.RobotState;
import teamcomm.gui.Camera;
import teamcomm.gui.drawings.RoSi2Loader;
import teamcomm.gui.drawings.PerPlayer;

/**
 * Drawing for the ball as seen by a robot.
 *
 * @author Felix Thielke
 */
public class Ball extends PerPlayer {

    private static final float ROBOT_HEAD_Z = 0.5f;
    private static final float BALL_RADIUS = 0.0325f;
    private static final float MIN_CYLINDER_RADIUS = 0.01f;
    private static final float MAX_BALLAGE = 5.0f;

    @Override
    protected void init(GL2 gl) {
        RoSi2Loader.getInstance().cacheModels(gl, new String[]{"ball"});
    }

    @Override
    public void draw(final GL2 gl, final RobotState player, final Camera camera) {
        final SPLStandardMessage msg = player.getLastMessage();
        if (msg != null && msg.poseValid && msg.ballValid && msg.ballAge > -1 && msg.ballAge < MAX_BALLAGE && player.getPenalty() == PlayerInfo.PENALTY_NONE) {
            final float[] ball = {msg.ball[0] / 1000.f, msg.ball[1] / 1000.f};

            gl.glPushMatrix();

            // Translate to robot
            gl.glTranslatef(msg.pose[0] / 1000.f, msg.pose[1] / 1000.f, 0);
            gl.glRotatef((float) Math.toDegrees(msg.pose[2]), 0, 0, 1);

            // Translate to ball
            gl.glTranslatef(ball[0], ball[1], 0);

            // Draw ball
            gl.glCallList(RoSi2Loader.getInstance().loadModel(gl, "ball"));

            // Draw ball velocity
            gl.glBegin(GL.GL_LINES);
            gl.glColor3f(1, 0, 0);
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(0, 0, BALL_RADIUS);
            gl.glVertex3f(msg.ballVel[0] / 1000.f, msg.ballVel[1] / 1000.f, BALL_RADIUS);
            gl.glEnd();

            // Translate back to robot
            gl.glTranslatef(-ball[0], -ball[1], 0);

            // Draw cylinder from robot to ball
            gl.glTranslatef(0, 0, ROBOT_HEAD_Z);
            gl.glRotatef((float) Math.toDegrees(Math.atan2(ball[1], ball[0])), 0, 0, 1);
            gl.glRotatef((float) Math.toDegrees(Math.atan2(Math.sqrt(ball[0] * ball[0] + ball[1] * ball[1]), BALL_RADIUS - ROBOT_HEAD_Z)), 0, 1, 0);
            gl.glColorMaterial(GL2.GL_FRONT, GL2.GL_AMBIENT_AND_DIFFUSE);
            gl.glColor4f(1, 0, 0, 0.25f);
            gl.glEnable(GL2.GL_BLEND);
            gl.glBlendFunc(GL2.GL_SRC_ALPHA, GL2.GL_ONE_MINUS_SRC_ALPHA);
            final GLU glu = GLU.createGLU(gl);
            final GLUquadric q = glu.gluNewQuadric();
            final double cylinderLength = Math.sqrt(ball[0] * ball[0] + ball[1] * ball[1] + (BALL_RADIUS - ROBOT_HEAD_Z) * (BALL_RADIUS - ROBOT_HEAD_Z));
            final double relativeAge = (double) msg.ballAge / (double) MAX_BALLAGE;
            final double thinpart = cylinderLength * relativeAge;
            final double thickpart = cylinderLength - thinpart;
            if (thickpart < 0.05) {
                glu.gluCylinder(q, MIN_CYLINDER_RADIUS, MIN_CYLINDER_RADIUS, cylinderLength, 16, 1);
            } else {
                glu.gluCylinder(q, BALL_RADIUS, BALL_RADIUS, thickpart - 0.05, 16, 1);
                gl.glTranslated(0, 0, thickpart - 0.05);
                glu.gluCylinder(q, BALL_RADIUS, MIN_CYLINDER_RADIUS, 0.05, 16, 1);
                glu.gluCylinder(q, MIN_CYLINDER_RADIUS, MIN_CYLINDER_RADIUS, thinpart, 16, 1);
            }
            glu.gluDeleteQuadric(q);
            gl.glDisable(GL2.GL_BLEND);

            gl.glPopMatrix();
        }
    }

    @Override
    public boolean hasAlpha() {
        return true;
    }

    @Override
    public int getPriority() {
        return 500;
    }

}
