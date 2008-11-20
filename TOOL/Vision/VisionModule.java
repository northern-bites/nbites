
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

/**
 * This Module is in charge of offline debugging. It listens on the data set
 * and each time a new frame is available, it sends it through cpp to be
 * analyzed.
 *
 * Current State:  You must have a "table.mtb" in the tool/ dir.
 *   If you also did 'make vision' then the link to cpp will be active
 *   and if you load a frame in the data manager, it's thresholded
 *   representation will be made visible under the Vision tab.
 *
 *  If the link is inactive, or there is no color table, the regular image
 *  is displayed.
 *
 *  IMPORTANT: Joint angles are currently not being sent to vision!!!
 *
 *  For example, use the data set nao/balls/ and the color table in the tar at
 *  https://robocup.bowdoin.edu/files/random/jnitoolframes.tar
 *
 * @author Johannes Strom
 * @date November 2008
 *
 */


package edu.bowdoin.robocup.TOOL.Vision;

//Graphics
import java.awt.Component;
import javax.swing.JPanel;

//Other
import java.util.Iterator;
import java.util.List;
import java.lang.Float;

//hack -- remove this include later
import java.io.File;

//TOOL
import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.TOOLModule;
import edu.bowdoin.robocup.TOOL.Image.ImagePanel;
import edu.bowdoin.robocup.TOOL.Image.TOOLImage;
import edu.bowdoin.robocup.TOOL.Image.ThresholdedImage;
import edu.bowdoin.robocup.TOOL.Data.Frame;
import edu.bowdoin.robocup.TOOL.Data.DataSet;
import edu.bowdoin.robocup.TOOL.Vision.TOOLVisionLink;

public class VisionModule extends TOOLModule  {

    private ImagePanel imgPanel;
    private TOOLVisionLink visionLink;

    //Controllers if we run vision processing on images or not
    private boolean active;

    public VisionModule(TOOL tool) {
        super(tool);
        active = true;

        tool.getDataManager().addDataListener(this);
        imgPanel = new ImagePanel();
        visionLink = new TOOLVisionLink();
    }

    public String getDisplayName() {
        return "Vision";
    }

    public Component getDisplayComponent() {
        return imgPanel;
    }
    public void setActive(boolean _active){
        active  = _active;
    }
    public boolean isActive(){
        return active;
    }

    /**
     *  Called evertime we change frames in the data set.
     *  Currently attempts to run cpp vision on the new frame each time
     */
    public void notifyFrame(Frame d) {
        currentFrame = d;
        if(!d.hasImage()) return; //Don't even bother if there's no image
        TOOLImage img = d.image();

        //Double check to see if the link is actually active (ie lib was found)
        if(!visionLink.isLinkActive()){
            imgPanel.updateImage(img);
            t.CONSOLE.message("VisionModule:: VisionLink is not active,"+
                              "so displaying regular image instead");
            imgPanel.updateImage(img);
            return;
        }
        //Get the joints from the frame if it exists
        float[] joints  =new float[22]; //default length for Nao
        if(d.hasJoints()){
            List<Float> list_joints = d.joints();
            joints  =new float[list_joints.size()];
            int i = 0;
            for(Iterator itr = list_joints.iterator(); itr.hasNext(); ){
                joints[i++] = ((Float)itr.next()).floatValue();
            }
        }else{
            t.CONSOLE.message("Warning: Processing image w/o joint info");
        }

        //create the byte buffer for a YUV422 image to send to cpp
        byte[] rawImage = new byte[img.getWidth()*img.getHeight()*2];
        img.writeByteArray(rawImage);

        //current hack to load a cpp color table
        //Eventually, need to pass an instance of the table to cpp
        String colorTable = "table.mtb";
        File ct = new File(colorTable);
        if(!ct.exists()){
            t.CONSOLE.error("Color table path \"" + colorTable +
                            "\" not found. Displaying reg. image instead");
            imgPanel.updateImage(img);
            return;
        }

        //If we've made it this far, everything is A OK, so process the image,
        // yields a thresh image
        byte[][] thresh = visionLink.processImage(rawImage,
                                                  joints,colorTable);
        //Init a new Thresholded image to display
        ThresholdedImage tImg = new ThresholdedImage(thresh,
                                                     img.getWidth(),
                                                     img.getHeight());
        imgPanel.updateImage(tImg);

    }
}
