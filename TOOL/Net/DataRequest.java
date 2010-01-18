
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

package TOOL.Net;

import TOOL.Data.DataTypes;

public class DataRequest {

    public static final DataTypes.DataType INFO    = DataTypes.DataType.INFO;
    public static final DataTypes.DataType JOINTS  = DataTypes.DataType.JOINTS;
    public static final DataTypes.DataType SENSORS = DataTypes.DataType.SENSORS;
    public static final DataTypes.DataType IMAGE   = DataTypes.DataType.IMAGE;
    public static final DataTypes.DataType THRESH  = DataTypes.DataType.THRESH;
    public static final DataTypes.DataType JPEG    = DataTypes.DataType.JPEG;
    public static final DataTypes.DataType OBJECTS = DataTypes.DataType.OBJECTS;
    public static final DataTypes.DataType MOTION  = DataTypes.DataType.MOTION;
    public static final DataTypes.DataType LOCAL   = DataTypes.DataType.LOCAL;
    public static final DataTypes.DataType COMM    = DataTypes.DataType.COMM;

    public static final int LENGTH = DataTypes.LENGTH;

    public static final DataRequest INFO_ONLY =
        new DataRequest(true, false, false, false, false, false, false, false,
                        false, false);
    public static final DataRequest IMAGE_ONLY =
        new DataRequest(false, false, false, true, false, false, false, false,
                        false, false);
    public static final DataRequest IMAGE_JOINTS =
        new DataRequest(false, true, false, true, false, false, false, false,
                        false, false);
    public static final DataRequest IMAGE_JOINTS_SENSORS =
        new DataRequest(false, true, true, true, false, false, false, false,
                        false, false);
    public static final DataRequest THRESH_ONLY =
        new DataRequest(false, false, false, false, true, false, false, false,
                        false, false);
    public static final DataRequest IMAGE_THRESH =
        new DataRequest(false, false, false, true, true, false, false, false,
                        false, false);
	public static final DataRequest OBJECTS_ONLY =
		new DataRequest(false, false, false, false, false,
						false, true, false, false, false);


    public static boolean isImplemented(DataTypes.DataType type) {
        switch (type) {
            case INFO:         return true;
            case JOINTS:       return true;
            case SENSORS:      return true;
            case IMAGE:        return true;
            case THRESH:       return true;
            case JPEG:         return false;
            case OBJECTS:      return false;
            case MOTION:       return false;
            case LOCAL:        return false;
            case COMM:         return false;
            default:           return false;
        }
    }

    private byte[] data;

    public DataRequest(boolean i, boolean j, boolean s, boolean img, boolean t,
					   boolean jpeg, boolean o, boolean m, boolean l, boolean c) {
        this(new boolean[] {i, j, s, img, t, jpeg, o, m, l, c});
    }
    public DataRequest(boolean[] values) {

        data = new byte[LENGTH];
        for (int i = 0; i < data.length; i++) {
            if (values[i])
                data[i] = (byte)1;
            else
                data[i] = (byte)0;
        }
    }
    public DataRequest(byte[] values) {

        data = new byte[LENGTH];
        System.arraycopy(values, 0, data, 0, LENGTH);
    }

    public byte[] getBytes() { return data; }
    public boolean info()    { return get(INFO   ); }
    public boolean joints()  { return get(JOINTS ); }
    public boolean sensors() { return get(SENSORS); }
    public boolean image()   { return get(IMAGE  ); }
    public boolean thresh()  { return get(THRESH ); }
    public boolean jpeg()    { return get(JPEG   ); }
    public boolean objects() { return get(OBJECTS); }
    public boolean motion()  { return get(MOTION ); }
    public boolean local()   { return get(LOCAL  ); }
    public boolean comm()    { return get(COMM   ); }

    public boolean get(DataTypes.DataType t) {
        return data[t.ordinal()] == 1;
    }
    public void set(DataTypes.DataType t, boolean b) {
        if (b) data[t.ordinal()] = 1;
        else   data[t.ordinal()] = 0;
    }
    public void flip(DataTypes.DataType t) {
        data[t.ordinal()] = ((byte)(((int)(data[t.ordinal()] + 1)) % 2));
    }

    public void setInfo   (boolean b) { set(INFO,    b); }
    public void setJoints (boolean b) { set(JOINTS,  b); }
    public void setSensors(boolean b) { set(SENSORS, b); }
    public void setImage  (boolean b) { set(IMAGE,   b); }
    public void setThresh (boolean b) { set(THRESH,  b); }
    public void setJPEG   (boolean b) { set(JPEG,    b); }
    public void setObjects(boolean b) { set(OBJECTS, b); }
    public void setMotion (boolean b) { set(MOTION,  b); }
    public void setLocal  (boolean b) { set(LOCAL,   b); }
    public void setComm   (boolean b) { set(COMM,    b); }

}
