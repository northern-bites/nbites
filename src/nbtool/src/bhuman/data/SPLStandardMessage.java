package data;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.LinkedList;
import java.util.List;

public class SPLStandardMessage implements Serializable {

    private static final long serialVersionUID = 2204681477211322628L;

    /**
     * Some constants from the C-structure.
     */
    public static final String SPL_STANDARD_MESSAGE_STRUCT_HEADER = "SPL ";
    public static final byte SPL_STANDARD_MESSAGE_STRUCT_VERSION = 6;
    public static final short SPL_STANDARD_MESSAGE_DATA_SIZE = 780;
    public static final byte SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS = 5;
    public static final int SIZE = 4 // header size
            + 1 // byte for the version
            + 1 // player number
            + 1 // team number
            + 1 // fallen
            + 12 // pose
            + 8 // walking target
            + 8 // shooting target
            + 4 // ball age
            + 8 // ball position
            + 8 // ball velocity
            + SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS // suggestions
            + 1 // intention
            + 2 // average walk speed
            + 2 // maximum kick distance
            + 1 // confidence of current position
            + 1 // confidence of current side
            + 2 // actual size of data
            + SPL_STANDARD_MESSAGE_DATA_SIZE; // data

    public String header;   // header to identify the structure
    public byte version;    // version of the data structure
    public byte playerNum;  // 1-5
    public byte teamNum;    // the number of the team (as provided by the organizers)
    public boolean fallen;  // whether the robot is fallen

    // position and orientation of robot
    // coordinates in millimeters
    // 0,0 is in center of field
    // +ve x-axis points towards the goal we are attempting to score on
    // +ve y-axis is 90 degrees counter clockwise from the +ve x-axis
    // angle in radians, 0 along the +x axis, increasing counter clockwise
    public float[] pose = new float[3];      // x,y,theta

    // the robot's target position on the field
    // the coordinate system is the same as for the pose
    // if the robot does not have any target, this attribute should be set to the robot's position
    public float[] walkingTo = new float[2];

    // the target position of the next shot (either pass or goal shot)
    // the coordinate system is the same as for the pose
    // if the robot does not intend to shoot, this attribute should be set to the robot's position
    public float[] shootingTo = new float[2];

    // Ball information
    public float ballAge;        // seconds since this robot last saw the ball. -1.f if we haven't seen it

    // position of ball relative to the robot
    // coordinates in millimeters
    // 0,0 is in centre of the robot
    // +ve x-axis points forward from the robot
    // +ve y-axis is 90 degrees counter clockwise from the +ve x-axis
    public float[] ball = new float[2];

    // velocity of the ball (same coordinate system as above)
    // the unit is millimeters per second
    public float[] ballVel = new float[2];

    // describes what - in the robot's opinion - the teammates should do:
    public enum Suggestion {

        NOTHING, // 0 - nothing particular (default)
        KEEPER, // 1 - play keeper
        DEFENSE, // 2 - support defense
        OFFENSE, // 3 - support the ball
        PLAY_BALL // 4 - play the ball
    }
    public Suggestion[] suggestion = new Suggestion[SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS];

    // describes what the robot intends to do
    public enum Intention {

        NOTHING, // 0 - nothing particular (default)
        KEEPER, // 1 - wants to be keeper
        DEFENSE, // 2 - wants to play defense
        PLAY_BALL, // 3 - wants to play the ball
        LOST       // 4 - robot is lost
    }
    public Intention intention;

    // the average speed that the robot has, for instance, when walking towards the ball
    // the unit is mm/s
    // the idea of this value is to roughly represent the robot's walking skill
    // it has to be set once at the beginning of the game and remains fixed
    public short averageWalkSpeed;

    // the maximum distance that the ball rolls after a strong kick by the robot
    // the unit is mm
    // the idea of this value is to roughly represent the robot's kicking skill
    // it has to be set once at the beginning of the game and remains fixed
    public short maxKickDistance;

    // describes the current confidence of a robot about its self-location,
    // the unit is percent [0,..100]
    // the value should be updated in the course of the game
    public byte currentPositionConfidence;

    // describes the current confidence of a robot about playing in the right direction,
    // the unit is percent [0,..100]
    // the value should be updated in the course of the game
    public byte currentSideConfidence;

    // buffer for arbitrary data
    public int nominalDataBytes;
    public byte[] data;

    public boolean valid = false;
    public boolean headerValid = false;
    public boolean versionValid = false;
    public boolean playerNumValid = false;
    public boolean teamNumValid = false;
    public boolean fallenValid = false;
    public boolean poseValid = false;
    public boolean walkingToValid = false;
    public boolean shootingToValid = false;
    public boolean ballValid = false;
    public boolean[] suggestionValid = new boolean[SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS];
    public boolean intentionValid = false;
    public boolean averageWalkSpeedValid = false;
    public boolean maxKickDistanceValid = false;
    public boolean currentPositionConfidenceValid = false;
    public boolean currentSideConfidenceValid = false;
    public boolean dataValid = false;

    public List<String> errors = new LinkedList<>();

    public static SPLStandardMessage createFrom(final SPLStandardMessage message) {
        final SPLStandardMessage m = new SPLStandardMessage();
        m.header = message.header;
        m.version = message.version;
        m.playerNum = message.playerNum;
        m.teamNum = message.teamNum;
        m.fallen = message.fallen;
        m.pose = message.pose;
        m.walkingTo = message.walkingTo;
        m.shootingTo = message.shootingTo;
        m.ballAge = message.ballAge;
        m.ball = message.ball;
        m.ballVel = message.ballVel;
        m.suggestion = message.suggestion;
        m.intention = message.intention;
        m.averageWalkSpeed = message.averageWalkSpeed;
        m.maxKickDistance = message.maxKickDistance;
        m.currentPositionConfidence = message.currentPositionConfidence;
        m.currentSideConfidence = message.currentSideConfidence;
        m.nominalDataBytes = message.nominalDataBytes;
        m.data = message.data;
        m.valid = message.valid;
        m.headerValid = message.headerValid;
        m.versionValid = message.versionValid;
        m.playerNumValid = message.playerNumValid;
        m.teamNumValid = message.teamNumValid;
        m.fallenValid = message.fallenValid;
        m.poseValid = message.poseValid;
        m.walkingToValid = message.walkingToValid;
        m.shootingToValid = message.shootingToValid;
        m.ballValid = message.ballValid;
        m.suggestionValid = message.suggestionValid;
        m.intentionValid = message.intentionValid;
        m.averageWalkSpeedValid = message.averageWalkSpeedValid;
        m.maxKickDistanceValid = message.maxKickDistanceValid;
        m.currentPositionConfidenceValid = message.currentPositionConfidenceValid;
        m.currentSideConfidenceValid = message.currentSideConfidenceValid;
        m.dataValid = message.dataValid;
        return m;
    }

    public byte[] toByteArray() {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        buffer.put(header.getBytes());
        buffer.put(version);
        buffer.put(playerNum);
        buffer.put(teamNum);
        buffer.put(fallen ? (byte) 1 : (byte) 0);
        buffer.putFloat(pose[0]);
        buffer.putFloat(pose[1]);
        buffer.putFloat(pose[2]);
        buffer.putFloat(walkingTo[0]);
        buffer.putFloat(walkingTo[1]);
        buffer.putFloat(shootingTo[0]);
        buffer.putFloat(shootingTo[1]);
        buffer.putFloat(ballAge);
        buffer.putFloat(ball[0]);
        buffer.putFloat(ball[1]);
        buffer.putFloat(ballVel[0]);
        buffer.putFloat(ballVel[1]);
        for (final Suggestion s : suggestion) {
            buffer.put((byte) s.ordinal());
        }
        buffer.put((byte) intention.ordinal());
        buffer.putShort(averageWalkSpeed);
        buffer.putShort(maxKickDistance);
        buffer.put(currentPositionConfidence);
        buffer.put(currentSideConfidence);
        buffer.putShort((short) data.length);
        buffer.put(data);

        return buffer.array();
    }

    public boolean fromByteArray(ByteBuffer buffer) {
        try {
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            byte[] header = new byte[4];
            buffer.get(header);
            this.header = new String(header);
            if (!this.header.equals(SPL_STANDARD_MESSAGE_STRUCT_HEADER)) {
                errors.add("wrong header; expected " + SPL_STANDARD_MESSAGE_STRUCT_HEADER + ", is: " + this.header);
            } else {
                headerValid = true;

                version = buffer.get();
                if (version != SPL_STANDARD_MESSAGE_STRUCT_VERSION) {
                    errors.add("wrong version; expected " + SPL_STANDARD_MESSAGE_STRUCT_VERSION + ", is: " + version);
                } else {
                    versionValid = true;

                    playerNum = buffer.get();
                    if (playerNum < 1 || playerNum > 6) {
                        errors.add("player number not within [1,6]; is: " + playerNum);
                    } else {
                        playerNumValid = true;
                    }

                    teamNum = buffer.get();
                    if (teamNum < 0) {
                        errors.add("team number not set");
                    } else {
                        teamNumValid = true;
                    }

                    final byte fallenState = buffer.get();
                    switch (fallenState) {
                        case 0:
                            fallen = false;
                            fallenValid = true;
                            break;
                        case 1:
                            fallen = true;
                            fallenValid = true;
                            break;
                        default:
                            errors.add("invalid fallen state; expected 0 or 1, is: " + fallenState);
                    }

                    pose[0] = buffer.getFloat();
                    pose[1] = buffer.getFloat();
                    pose[2] = buffer.getFloat();
                    if (!Float.isNaN(pose[0]) && !Float.isNaN(pose[1]) && !Float.isNaN(pose[2])) {
                        poseValid = true;
                    }

                    walkingTo[0] = buffer.getFloat();
                    walkingTo[1] = buffer.getFloat();
                    if (!Float.isNaN(walkingTo[0]) && !Float.isNaN(walkingTo[1])) {
                        walkingToValid = true;
                    }

                    shootingTo[0] = buffer.getFloat();
                    shootingTo[1] = buffer.getFloat();
                    if (!Float.isNaN(shootingTo[0]) && !Float.isNaN(shootingTo[1])) {
                        shootingToValid = true;
                    }

                    ballAge = buffer.getFloat();

                    ball[0] = buffer.getFloat();
                    ball[1] = buffer.getFloat();

                    ballVel[0] = buffer.getFloat();
                    ballVel[1] = buffer.getFloat();
                    if (!Float.isNaN(ballAge) && !Float.isNaN(ball[0]) && !Float.isNaN(ball[1]) && !Float.isNaN(ballVel[0]) && !Float.isNaN(ballVel[1])) {
                        ballValid = true;
                    }

                    for (int i = 0; i < SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS; i++) {
                        int s = (int) buffer.get();
                        if (s == -1) {
                            s = 0;
                        }
                        if (s < 0 || s >= Suggestion.values().length) {
                            errors.add("invalid suggestion; expected value in [0," + (Suggestion.values().length - 1) + "], is: " + s);
                            suggestionValid[i] = false;
                        } else {
                            this.suggestion[i] = Suggestion.values()[s];
                            suggestionValid[i] = true;
                        }
                    }

                    int intention = (int) buffer.get();
                    if (intention < 0 || intention >= Intention.values().length) {
                        errors.add("invalid intention; expected value in [0," + (Intention.values().length - 1) + "], is: " + intention);
                    } else {
                        this.intention = Intention.values()[intention];
                        intentionValid = true;
                    }

                    averageWalkSpeed = buffer.getShort();
                    if (averageWalkSpeed < 0) {
                        errors.add("invalid average walk speed, is: " + averageWalkSpeed);
                    } else {
                        averageWalkSpeedValid = true;
                    }
                    maxKickDistance = buffer.getShort();
                    if (maxKickDistance < 0) {
                        errors.add("invalid maximum kick distance, is: " + maxKickDistance);
                    } else {
                        maxKickDistanceValid = true;
                    }

                    currentPositionConfidence = buffer.get();
                    if (currentPositionConfidence < 0 || currentPositionConfidence > 100) {
                        errors.add("invalid position confidence; expected in [0,100], is: " + currentPositionConfidence);
                    } else {
                        currentPositionConfidenceValid = true;
                    }
                    currentSideConfidence = buffer.get();
                    if (currentSideConfidence < 0 || currentSideConfidence > 100) {
                        errors.add("invalid side confidence; expected in [0,100], is: " + currentPositionConfidence);
                    } else {
                        currentSideConfidenceValid = true;
                    }

                    nominalDataBytes = buffer.getShort();
                    boolean dValid = true;
                    if (nominalDataBytes > SPL_STANDARD_MESSAGE_DATA_SIZE) {
                        errors.add("custom data size too large; allowed up to " + SPL_STANDARD_MESSAGE_DATA_SIZE + ", is: " + nominalDataBytes);
                        dValid = false;
                    }
                    if (buffer.remaining() < nominalDataBytes) {
                        errors.add("custom data size is smaller than named: " + buffer.remaining() + " instead of " + nominalDataBytes);
                        dValid = false;
                    }
                    data = new byte[nominalDataBytes];
                    buffer.get(data, 0, nominalDataBytes);
                    dataValid = dValid;
                }
            }
        } catch (RuntimeException e) {
            errors.add("error while reading message: " + e.getClass().getSimpleName() + e.getMessage());
        }

        valid = headerValid && versionValid && playerNumValid && teamNumValid && fallenValid && poseValid && walkingToValid && shootingToValid && ballValid && intentionValid && averageWalkSpeedValid && maxKickDistanceValid && currentPositionConfidenceValid && currentSideConfidenceValid && dataValid;
        if (valid) {
            for (final boolean v : suggestionValid) {
                if (!v) {
                    valid = false;
                    return false;
                }
            }
        }

        return valid;
    }
}
