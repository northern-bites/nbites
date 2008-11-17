
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

package edu.bowdoin.robocup.TOOL.Data;

import java.awt.image.BufferedImage;
import java.lang.Iterable;
import java.util.Iterator;

import edu.bowdoin.robocup.TOOL.TOOLException;

/**
 * Holds the contents of one set of Frame data points.  On the back end
 * the data could be stored in FRM files, log files, a database, streamed from
 * a robot, etc.  See AbtractDataSet for a specific implementation and much
 * more verbose comments describing the features and methods.
 */
public interface DataSet extends Iterable<Frame> {

    // responses available to the has...() methods
    //   a given data set may have some Frames with certain data and others
    //   without
    public static final int NONE = 0;
    public static final int ALL = 1;
    public static final int MIXED = 2;

    public DataSource source();
    public int index();

    public int hasImages();
    public int hasJoints();
    public int hasSensors();
    public int hasTags();
    public int hasObjects();
    public int hasRuns();
    public int hasStates();

    public String path();
    public String name();
    public String desc();
    public void   setName(String s);
    public void   setDesc(String s);

    public boolean dynamic();
    public boolean autoSave();
    public void    setAutoSave(boolean autoSave);

    public int     size();
    public boolean changed();
    public boolean changed(int i);
    public boolean loaded();
    public boolean loaded(int i);

    public Frame   add(boolean loaded, boolean changed);
    public Frame   get(int i);
    public void    mark(int i);

    public void    load()                       throws TOOLException;
    public void    load(int offset, int number) throws TOOLException;
    public void    load(int i)                  throws TOOLException;

    public void    store()                      throws TOOLException;
    public void    store(String path)           throws TOOLException;
    public void    store(int i)                 throws TOOLException;
    public void    store(int i, String path)    throws TOOLException;

    public void    unload()                     throws TOOLException;
    public void    unload(int i)                throws TOOLException;


    // Iterable contract

    public Iterator<Frame> iterator();

}
