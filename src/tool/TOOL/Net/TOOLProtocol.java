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
import java.util.Vector;

import TOOL.TOOL;
import TOOL.Data.RobotDef;
import TOOL.Data.DataTypes;
import TOOL.WorldController.Observation;
import TOOL.WorldController.LocalizationPacket;

public class TOOLProtocol {

    public static final byte COMMAND_MSG = 0;
    public static final byte REQUEST_MSG = 1;
    public static final byte DISCONNECT = 2;

    public static final int NUM_MOTION_ENG = 4;
    public static final int NUM_HEAD_ENG   = 4;

    private static final int NUM_POSSIBLE_OBJECTS = 120;
    private static final float INIT_OBJECT_VALUE = -1337.0f;

    private static final int NUM_LOC_PACKET_VALUES = 17;
    private static final int NUM_MM_VALUES = 300;

    private static final int NUM_GC_VALUES = 3;
    private static final int GC_TEAM_INDEX = 0;
    private static final int GC_PLAYER_INDEX = 1;
    private static final int GC_COLOR_INDEX = 2;

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
    private float[] objects;
    private float[] local;
    private byte[] image;
    private byte[] thresh;
    private int[] GCInfo;
    private float[] MMInfo;

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
        objects = null;
		local = null;
		GCInfo = null;
		MMInfo = null;
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
		objects = null;
		local = null;
		GCInfo = null;
		MMInfo = null;
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
                request(DataRequest.INFO_ONLY);
            }
        }catch (IOException e) {
            NetworkModule.logError(NetworkModule.class,
                "Error on serial connection to remote host.", e);
            connected = false;
        }
    }

    public void processInfo() {
        gotInfo = true;
        robotDef = getRobotType().getRobotDef();
        if (robotDef == RobotDef.NAO_DEF_VERSIONED) {
            robotDef.setVersion(0);
        }

        if (robotDef != null) {
            joints = new float[robotDef.numJoints()];
            sensors = new float[robotDef.numSensors()];

            image = new byte[robotDef.inputImageByteSize()];
            thresh = new byte[robotDef.outputImageSize()];
        }
    }

    public void request(DataRequest r) {
        if (!connected)
            return;
        byte[] data = r.getBytes();
        for (int i=0; i < data.length; i++){
            byte type = data[i];

            if (type == 1 &&
                !DataRequest.isImplemented(DataTypes.DataType.values()[i])){
                System.out.println("Data Request is not implemented");
                return;
            }
        }

        try {

            serial.writeByte(REQUEST_MSG);
            serial.writeBytes(r.getBytes());
            serial.flush();

            if (r.info()) {
                robotType = serial.readByte();
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

			if (r.objects()){
				objects = new float[NUM_POSSIBLE_OBJECTS];
				for (int i=0; i < objects.length ; ++i)
					objects[i] = INIT_OBJECT_VALUE;
				serial.readFloats(objects,true);
			}

			if (r.local()){
				local = new float[NUM_LOC_PACKET_VALUES];
				serial.readFloats(local,false);
			}

			if (r.comm()){
				GCInfo = new int[NUM_GC_VALUES];
				serial.readInts(GCInfo);
			}

			if (r.mmekf()){
				MMInfo = new float[NUM_MM_VALUES];
				for (int i=0; i < MMInfo.length ; ++i)
					MMInfo[i] = INIT_OBJECT_VALUE;
				serial.readFloats(MMInfo, true);
			}

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

    public RobotDef.ImageType getRobotType() {
        switch(robotType){
        case RobotDef.NAO_SIM_NUM:
            return RobotDef.ImageType.NAO_SIM;
        case RobotDef.NAO_NUM:
        case RobotDef.NAO_RL_33_NUM:
        case RobotDef.NAO_RL_NUM:
        default:
            return RobotDef.ImageType.NAO_VER;
        }
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

	public Vector<Observation> getObjects() {
		if (connected) {
			Vector<Observation> obs = new Vector<Observation>();
			for (int i = 0; i < objects.length; ++i){
				 if (objects[i] != INIT_OBJECT_VALUE)
					 obs.add(new Observation((int)objects[i], objects[++i], objects[++i]));
			}
			return obs;
		}
		return null;
	}

	public int getPlayer() {
		if (connected)
			return GCInfo[GC_PLAYER_INDEX];
		return -1;
	}

	public int getTeam() {
		if (connected)
			return GCInfo[GC_TEAM_INDEX];
		return -1;
	}

	public int getColor() {
		if (connected)
			return GCInfo[GC_COLOR_INDEX];
		return -1;
	}

	public LocalizationPacket getMyLocalization() {
		if (connected) {
		    LocalizationPacket myLoc =
			LocalizationPacket.makeEstimateAndUncertPacket(
								       local[0], local[1],
								       local[2], local[3],
								       local[4], local[5]);
		    return myLoc;
		}
		return null;
	}

	public LocalizationPacket getBallLocalization() {
		if (connected) {
			LocalizationPacket ballLoc =
				LocalizationPacket.makeBallEstimateAndUncertPacket(
																   local[6],local[7],
																   local[8], local[9],
																   local[10], local[11]);
			return ballLoc;
		}
		return null;
	}

	public Vector<LocalizationPacket> getMultimodalEKFInfo(){
		if (connected){
			Vector<LocalizationPacket> info = new Vector<LocalizationPacket>();
			int i = 0;
			while (MMInfo[i] != INIT_OBJECT_VALUE){
				info.add(
						 LocalizationPacket.
						 makeEstimateAndUncertPacket(
													 (double)MMInfo[i],
													 (double)MMInfo[++i],
													 (double)MMInfo[++i],
													 (double)MMInfo[++i],
													 (double)MMInfo[++i],
													 (double)MMInfo[++i] ));
				++i;
			}
			return info;
		}
		return null;
	}

    // cmd must be 256 bytes for now
    public void sendCommand(byte[] cmd) {
        System.out.println("\nSending a command now.\n");
        initConnection();
        try {
            serial.writeByte(COMMAND_MSG);
            serial.writeBytes(cmd);
            serial.flush();
        }catch (IOException e) {
            NetworkModule.logError(NetworkModule.class,
                "Sending command failed", e);
            disconnect();
        }
    }

}
