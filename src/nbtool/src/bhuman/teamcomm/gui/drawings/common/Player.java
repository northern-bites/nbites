package teamcomm.gui.drawings.common;

import com.jogamp.opengl.GL2;
import data.GameControlData;
import data.PlayerInfo;
import data.SPLStandardMessage;
import teamcomm.data.GameState;
import teamcomm.data.RobotState;
import teamcomm.gui.Camera;
import teamcomm.gui.drawings.RoSi2Loader;
import teamcomm.gui.drawings.PerPlayer;

/**
 * Drawing for a robot.
 *
 * @author Felix Thielke
 */
public class Player extends PerPlayer {

    @Override
    protected void init(GL2 gl) {
        RoSi2Loader.getInstance().cacheModels(gl, new String[]{"robotBlue", "robotRed", "robotBlack", "robotYellow"});
    }

    @Override
    public void draw(final GL2 gl, final RobotState player, final Camera camera) {
        final SPLStandardMessage msg = player.getLastMessage();
        if (msg != null && msg.poseValid) {
            gl.glPushMatrix();

            if (player.getPenalty() != PlayerInfo.PENALTY_NONE) {
                gl.glTranslatef(-msg.playerNum, -3.5f, 0);
                gl.glRotatef(-90, 0, 0, 1);
            } else {
                gl.glTranslatef(msg.pose[0] / 1000.f, msg.pose[1] / 1000.f, 0);
                gl.glRotatef((float) Math.toDegrees(msg.pose[2]), 0, 0, 1);

                if (msg.fallenValid && msg.fallen) {
                    gl.glTranslatef(0, 0, 0.05f);
                    gl.glRotatef(90, 0, 1, 0);
                }
            }

            switch (GameState.getInstance().getTeamColor(player.getTeamNumber())) {
                case GameControlData.TEAM_BLUE:
                    gl.glCallList(RoSi2Loader.getInstance().loadModel(gl, "robotBlue"));
                    break;
                case GameControlData.TEAM_RED:
                    gl.glCallList(RoSi2Loader.getInstance().loadModel(gl, "robotRed"));
                    break;
                case GameControlData.TEAM_BLACK:
                    gl.glCallList(RoSi2Loader.getInstance().loadModel(gl, "robotBlack"));
                    break;
                case GameControlData.TEAM_YELLOW:
                    gl.glCallList(RoSi2Loader.getInstance().loadModel(gl, "robotYellow"));
                    break;
            }

            gl.glPopMatrix();
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
