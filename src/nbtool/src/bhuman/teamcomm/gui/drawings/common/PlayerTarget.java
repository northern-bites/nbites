package teamcomm.gui.drawings.common;

import com.jogamp.opengl.GL2;
import data.PlayerInfo;
import data.SPLStandardMessage;
import teamcomm.data.RobotState;
import teamcomm.gui.Camera;
import teamcomm.gui.drawings.PerPlayer;

/**
 * Drawing for the walking and shooting targets of a player.
 *
 * @author Felix Thielke
 */
public class PlayerTarget extends PerPlayer {

    private static final float CROSS_RADIUS = 0.1f;

    @Override
    public void draw(final GL2 gl, final RobotState player, final Camera camera) {
        final SPLStandardMessage msg = player.getLastMessage();
        if (msg != null && msg.poseValid && msg.walkingToValid && player.getPenalty() == PlayerInfo.PENALTY_NONE) {
            final float poseX = msg.pose[0] / 1000.f;
            final float poseY = msg.pose[1] / 1000.f;
            final float walkingToX = msg.walkingTo[0] / 1000.f;
            final float walkingToY = msg.walkingTo[1] / 1000.f;

            // Set color and normal
            gl.glColor3f(0, 0, 0);
            gl.glNormal3f(0, 0, 1);

            // Draw line from player to walking target
            gl.glBegin(GL2.GL_LINES);
            gl.glVertex2f(poseX, poseY);
            gl.glVertex2f(walkingToX, walkingToY);
            gl.glEnd();

            // Draw cross on walking target
            gl.glTranslatef(walkingToX, walkingToY, 0);
            gl.glBegin(GL2.GL_LINES);
            gl.glVertex2f(-CROSS_RADIUS, -CROSS_RADIUS);
            gl.glVertex2f(CROSS_RADIUS, CROSS_RADIUS);
            gl.glVertex2f(-CROSS_RADIUS, CROSS_RADIUS);
            gl.glVertex2f(CROSS_RADIUS, -CROSS_RADIUS);
            gl.glEnd();
            gl.glTranslatef(-walkingToX, -walkingToY, 0);

            // Set color
            gl.glColor3f(1, 0, 0);

            // Draw line from player to shooting target
            if (msg.shootingToValid) {
                final float shootingToX = msg.shootingTo[0] / 1000.f;
                final float shootingToY = msg.shootingTo[1] / 1000.f;

                gl.glBegin(GL2.GL_LINES);
                gl.glVertex2f(poseX, poseY);
                gl.glVertex2f(shootingToX, shootingToY);
                gl.glEnd();

                // Draw cross on shooting target
                gl.glTranslatef(shootingToX, shootingToY, 0);
                gl.glBegin(GL2.GL_LINES);
                gl.glVertex2f(-CROSS_RADIUS, -CROSS_RADIUS);
                gl.glVertex2f(CROSS_RADIUS, CROSS_RADIUS);
                gl.glVertex2f(-CROSS_RADIUS, CROSS_RADIUS);
                gl.glVertex2f(CROSS_RADIUS, -CROSS_RADIUS);
                gl.glEnd();
                gl.glTranslatef(-shootingToX, -shootingToY, 0);
            }
        }
    }

    @Override
    public boolean hasAlpha() {
        return false;
    }

    @Override
    public int getPriority() {
        return 0;
    }
}
