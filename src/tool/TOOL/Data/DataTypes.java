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

import java.util.Collection;

public class DataTypes {

    public static enum DataType {
        INFO,
		JOINTS,
		SENSORS,
		IMAGE,
		THRESH,
		JPEG,
		OBJECTS,
		MOTION,
		LOCAL,
		COMM,
		MMEKF
		}
    public static final int LENGTH = DataType.values().length;

    public static String title(DataType type) {
        switch (type) {
            case INFO:			return "Robot Info";
            case JOINTS:		return "Joints";
            case SENSORS:		return "Sensors";
            case IMAGE:			return "Image";
            case THRESH:		return "Thresh Image";
            case JPEG:			return "JPEG Image";
            case OBJECTS:		return "Objects";
            case MOTION:		return "Motion";
            case LOCAL:			return "Localization";
            case COMM:			return "Comm";
            case MMEKF:         return "Multimodal EKF";
            default:			return "N/A";
        }
    }

    public static DataType[] types() {
        return DataType.values();
    }

}
