
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

import java.awt.image.BufferedImage;
import java.util.List;

import TOOL.TOOLException;
import TOOL.Image.TOOLImage;

/**
 * Holds all data specific to one recorded frame in a robot's log.  May contain
 * any or all of the following: image, joint values, object recognition data,
 * behavioral action records, human comments about the frame.  If an image is
 * available, a preview by default will be available by resizing the image.  If
 * a sub-class would like to provide a pre-made preview image, it should
 * override the corresponding methods.
 */
public class Frame {

    private int frame_index;
    protected DataSet frame_dataSet;

    private String frame_robot;
    private int frame_type;
    private RobotDef frame_def;

    protected TOOLImage frame_image;
    protected BufferedImage frame_preview;
    protected List<Float> frame_joints;
    protected List<Float> frame_sensors;
    protected List<String> frame_tags;
    //protected List<*Object*> frame_objs;
    //protected List<Run> frame_runs;
    protected List<String> frame_states;

    public Frame(DataSet set, int i) {
        frame_index = i;
        frame_dataSet = set;

        frame_robot = "Unknown";
        frame_type = RobotDef.AIBO_ERS7;
        frame_def = RobotDef.ERS7_DEF;

        frame_image = null;
        frame_preview = null;
        frame_joints = null;
        frame_sensors = null;
	frame_tags = null;
	//frame_objs = null;
	//frame_runs = null;
	frame_states = null;
    }

    public Frame() {
		// TODO Auto-generated constructor stub
	}

	public boolean hasImage() {
        return frame_image != null;
    }

    public boolean hasJoints() {
        return frame_joints != null;
    }

    public boolean hasSensors() {
        return frame_sensors != null;
    }

    public boolean hasObjects() {
        return false;
    }

    public boolean hasRuns() {
	return false;
    }

    public boolean hasStates() {
        return false;
    }

    public boolean hasTags() {
        return false;
    }

    public DataSet dataSet() {
        return frame_dataSet;
    }

    public int index() {
        return frame_index;
    }

    public boolean loaded() {
        return dataSet().loaded(index());
    }

    public void load() throws TOOLException {
        dataSet().load(index());
    }

    public void store() throws TOOLException {
        dataSet().store(index());
    }

    public void unload() throws TOOLException {
        dataSet().unload(index());
    }

    public void mark() {
        dataSet().mark(index());
    }

    public boolean changed() {
        return dataSet().changed(index());
    }


    public int type() {
        return frame_type;
    }

    public String robot() {
        return frame_robot;
    }

    public List<Float> joints() {
        return frame_joints;
    }
    public List<Float> sensors() {
        return frame_sensors;
    }

    public TOOLImage image() {
        return frame_image;
    }

    public BufferedImage preview() {
        return frame_preview;
    }

    public List<String> tags() {
        return frame_tags;
    }

    /*
    public List<*Object*> objects() {
	return frame_objs;
    }

    public List<Run> runs() {
	return frame_runs;
    }
    */

    public List<String> states() {
	return frame_states;
    }

    // return calculated size value for this frame
    public int size() {
        return 0;
    }


    public void setType(int robotType) {
        frame_type = robotType;
    }

    public void setRobot(String robotName) {
        frame_robot = robotName;
    }

    public void setJoints(List<Float> j) {
        frame_joints = j;
    }
    
    public void setSensors(List<Float> s) {
        frame_sensors = s;
    }

    public void setImage(TOOLImage img) {
        frame_image = img;
    }

    public void setPreview(BufferedImage img) {
        frame_preview = img;
    }

    public void setTags(List<String> t) {
        frame_tags = t;
    }

    /*
    public void setObjects(List<*Object*> o) {
	frame_objs = o;
    }

    public void setRuns(List<Run> r) {
	frame_runs = r;
    }
    */

    public void setStates(List<String> s) {
	frame_states = s;
    }

}
