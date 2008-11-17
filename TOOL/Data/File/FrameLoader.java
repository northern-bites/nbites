package edu.bowdoin.robocup.TOOL.Data.File;


import edu.bowdoin.robocup.TOOL.TOOLException;
import edu.bowdoin.robocup.TOOL.Image.*;
import java.io.*;
import java.util.*;
import edu.bowdoin.robocup.TOOL.Data.DataModule;
import edu.bowdoin.robocup.TOOL.Data.Frame;
import edu.bowdoin.robocup.TOOL.Data.RobotDef;

/**
 * Parses the extensions of the files and determines how best to load them.
 * Passes work off onto constituent class.
 * @author Nicholas Dunn
 */

public class FrameLoader implements FileFilter {

    public static final FileFilter FILTER = new FrameLoader();
    
    public static final String BMP_EXT = ".BMP";
    public static final String AIBO_EXT = ".FRM";
    public static final String NAO_EXT = ".NFRM";
    public static final String NAO_SIM_EXT = ".NSFRM";

    public static final int AIBO_IMAGE_WIDTH = 208;
    public static final int AIBO_IMAGE_HEIGHT = 160;
    
    public static final String EXTENSIONS[] = {
        AIBO_EXT,
        NAO_EXT,
        NAO_SIM_EXT,
        BMP_EXT
    };

    public static final String ROBOT_EXTS[] = {
        AIBO_EXT,
        AIBO_EXT,
        AIBO_EXT,
        NAO_EXT,
        NAO_EXT,
        NAO_SIM_EXT,
    };

    public static final int AIBO_HEADER_SIZE = 100;

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
            byte[] image;
            byte[] footer;
            
            if (upper.endsWith(NAO_EXT)) {
                frm.setImage(new YUV422Image(input,
                                             RobotDef.NAO_DEF.imageWidth(),
                                             RobotDef.NAO_DEF.imageHeight()));

            }else if (upper.endsWith(AIBO_EXT)) {
                // skip header
                input.skip(AIBO_HEADER_SIZE);
                // read image 
                frm.setImage(new YCbCrImage(input,
                                            RobotDef.ERS7_DEF.imageWidth(), 
                                            RobotDef.ERS7_DEF.imageHeight())); 
                // read footer
                footer = new byte[input.available()];
                input.readFully(footer);

                String fullFooter = new String(footer, "ASCII");
                String[] values = fullFooter.split(" ");
                Vector<Float> joints = new Vector<Float>();
                for (int i = 0; i < values.length; i++) {
                    try {
                        joints.add(Float.parseFloat(values[i]));
                    }catch (NumberFormatException e) {
                        break;
                    }
                }
                frm.setJoints(joints);

            }else if (upper.endsWith(NAO_SIM_EXT)) {
                frm.setImage(new RGBImage(input,
                                          RobotDef.NAO_SIM_DEF.imageWidth(),
                                          RobotDef.NAO_SIM_DEF.imageHeight()));
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
            }else if (upper.endsWith(BMP_EXT)) {
                frm.setImage(new BMPImage(input,
                                          AIBO_IMAGE_WIDTH, AIBO_IMAGE_HEIGHT));
            }

        }catch (IOException e) {
            // raise TOOLException from IOException source, with module info
            DataModule.raiseError(DataModule.class, e);
        }
    }

    public static TOOLImage loadBytes(int type, byte[] data) {
        RobotDef def;
        switch (type) {
            case RobotDef.AIBO:
            case RobotDef.AIBO_ERS7:
            case RobotDef.AIBO_220:
                return new YCbCrImage(data,
                        RobotDef.ROBOT_DEFS[type].imageWidth(),
                        RobotDef.ROBOT_DEFS[type].imageHeight());

            case RobotDef.NAO:
            case RobotDef.NAO_RL:
                return new YUV422Image(data,
                        RobotDef.ROBOT_DEFS[type].imageWidth(),
                        RobotDef.ROBOT_DEFS[type].imageHeight());

            case RobotDef.NAO_SIM:
                return new RGBImage(data,
                        RobotDef.ROBOT_DEFS[type].imageWidth(),
                        RobotDef.ROBOT_DEFS[type].imageHeight());

            default:
                DataModule.logError(DataModule.class, "Undefined robot type");
                return null;
        }
    }

    public static void storeFrame(File f, Frame frm) throws TOOLException {

        try {
            FileOutputStream fos = new FileOutputStream(f);
            DataOutputStream output = new DataOutputStream(fos);
    
            switch (frm.type()) {
                case RobotDef.AIBO:
                case RobotDef.AIBO_220:
                case RobotDef.AIBO_ERS7:
                    output.write(new byte[AIBO_HEADER_SIZE]);
    
                    if (frm.hasImage())
                        frm.image().writeOutputStream(output);
    
                    if (frm.hasJoints())
                        for (Float fl : frm.joints())
                            output.writeBytes(fl.toString() + " ");
                    break;
    
                case RobotDef.NAO:
                case RobotDef.NAO_RL:
                    if (frm.hasImage())
                        frm.image().writeOutputStream(output);
                    if (frm.hasJoints())
                        for (Float fl : frm.joints())
                            output.writeBytes(fl.toString() + " ");
                    break;

                case RobotDef.NAO_SIM:
                    frm.image().writeOutputStream(output);
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
        
        for (int i = 0; i < EXTENSIONS.length; i++)
            if (upper.endsWith(EXTENSIONS[i]))
                return true;

        return false;
        
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
