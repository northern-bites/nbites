
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

    public static final String NAO_EXT = ".NFRM";
    public static final String NAO_SIM_EXT = ".NSFRM";
    public static final String NAO_VERSIONED_EXT = ".FRM";

    public static final int NAO_RL_NUM = 3;
    public static final int NAO_SIM_NUM = 4;
    public static final int NAO_NUM = 5;
    public static final int NAO_RL_33_NUM = 6;

    public enum ImageType {
        NAO_RL("NAO_RL",
               "A real Nao robot",
               NAO_DEF_VERSIONED,
               NAO_VERSIONED_EXT),

            NAO_SIM("NAO_SIM",
                    "A simulated Nao robot",
                    NAO_SIM_DEF,
                    NAO_SIM_EXT),

            NAO("NAO",
                "A Nao robot of unspecified nature",
                NAO_DEF_VERSIONED,
                NAO_VERSIONED_EXT),

            // This is the latest frame format which includes a version number
            // in its header. That will allow us to parse it differently
            // according to version.
            NAO_VER("NAO_VERSIONED",
                    "A real-life Nao robot. Frame supports version number",
                    NAO_DEF_VERSIONED,
                    NAO_VERSIONED_EXT);

        private final String type;
        private final String desc;
        private final String ext;
        private final RobotDef def;

        ImageType(String type, String description, RobotDef def, String ext){
            this.type = type;
            this.desc = description;
            this.def = def;
            this.ext = ext;
        }

        public String getType() { return type; }
        public String getDescription() { return desc; }
        public String getExtension() { return ext; }
        public RobotDef getRobotDef() { return def;}
    }

    /**
     * Versions should allow us to change the number of sensors. I think we can
     * somewhat safely assume that the joints will not change in the near
     * future, so the only thing that can change is the number of sensors.
     * If you want to add a version, you need to add one more entry to the
     * array.
     **/
    private static int [] sensorsForVersions = {22, 0};
    public static final RobotDef NAO_DEF_VERSIONED =
        new RobotDef(ImageType.NAO_VER,
                     640, 480,
                     320, 240, 22,
                     // sensor number version 1
                     sensorsForVersions
                     );
    public static final RobotDef NAO_SIM_DEF =
        new RobotDef(ImageType.NAO_SIM, 160, 120,
                     160, 120,
                     22, 15);

    private ImageType type;
    private Dimension inputImageDim, outputImageDim;
    private int joints;

    private int sensors;
    private int [] versionSensors;
    private boolean hasVersions;
    private int version;

    private int inputImageSize, outputImageSize;

    // Version-less sensor constructor
    private RobotDef(ImageType raw_type,
                     int inputWidth, int inputHeight,
                     int outputWidth, int outputHeight,
                     int numJoints, int numSensors) {
        this(raw_type, inputWidth, inputHeight,
             outputWidth, outputHeight,
             numJoints);
        sensors = numSensors;
        hasVersions = false;
    }

    // Constructor that deals with sensors for versions
    private RobotDef(ImageType raw_type,
                     int inputWidth, int inputHeight,
                     int outputWidth, int outputHeight,
                     int numJoints, int [] numSensors) {
        this(raw_type, inputWidth, inputHeight,
             outputWidth, outputHeight,
             numJoints);

        versionSensors = numSensors;
        hasVersions = true;

        // this is a default value. you should call setVersion before you ask
        // the class how many sensors there are.
        version = -1;
    }

    // Constructor for all non-sensor data
    private RobotDef(ImageType raw_type,
                     int inputWidth, int inputHeight,
                     int outputWidth, int outputHeight,
                     int numJoints){
        type = raw_type;

        inputImageDim = new Dimension(inputWidth, inputHeight);
        outputImageDim = new Dimension(outputWidth, outputHeight);

        joints = numJoints;

        inputImageSize = inputWidth * inputHeight;
        outputImageSize = outputWidth * outputHeight;
    }

    public Dimension inputImageDimensions() {
        return inputImageDim;
    }

    public Dimension outputImageDimensions(){
        return outputImageDim;
    }

    public int inputImageSize() {
        return inputImageSize;
    }

    public int inputImageByteSize() {
        return inputImageSize * 2; // Only true for YUV422! Bit of a HACK
    }

    public int outputImageSize(){
        return outputImageSize;
    }

    public int numJoints() {
        return joints;
    }

    public void setVersion(int _version) {
        version = _version;
    }

    public int numSensors() {
        if (hasVersions)
            return versionSensors[version];
        else
            return sensors;
    }

    public ImageType imageType(){
        return type;
    }
}

