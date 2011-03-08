package TOOL.Data.File;


import TOOL.TOOLException;
import TOOL.Image.*;
import java.io.*;
import java.util.*;
import TOOL.Data.DataModule;
import TOOL.Data.Frame;
import TOOL.Data.RobotDef;

/**
 * Parses the extensions of the files and determines how best to load them.
 * Passes work off onto constituent class.
 * @author Nicholas Dunn
 */

public class FrameLoader implements FileFilter {

    public static final FileFilter FILTER = new FrameLoader();

    public static void loadFrame(File f, Frame frm) throws TOOLException {
        loadFrame(f.getPath(), frm);
    }

    public static void loadFrame(String path, Frame frm)
            throws TOOLException {
        String upper = path.toUpperCase();

        try {
            FileInputStream fis = new FileInputStream(path);
            DataInputStream input = new DataInputStream(fis);
            byte[] header;
            byte[] footer;

            // This is the latest frame format.
            if (upper.endsWith(RobotDef.NAO_VERSIONED_EXT)) {
                RobotDef def = RobotDef.NAO_DEF_VERSIONED;
                frm.setImage
                    (new YUV422Image(input,
                                     def.inputImageDimensions().width,
                                     def.inputImageDimensions().height));
                // read footer
                footer = new byte[input.available()];
                input.readFully(footer);

                String fullFooter = new String(footer, "ASCII");
                String[] values = fullFooter.split(" ");
                Vector<Float> joints = new Vector<Float>();
                Vector<Float> sensors = new Vector<Float>();

                // get the frame version number
                int version = Integer.parseInt(values[0]);
                RobotDef.NAO_DEF_VERSIONED.setVersion(version);

                // skip the first byte because that's the version number
                int currentValue = 1;
                for (int i = 0;
                     i < RobotDef.NAO_DEF_VERSIONED.numJoints() &&
                         currentValue < values.length; i++, currentValue++) {
                    try {
                        joints.add(Float.parseFloat(values[currentValue]));
                    }catch (NumberFormatException e) {
                        break;
                    }
                }
                if (joints.size() == RobotDef.NAO_DEF_VERSIONED.numJoints())
                    frm.setJoints(joints);
                else
                    System.out.println("Couldn't read joints from file " +
                                       path);

                for (int i = 0;
                     i < RobotDef.NAO_DEF_VERSIONED.numSensors() &&
                         currentValue < values.length; i++, currentValue++) {
                    try {
                        sensors.add(Float.parseFloat(values[currentValue]));
                    } catch (NumberFormatException e) {
                        break;
                    }
                }
                if (sensors.size() == RobotDef.NAO_DEF_VERSIONED.numSensors())
                    frm.setSensors(sensors);
                else
                    System.out.println("Couldn't read sensors from file " +
                                       path);

            }
            else if (upper.endsWith(RobotDef.NAO_EXT)) {
                RobotDef def = RobotDef.NAO_DEF_VERSIONED;
                frm.setImage(new YUV422Image(input,
                                             def.inputImageDimensions().width,
                                             def.inputImageDimensions().height));

                // read footer
                footer = new byte[input.available()];
                input.readFully(footer);

                String fullFooter = new String(footer, "ASCII");
                String[] values = fullFooter.split(" ");
                Vector<Float> joints = new Vector<Float>();
                Vector<Float> sensors = new Vector<Float>();

                int currentValue = 0;
                for (int i = 0;
                     i < RobotDef.NAO_DEF_VERSIONED.numJoints() &&
                         currentValue < values.length; i++, currentValue++) {
                    try {
                        joints.add(Float.parseFloat(values[currentValue]));
                    }catch (NumberFormatException e) {
                        break;
                    }
                }
                if (joints.size() == RobotDef.NAO_DEF_VERSIONED.numJoints())
                    frm.setJoints(joints);
                else
                    System.out.println("Couldn't read joints from file " +
                                       path);

                for (int i = 0;
                     i < RobotDef.NAO_DEF_VERSIONED.numSensors() &&
                         currentValue < values.length; i++, currentValue++) {
                    try {
                        sensors.add(Float.parseFloat(values[currentValue]));
                    } catch (NumberFormatException e) {
                        break;
                    }
                }
                if (sensors.size() == RobotDef.NAO_DEF_VERSIONED.numSensors())
                    frm.setSensors(sensors);
                else
                    System.out.println("Couldn't read sensors from file " +
                                       path);

            }else if (upper.endsWith(RobotDef.NAO_SIM_EXT)) {

                RobotDef def = RobotDef.NAO_SIM_DEF;
                frm.setImage(new RGBImage(input,
                                          def.inputImageDimensions().width,
                                          def.inputImageDimensions().height));

                footer = new byte[input.available()];
                input.readFully(footer);

                String fullFooter = new String(footer, "ASCII");
                String[] values = fullFooter.split(" ");
                Vector<Float> joints = new Vector<Float>();
                for (int i = 0; i < values.length; i++) {
                    try {
                        joints.add(Float.parseFloat(values[i]));
                    }catch (NumberFormatException e) {
                        DataModule.logError(DataModule.class,
                                            "Failed to parse joint info.",
                                            e);
                    }
                }
            }

        }catch (IOException e) {
            // raise TOOLException from IOException source, with module info
            DataModule.raiseError(DataModule.class, e);
        }
    }

    public static TOOLImage loadBytes(RobotDef.ImageType type, byte[] data) {
        switch (type) {
            case NAO:
            case NAO_RL:
   		    case NAO_VER:
                return new YUV422Image(data,
                                       type.getRobotDef().
                                       inputImageDimensions().width,
                                       type.getRobotDef().
                                       inputImageDimensions().height);

            case NAO_SIM:
                return new RGBImage(data,
                                    type.getRobotDef().
                                    inputImageDimensions().width,
                                    type.getRobotDef().
                                    inputImageDimensions().height);
            default:
                DataModule.logError(DataModule.class, "Undefined robot type");
                return null;
        }
    }

    public static void storeFrame(File f, Frame frm) throws TOOLException {

		final Integer CURRENT_FRM_VERSION = new Integer(0);

        try {
            FileOutputStream fos = new FileOutputStream(f);
            DataOutputStream output = new DataOutputStream(fos);

            switch (frm.type()) {
			case NAO:
			case NAO_RL:
				if (frm.hasImage())
					frm.image().writeOutputStream(output);
				if (frm.hasJoints())
					for (Float fl : frm.joints())
						output.writeBytes(fl.toString() + " ");
				if (frm.hasSensors())
					for (Float fl : frm.sensors())
						output.writeBytes(fl.toString() + " ");
				break;

			case NAO_SIM:
				frm.image().writeOutputStream(output);
				break;
                // This writes a file of VERSION 0 for the TOOL
			case NAO_VER:
				if (frm.hasImage()) {
					frm.image().writeOutputStream(output);
				}
				// HACK. Should be done more eloquently?
				output.writeBytes(CURRENT_FRM_VERSION.toString() + " ");

				if (frm.hasJoints()){
					for (Float fl : frm.joints())
						output.writeBytes(fl.toString() + " ");
				}
				if (frm.hasSensors()){
					for (Float fl : frm.sensors())
						output.writeBytes(fl.toString() + " ");
				}
				break;
            }
        }catch (IOException e) {
            // raise TOOLException from IOException source, with module info
            DataModule.raiseError(DataModule.class, e);
        }
    }

    /**
     * Return true if and only if our ImageLoader class knows how to handle
     * the file you are attempting to load.
     * Currently those file extensions are: .FRM, .NSFRM, .RAW
     */
    public static boolean acceptableFormat(File f) {
        return f.isFile() && acceptableFormat(f.getPath());
    }
    public static boolean acceptableFormat(String imagePath) {

        String upper = imagePath.toUpperCase();

        return (upper.endsWith(RobotDef.NAO_EXT) ||
                upper.endsWith(RobotDef.NAO_SIM_EXT) ||
                upper.endsWith(RobotDef.NAO_VERSIONED_EXT));
    }

    /**
     * FileFilter evaluation method.  Used to automatically filter bad file
     * entries from directory listings.  Merely passes the File's basename
     * onto acceptableFormat()
     *
     * @param f The File object to check.
     * @return whether the File points to a .FRM, .NSFRM, or .raw file
     */
    public boolean accept(File f) {
        return acceptableFormat(f);
    }

}
