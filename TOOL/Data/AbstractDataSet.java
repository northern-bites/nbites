
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
import java.util.Vector;

import edu.bowdoin.robocup.TOOL.TOOLException;

/**
 * Holds the contents of one set of Frame data points.  On the back end
 * the data could be stored in FRM files, log files, a database, streamed from
 * a robot, etc.
 *
 * This is an abstract basic implementation of the DataSet interface, for use
 * in other Java implementations
 */
public abstract class AbstractDataSet implements DataSet {
    
    private DataSource set_source;
    private int    set_index;
    private String set_path;
    private String set_name;
    private String set_desc;
    private boolean auto_save;

    protected Vector<Frame> frames;
    protected Vector<Boolean> frameLoaded;
    protected Vector<Boolean> frameChanged;

    protected AbstractDataSet(DataSource src, int i, String path) {
        set_source = src;
        set_index = i;
        auto_save = false;
        set_path = path;
    }

    protected AbstractDataSet(DataSource src, int i, String path, String name,
            String desc, int size) {
        set_source = src;
        set_index = i;
        set_path = path;
        auto_save = false;

        init(name, desc, size);
    }

    public DataSource source() {
        return set_source;
    }

    public int index() {
        return set_index;
    }
    
    protected void init(String name, String desc, int size) {

        // initialize vectors to unloaded Frame objects
        frames = new Vector<Frame>(size);
        frameLoaded = new Vector<Boolean>(size);
        frameChanged = new Vector<Boolean>(size);

        for (int i = 0; i < size; i++) {
            frames.add(new Frame(this, i));
            frameLoaded.add(false);
            frameChanged.add(false);
        }

        set_name = name;
        set_desc = desc;
    }

    public int hasImages() {
        
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasImage())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }
    public int hasJoints() {
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasJoints())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }
    public int hasSensors() {
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasSensors())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }
    public int hasTags() {
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasTags())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }
    public int hasObjects() {
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasObjects())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }
    public int hasRuns() {
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasRuns())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }
    public int hasStates() {
        int sum = NONE;
        for (Frame f : frames)
            if (f.hasStates())
                sum++;

        if (sum == size())
            sum = ALL;
        else if (sum > 0)
            sum = MIXED;
        return sum;
    }

    public String path() {
        return set_path;
    }

    public String name() {
        return set_name;
    }
    
    public void setName(String s) {
        set_name = s;
    }

    public String desc() {
        return set_desc;
    }

    public void setDesc(String s) {
        set_desc = s;
    }

    public boolean dynamic() {
        return false;
    }

    public boolean autoSave() {
        return auto_save;
    }

    public void setAutoSave(boolean toAutoSave) {
        auto_save = toAutoSave;
    }

    public int size() {
        return frames.size();
    }

    public boolean changed() {
        for (Boolean c : frameChanged)
            if (c)
                return true;
        return false;
    }

    public boolean changed(int i) {
        return frameChanged.get(i);
    }

    public boolean loaded() {
        for (Boolean l : frameLoaded)
            if (!l)
                return false;
        return true;
    }
    
    public boolean loaded(int i) {
        return frameLoaded.get(i);
    }

    public Frame add(boolean loaded, boolean changed) {
        Frame f = new Frame(this, size());

        frames.add(f);
        frameLoaded.add(loaded);
        frameChanged.add(changed);

        return f;
    }

    public Frame get(int i) {
        return frames.get(i);
    }

    public void mark(int i) {
        frameChanged.set(i, true);
    }

    public void load() throws TOOLException {
        for (Frame f : frames)
            load(f.index());
    }

    public  void load(int offset, int span) throws TOOLException {
        if (offset < 0 || offset > size())
            DataModule.raiseError(DataModule.class,
                new ArrayIndexOutOfBoundsException("Invalid offset of " +
                offset));
        if (span < 1 || offset + span > size())
            DataModule.raiseError(DataModule.class,
                new ArrayIndexOutOfBoundsException("Invalid span of " +
                span));

        for (int i = offset; i < offset + span; i++)
            load(i);
    }

    public abstract void load(int i) throws TOOLException;

    /**
     * Save all loaded frames.  Used to save data sets.
     */
    public void store() throws TOOLException {
        for (int i = 0; i < size(); i++)
            store(i);
    }

    /**
     * Save all loaded frames, in the location specified.  Used to save data
     * sets in a new location, without changing the current store location.
     */
    public void store(String path) throws TOOLException {
        for (int i = 0; i < size(); i++)
            store(i, path);
    }

    public void store(int i) throws TOOLException {
        store(i, path());
    }

    public abstract void store(int i, String path) throws TOOLException;

    /**
    * unload() removes data associated with every frame
    * and removes them from the cache, if applicable.
    */
    public void unload() throws TOOLException {
        for (Frame f : frames)
            f.unload();
    }

    /**
    * unload(i) removes data associated with the frame at index i, 
    * and removes it from the cache, if applicable.
    */
    public void unload(int i) throws TOOLException {
        if (autoSave())
            store(i);
        clear(i);
        frameLoaded.set(i, false);
    }

    /**
     * Directly clear all data from the specified Datum.  Sets all values to
     * null.  Do not call this method directly unless you know what you're
     * doing.
     */
    protected void clear(int i) {
        Frame f = get(i);

        f.setPreview(null);
        f.setImage(null);
        f.setJoints(null);
        f.setSensors(null);
        f.setTags(null);
        //f.setObjects(null);
        //f.setRuns(null);
        f.setStates(null);
    }

    //
    // Iterable contract
    //

    public Iterator<Frame> iterator() {
        return frames.iterator();
    }
}
