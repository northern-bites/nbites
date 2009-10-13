
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

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

import TOOL.TOOL;
import TOOL.Data.RobotDef;

public class TOOLProtocol {

    public static final byte COMMAND_MSG = 0;
    public static final byte REQUEST_MSG = 1;
    public static final byte DISCONNECT = 2;

    public static final byte CMD_TABLE  = 0;
    public static final byte CMD_MOTION = 1;
    public static final byte CMD_HEAD   = 2;
    public static final byte CMD_JOINTS = 3;

    public static final byte USE_TOOL_DATA = 0;

    public static final int NUM_MOTION_ENG = 4;
    public static final int NUM_HEAD_ENG   = 4;

    private DataSerializer serial;

    private RobotDef robotDef;

    private boolean connected;
    private boolean gotInfo;

    private InetAddress host;
    private byte robotType;
    private String robotName;
    private String calFile;
    private float[] joints;
    private float[] sensors;
    private byte[] image;
    private byte[] thresh;

    public TOOLProtocol(String remoteHost) {
        try {
            host = InetAddress.getByName(remoteHost);
            serial = new DataSerializer(host);
        }catch(UnknownHostException e) {
            NetworkModule.logError(NetworkModule.class,
                "Could not resolve address for remote host name", e);
            connected = false;
        }

        robotDef = null;

        connected = false;
        gotInfo = false;

        robotType = 0;
        robotName = null;
        calFile = null;
        joints = null;
        sensors = null;
        image = null;
        thresh = null;
    }

    public TOOLProtocol(InetAddress remoteHost) {
        host = remoteHost;
        serial = new DataSerializer(host);

        robotDef = null;

        connected = false;
        gotInfo = false;

        robotType = 0;
        robotName = null;
        calFile = null;
        joints = null;
        sensors = null;
        image = null;
        thresh = null;
    }

    public void connect(InetAddress host) {
        if (connected)
            disconnect();

        try {
            serial.connect();
            connected = true;
        }catch (IOException e) {
            connected = false;
        }
    }

    public void disconnect() {
        if (connected) {
            connected = false;

            try {
                serial.writeByte(DISCONNECT);
                serial.flush();
            }catch (IOException e) {
            }
        }
        serial.close();
    }

    public void initConnection() {
        try {
            if (!connected) {
                serial.connect();
                connected = true;
            }

            request(DataRequest.INFO_ONLY);
        }catch (IOException e) {
            NetworkModule.logError(NetworkModule.class,
                "Error on serial connection to remote host.", e);
            connected = false;
        }
    }

    public void processInfo() {
        gotInfo = true;
        switch (robotType) {
		case RobotDef.AIBO:
		case RobotDef.AIBO_ERS7:
			robotDef = RobotDef.ERS7_DEF;
			break;
		case RobotDef.AIBO_220:
			robotDef = RobotDef.ERS220_DEF;
			break;
		case RobotDef.NAO:
		case RobotDef.NAO_RL:
		case RobotDef.NAO_VER:
			robotDef = RobotDef.NAO_DEF_VERSIONED;
			robotDef.setVersion(0);
			break;
		case RobotDef.NAO_SIM:
			robotDef = RobotDef.NAO_SIM_DEF;
			break;
		default:
			robotDef = null;
			gotInfo = false;
        }

        if (robotDef != null) {
            joints = new float[robotDef.numJoints()];
            sensors = new float[robotDef.numSensors()];
            image = new byte[robotDef.rawImageSize()];
            thresh = new byte[robotDef.imageWidth() * robotDef.imageHeight()];
        }
    }

    public void request(DataRequest r) {
        if (!connected)
            return;

        try {

            serial.writeByte(REQUEST_MSG);
            serial.writeBytes(r.getBytes());
            serial.flush();

            if (r.info()) {
                robotType = serial.readByte();
				if (robotType == RobotDef.NAO_RL || robotType == RobotDef.NAO){
					robotType = RobotDef.NAO_VER;
				}
                byte buf[] = new byte[1024];
                int length = serial.readBytes(buf, true);
                robotName = new String(buf, 0, length, "US-ASCII");

                length = serial.readBytes(buf, true);
                calFile = new String(buf, 0, length, "US-ASCII");
                processInfo();
            }

            if (!gotInfo)
                return;

            if (r.joints())
                serial.readFloats(joints);

            if (r.sensors())
                serial.readFloats(sensors);

            if (r.image())
                serial.readBytes(image);

            if (r.thresh())
                serial.readBytes(thresh);

        }catch (IOException e) {
            TOOL.CONSOLE.error(e);
            disconnect();
        }
    }

    public void send(DataRequest r) {

    }

    public boolean isConnected() {
        return connected;
    }

    public boolean hasInfo() {
        return gotInfo;
    }

    public String getHost() {
        if (connected)
            return host.getHostAddress();
        return null;
    }

    public int getRobotType() {
        return robotType;
    }

    public RobotDef getRobotDef() {
        return robotDef;
    }

    public String getRobotName() {
        if (connected)
            return robotName;
        return null;
    }

    public String getCalFile() {
        if (connected)
            return calFile;
        return null;
    }

    public Float[] getJoints() {
        if (connected) {
            Float[] a = new Float[joints.length];
            for (int i = 0; i < joints.length; i++)
                a[i] = joints[i];
            return a;
        }
        return null;
    }

    public Float[] getSensors() {
        if (connected) {
            Float[] a = new Float[sensors.length];
            for (int i = 0; i < sensors.length; i++)
                a[i] = sensors[i];
            return a;
        }
        return null;
    }

    public byte[] getImage() {
        if (connected)
            return image;
        return null;
    }

    public byte[] getThresh() {
        if (connected)
            return thresh;
        return null;
    }

    public void sendMotion(double[] motion) {
        try {
            serial.writeByte(COMMAND_MSG);
            serial.writeByte(CMD_MOTION);
            serial.writeDoubles(motion);
            serial.flush();
        }catch (IOException e) {
            NetworkModule.logError(NetworkModule.class,
                "Sending motion command failed", e);
            disconnect();
        }
    }

    public void sendHead(double[] head) {
        try {
            serial.writeByte(COMMAND_MSG);
            serial.writeByte(CMD_MOTION);
            serial.writeDoubles(head);
            serial.flush();
        }catch (IOException e) {
            NetworkModule.logError(NetworkModule.class,
                "Sending head command failed", e);
            disconnect();
        }
    }

}
