package teamcomm.gui.drawings.common;

import com.jogamp.opengl.GL2;
import data.PlayerInfo;
import data.SPLStandardMessage;
import teamcomm.data.RobotState;
import teamcomm.gui.Camera;
import teamcomm.gui.drawings.PerPlayer;
import teamcomm.gui.drawings.Text;

/**
 * Drawing for the player number of a robot.
 *
 * @author Felix Thielke
 */
public class PlayerNumber extends PerPlayer {

    @Override
    public void draw(final GL2 gl, final RobotState player, final Camera camera) {
        final SPLStandardMessage msg = player.getLastMessage();
        if (msg != null && msg.playerNumValid && msg.poseValid) {
            gl.glPushMatrix();

            if (player.getPenalty() != PlayerInfo.PENALTY_NONE) {
                gl.glTranslatef(-msg.playerNum, -3.5f, 0.7f);
            } else {
                gl.glTranslatef(msg.pose[0] / 1000.f, msg.pose[1] / 1000.f, 0.7f);
            }

            camera.turnTowardsCamera(gl);
            Text.drawText(gl, "" + msg.playerNum, 0, 0, 0.25f);

            gl.glPopMatrix();
        }
    }

    @Override
    public boolean hasAlpha() {
        return true;
    }

    @Override
    public int getPriority() {
        return 10;
    }
}
