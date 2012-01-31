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

import java.io.File;
import java.util.Arrays;
import java.util.Vector;

import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;


import TOOL.TOOL;
import TOOL.TOOLException;
import TOOL.Data.DataSource;
import TOOL.Data.Frame;
import TOOL.Data.RobotDef;
import TOOL.Data.File.FileSet;
import TOOL.Data.File.FrameLoader;
import TOOL.Image.ThresholdedImage;
import TOOL.Image.TOOLImage;
import TOOL.WorldController.Observation;
import TOOL.WorldController.LocalizationPacket;
import TOOL.WorldController.PlayerInfo;

/**
 * A RemoteRobot object represents an advanced DataSet layered over a network
 * connection to a remote host.  It will have many options, but is backed
 * (currently) by a file set.  As it is dynamic, requests for data 'off the end'
 * of the DataSet will cause a request for new data to be requested and
 * retreived from the remote host.<br><br>
 *
 * Currently, only image retrieval is implemeneted on both sides, but we plan
 * to include accessibility for virtually all type of information from the
 * robot, and advanced controls over which data to retrieve each frame.<br><br>
 *
 * Also needing to be implemented is advanced memory handling.  This may be the
 * job of the underlying FileSet, not the RemoteRobot.  Currently all frames
 * are kept in memory unsaved until explicitly stored.  No memory-collection
 * and management features have been written.  The extra space in the Data
 * module and manager may be a great place for this.  In fact I'm copying this
 * comment there.
 */

public class RemoteRobot extends FileSet {

    public static final String NAME_UNKNOWN = "Unknown";
    public static final String ADDR_UNKNOWN = "0.0.0.0";
    public static final int SIZEOF_COMMAND = 256;

    private TOOLProtocol proto;
    private DataRequest requestDetails;

    public RemoteRobot(DataSource src, int i, String storagePath, String host,
            InetAddress addr) {
        super(src, i, storagePath);

        proto = new TOOLProtocol(addr);
        requestDetails = DataRequest.IMAGE_JOINTS_SENSORS;

        init(host, addr.getHostAddress(), 0);
    }

    public boolean isConnected() {
        return proto.isConnected();
    }

    public DataRequest getRequestDetails() {
        return requestDetails;
    }

    //
    // DataSet methods
    //

    public boolean autoSave() {
        return true;
    }

    public boolean dynamic() {
        return true;
    }

    public Frame get(int i) {
        if (i >= size())
            // need to create a new Frame
            return add(false, false);
        else
            // return the existing frame
            return super.get(i);
    }

    public void load(int i) throws TOOLException {
        Frame f = get(i);
        if (f.loaded())
            return;

        if (!proto.isConnected())
            connect();

        proto.request(requestDetails);

        if (requestDetails.info()) {
            setName(proto.getRobotName());
            setDesc(proto.getHost());
        }

        f.setType(proto.getRobotType());
        f.setRobot(proto.getRobotName());

        Vector<Float> v;

        if (requestDetails.joints())
            f.setJoints(Arrays.asList(proto.getJoints()));

        if (requestDetails.sensors())
            f.setSensors(Arrays.asList(proto.getSensors()));

        if (requestDetails.image())
            f.setImage(FrameLoader.loadBytes(proto.getRobotType(),
                                             proto.getImage()));

        // Mark as loaded
        frameLoaded.set(f.index(), true);
        // Mark as changed, as we have not written to file yet
        frameChanged.set(f.index(), true);
    }

    public void connect() throws TOOLException {
        if (proto.isConnected())
            return;

        proto.initConnection();

        if (proto.hasInfo()) {
            setName(proto.getRobotName());
            setDesc(proto.getHost());
            NetworkModule.message(NetworkModule.class,
                "Robot " + proto.getRobotName() +
                " is type " + proto.getRobotDef());
        }else if (proto.isConnected()) {
            setName(NAME_UNKNOWN);
            setDesc(ADDR_UNKNOWN);
        }else
            NetworkModule.raiseError(NetworkModule.class,
                "Could not connect to remote robot");
    }

    public void disconnect() {
        TOOL.CONSOLE.message("Disconnecting from " + name());
        proto.disconnect();
    }

    public TOOLImage retrieveImage() {
        try {
            if (!proto.isConnected())
                connect();
            proto.request(DataRequest.IMAGE_ONLY);
            return FrameLoader.loadBytes(proto.getRobotType(),
										 proto.getImage());
        }catch (TOOLException e) {
            NetworkModule.logError("Attempt to retrieve direct image failed",
                                   e);
            return null;
        }
    }

    public ThresholdedImage retrieveThresh() {
        try {
            if (!proto.isConnected())
                connect();
            proto.request(DataRequest.THRESH_ONLY);
            return new ThresholdedImage(proto.getThresh(),
                                        proto.getRobotDef().
                                        outputImageDimensions().width,
                                        proto.getRobotDef().
                                        outputImageDimensions().height);
        }catch (TOOLException e) {
            NetworkModule.logError("Attempt to retrieve direct image failed",
                                   e);
            return null;
        }
    }

	public Vector<Observation> retrieveObjects() {
		try {
			if (!proto.isConnected())
				connect();
			proto.request(DataRequest.OBJECTS_ONLY);
			return proto.getObjects();
		} catch (TOOLException e) {
            NetworkModule.logError("Attempt to retrieve objects failed",
                                   e);
			return null;
		}
	}

	public Vector<LocalizationPacket> retrieveLocalization() {
		try {
			if ( !proto.isConnected())
				connect();
			proto.request(DataRequest.LOC_ONLY);

			Vector<LocalizationPacket>  locInfo =
				new Vector<LocalizationPacket>();
			locInfo.add(proto.getMyLocalization());
			locInfo.add(proto.getBallLocalization());

			return locInfo;
		} catch (TOOLException e) {
			NetworkModule.logError("Attempt to retrieve localization failed",
								   e);
			return null;
		}
	}

	public Vector<LocalizationPacket> retrieveMMLocEKF() {
		try {
			if ( !proto.isConnected())
				connect();
			proto.request(DataRequest.MMEKF_ONLY);

			return proto.getMultimodalEKFInfo();
		} catch (TOOLException e) {
			NetworkModule.logError("Attempt to retrieve multimodal" +
								   "information failed",
								   e);
			return null;
		}
	}


	public PlayerInfo retrieveGCInfo() {
		try {
			if (! proto.isConnected())
				connect();
			proto.request(DataRequest.COMM_ONLY);
			PlayerInfo info = new PlayerInfo(proto.getTeam(), proto.getPlayer(),
											 proto.getColor());
			return info;
		} catch(TOOLException e) {
			NetworkModule.logError("Attempt to retrieve game controller info failed",
								   e);
			return null;
		}
	}

	public void fillNewFrame(Frame f) {
		try {

			if (!proto.isConnected())
				connect();

			proto.request(DataRequest.IMAGE_JOINTS_SENSORS);

			f.setType(proto.getRobotType());
			f.setImage(FrameLoader.loadBytes(proto.getRobotType(), proto.getImage()));
			f.setJoints(Arrays.asList(proto.getJoints()));
			f.setSensors(Arrays.asList(proto.getSensors()));
		} catch (TOOLException e) {
			NetworkModule.logError("Attempt to retrieve joints failed",
								   e);
		}
	}

    // For sending a command; s will come from the input field of the GUI
    public void sendCmd(String s) {
        byte[] sBytes = s.getBytes();

        // Robot expects command to be 256 bytes for simplicity
        byte[] fix = new byte[SIZEOF_COMMAND];
        for (int i = 0; i < sBytes.length; i++) {
            fix[i] = sBytes[i];
        }

        // Terminating characer for the C string
        fix[sBytes.length] = '\0';

        proto.sendCommand(fix);
        System.out.println("Sent command: " + s);
    }
}
