
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
 * Current State:  Vision processing is contingent on having built the jnilib
 *                 which contains the cpp vision code. You can compile it
 *                 by using 'make vision' in the tool/ base directory.
 *
 *  If the link is inactive, the regular image is displayed.
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


package TOOL.Vision;

//Graphics
import java.awt.Component;
import javax.swing.JPanel;

//Other
import java.util.Iterator;
import java.util.List;
import java.lang.Float;

//TOOL
import TOOL.TOOL;
import TOOL.TOOLModule;
import TOOL.Image.ImagePanel;
import TOOL.Image.TOOLImage;
import TOOL.Image.ThresholdedImage;
import TOOL.Image.ColorTable;
import TOOL.Data.Frame;
import TOOL.Data.DataSet;
import TOOL.Data.ColorTableListener;
import TOOL.Calibrate.ColorTableUpdate;
import TOOL.Vision.TOOLVisionLink;

public class VisionModule extends TOOLModule implements ColorTableListener {

    private ImagePanel imgPanel;
    private TOOLVisionLink visionLink;
    private ColorTable currentTable;

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


    protected void processFrame(){
        if(!active) return;
        if(!currentFrame.hasImage()) return; //Don't even bother if there's no image
        TOOLImage img = currentFrame.image();

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
        if(currentFrame.hasJoints()){
            List<Float> list_joints = currentFrame.joints();
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

        //create the byte buffer for the 2MB color table
        int ct_size =  currentTable.getYDimension()*
            currentTable.getUDimension()*currentTable.getVDimension();
        byte[] rawTable = new byte[ct_size];
        currentTable.writeByteArray(rawTable);

        //If we've made it this far, everything is A OK, so process the image,
        // yields a thresh image
        byte[][] thresh = visionLink.processImage(rawImage,joints,
                                                  rawTable);
        //Init a new Thresholded image to display
        ThresholdedImage tImg = new ThresholdedImage(thresh,
                                                     img.getWidth(),
                                                     img.getHeight());
        imgPanel.updateImage(tImg);

    }

    /**
     *  Called evertime we change frames in the data set.
     *  Currently attempts to run cpp vision on the new frame each time
     */
    public void notifyFrame(Frame d) {
        currentFrame = d;
        processFrame();
    }

    public void colorTableChanged(ColorTable source, ColorTableUpdate update,
                                  ColorTableListener originator) {
        currentTable = source;
        processFrame();
    }

    public void newColorTable(ColorTable newTable){
        setColorTable(newTable);
        //not needed since notifyFrame is called in this instance,
        //processFrame();
    }

    public void setColorTable(ColorTable newTable){
        currentTable = newTable;
    }

}
