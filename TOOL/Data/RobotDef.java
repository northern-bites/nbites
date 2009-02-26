
// This file is part of TOOL, a robotics interaction and development
// package created by the Northern Bites RoboCup team of Bowdoin College
// in Brunswick, Maine.
//
// TOOL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TOOL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TOOL.  If not, see <http://www.gnu.org/licenses/>.

package TOOL.Data;

import java.awt.Dimension;

public class RobotDef {

    public static final int AIBO_ERS7 = 0;
    public static final int AIBO_220  = 1;
    public static final int AIBO      = 2;
    public static final int NAO_RL    = 3;
    public static final int NAO_SIM   = 4;
    public static final int NAO       = 5;

    public static final String ROBOT_TYPES[] = {
        "AIBO_ERS7",
        "AIBO_220",
        "AIBO",
        "NAO_RL",
        "NAO_SIM",
        "NAO",
        };

    public static final String ROBOT_DESCS[] = {
        "An Aibo model ERS-7",
        "An Aibo model 220",
        "An Aibo of unspecified type",
        "A real-life Nao robot",
        "A simulated Nao robot",
        "A Nao robot of unspecified nature"
        };

    public static final RobotDef ERS7_DEF =
        new RobotDef(AIBO_ERS7, 208, 160, 18, 16);
    public static final RobotDef ERS220_DEF =
        new RobotDef(AIBO_220, 208, 160, 15, 16);
    public static final RobotDef NAO_DEF_HIGH =
        new RobotDef(NAO_RL, 640, 480, 640*480*2, 22, 21);
    public static final RobotDef NAO_DEF =
        new RobotDef(NAO_RL, 320, 240, 320*240*2, 22, 21);
    public static final RobotDef NAO_SIM_DEF =
        new RobotDef(NAO_SIM, 160, 120, 22, 15);

    public static final RobotDef ROBOT_DEFS[] = {
        ERS7_DEF,
        ERS7_DEF,
        ERS220_DEF,
        NAO_DEF,
        NAO_SIM_DEF,
        NAO_DEF,
    };

    private int type;
    private Dimension imageDim;
    private int joints;
    private int sensors;
    private int imageSize;

    private RobotDef(int raw_type, int width, int height, int numJoints,
            int numSensors) {
        this(raw_type, width, height, width*height*3, numJoints, numSensors);
    }

    private RobotDef(int raw_type, int width, int height, int size,
            int numJoints, int numSensors) {

        type = raw_type;
        imageDim = new Dimension(width, height);
        joints = numJoints;
        sensors = numSensors;
        imageSize = size;
    }

    public int imageWidth() {
        return imageDim.width;
    }

    public int imageHeight() {
        return imageDim.height;
    }

    public int rawImageSize() {
        return imageSize;
    }

    public Dimension imageDimensions() {
        return new Dimension(imageDim);
    }

    public int numJoints() {
        return joints;
    }

    public int numSensors() {
        return sensors;
    }

    public String toString() {
        return ROBOT_TYPES[type];
    }
}

