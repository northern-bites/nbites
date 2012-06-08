
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

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.Vector;

import javax.swing.JPanel;

import TOOL.TOOL;
import TOOL.TOOLException;
import TOOL.Calibrate.ColorTableUpdate;
import TOOL.GUI.ListPanel;
import TOOL.GUI.DataSetObject;
import TOOL.GUI.DataSetOptionPanel;
import TOOL.GUI.ListObject;
import TOOL.Image.ColorTable;

public class DataManager implements ItemListener,
                                    SourceListener {

    public static final int LIST_ROW_HEIGHT = 35;
    public static final int MAX_CACHE_SIZE = 5;

    private Vector<DataSet> dataSets;
    private int dataSetIndex;
    private int frameIndex;


    private ListPanel<DataSetObject> listPanel;
    private HashSet<DataListener> listeners;
    private HashSet<ColorTableListener> colorListeners;


    // the cache is a listing frame objects
    // that are in memory.
    private LinkedList<Frame> mainCache;
    private int cache_limit;

    public DataManager() {
        this(MAX_CACHE_SIZE);
    }

    public DataManager(int size_limit) {
        dataSets = new Vector<DataSet>();
        listeners = new HashSet<DataListener>();
        colorListeners = new HashSet<ColorTableListener>();
        mainCache = new LinkedList<Frame>();
        cache_limit = size_limit;

        dataSetIndex = -1;
        frameIndex = -1;

        listPanel = new ListPanel<DataSetObject>();
        listPanel.setRowHeight(LIST_ROW_HEIGHT);
        listPanel.addItemListener(this);
    }

    /**
     * Retrieve the ListPanel that displays the cells for the available
     * DataSets.
     *
     * @return the ListPanel object for display
     */
    public ListPanel<DataSetObject> getListPanel() {
        return listPanel;
    }

    /**
     * Retrieve the JPanel that contains components for setting and viewing
     * configurations for the selected DataSet.
     *
     * @return the JPanel object for display
     */
    public JPanel getOptionPanel() {
        if (activeSet() != null)
            return new DataSetOptionPanel(this, activeSet());
        return null;
    }

    public DataSet activeSet() {
        if (dataSetIndex < 0)
            return null;
		dataSets.get(dataSetIndex);
        return dataSets.get(dataSetIndex);
    }

    public Frame activeFrame() {
        if (dataSetIndex < 0)
            return null;
		activeSet().get(frameIndex);
        return activeSet().get(frameIndex);
    }

    public int activeSetIndex() {
        return dataSetIndex;
    }

    public int activeFrameIndex() {
        return frameIndex;
    }

    public void next() {
        useFrame(frameIndex + 1);
    }

    public void last(){
        useFrame(frameIndex - 1);
    }

    public void set(int i) {
        useFrame(i);
    }



    /**
     * Advances by amount; if at the end of the set, does NOT wrap
     * around.  If amount < 0, goes to previous frame.
     */
    public void advance(int amount) {
        if (activeSet() != null) {
            useFrame(frameIndex + amount);
        }
    }


    /**
     * Advances by amount; if at the end of the set, wraps around.
     */
    public void skip(int amount) {
        if (activeSet() != null) {
            useFrame((frameIndex + amount) % activeSet().size());
        }
    }

    public void revert(int amount) {
        if (activeSet() != null) {
            useFrame((frameIndex - amount) % activeSet().size());
        }
    }

    public void useFrame(int i) {
        if (i != frameIndex && i >= 0 && activeSet() != null) {
            boolean size_change = false;
            if (i >= activeSet().size())
                size_change = true;

            if (size_change && !activeSet().dynamic())
                return;

            if (safeLoad(activeSet(), i, mainCache)) {
                frameIndex = i;
                if (size_change)
                    // dynamic sets will create a new frame at index
                    //   (old_size), or index (new_size - 1) when successful
                    frameIndex = activeSet().size() - 1;
                notifyDependants(false);
            }
        }
    }

    public void addDataSet(DataSet d) {
        dataSets.add(d);
        listPanel.add(new DataSetObject(d));
    }

    public void addDataSets(List<DataSet> c) {
        dataSets.addAll(c);

        Vector<DataSetObject> v = new Vector<DataSetObject>();
        Iterator<DataSet> itr = c.iterator();
        while (itr.hasNext())
            v.add(new DataSetObject(itr.next()));

        listPanel.addAll(v);
    }

    public void useDataSet(int i) {
        if (i != dataSetIndex && i >= 0 && i < dataSets.size()) {
            mainCache.clear();
            DataSet d = dataSets.get(i);
            dataSetIndex = i;
            frameIndex = 0;
            if (safeLoad(d, frameIndex, mainCache))
                notifyDependants(true);
            else {
                dataSetIndex = -1;
                frameIndex = -1;
            }
        }
    }

    /**
     * @return true if and only if there are more elements after the current
     * frame
     */
    public boolean hasElementAfter() {
        return activeSet() != null &&
            (activeSet().dynamic() || frameIndex < activeSet().size() - 1);
    }

    /**
     * @return true if and only if there are more elements before the current
     * frame
     */
    public boolean hasElementBefore() {
        return frameIndex > 0;
    }


    public void useDataSet(DataSet d) {
        addDataSet(d);
        useDataSet(dataSets.size() - 1);
    }

    public int numDataSets() {
        return dataSets.size();
    }

    public void clear() {
        listPanel.clear();
        dataSets.clear();

        dataSetIndex = -1;
        frameIndex = -1;
    }

    public void saveCurrent() {
        try {
            if (activeSet() != null)
                activeSet().store();
        }catch (TOOLException e) {
            DataModule.logError("Couldn't save current Data Set", e);
        }
    }

    public void saveCurrentAs(String path) {
        SourceHandler newHandler = SourceManager.retrieveHandler(path);
        if (newHandler == null) {
            DataModule.logError(DataModule.class,
                "Cannot save to the path specified.  There are no " +
                "available handlers for the URL format or there is no data " +
                "at the specified location.");
            return;
        }

        try {
            SourceHandler oldHandler = activeSet().source().getHandler();
            if (oldHandler == newHandler)
                activeSet().store(path);

            else {
                DataSet d = newHandler.getSource(path).createNew(path);
                if (d != null) {
                    copyActiveTo(d);
                    d.store();
                }
            }
        }catch (TOOLException e) {
            DataModule.logError("Could not store DataSet in given location",
                e);
        }
    }

    protected void copyActiveTo(DataSet dest) {
        DataSet src = activeSet();
        if (src == null || dest == null || src == dest)
            return;

        int orig_limit = cache_limit;
        cache_limit /= 2;

        LinkedList<Frame> tmp_cache = new LinkedList<Frame>();

        for (Frame orig : src) {
            Frame copy = dest.add(true, true);
            if (!safeLoad(src, orig.index(), mainCache))
                break;

            copy.setPreview(orig.preview());
            copy.setImage(orig.image());
            copy.setJoints(orig.joints());
            copy.setSensors(orig.sensors());
            copy.setTags(orig.tags());
            //copy.setObjects(orig.objects());
            //copy.setRuns(orig.runs());
            copy.setStates(orig.states());

            if (!safeLoad(dest, copy.index(), tmp_cache))
                break;

            try {
                copy.store();
            }catch(TOOLException e) {
                DataModule.logError("Could not store new DataSet", e);
            }
        }

    }

    public boolean safeLoad(DataSet set, int i, LinkedList<Frame> cache) {
        if (i < 0 || i >= set.size() && !set.dynamic())
            // index is invalid
            return false;

        Frame f = set.get(i);

        if (f.loaded())
            // already loaded
            return true;

        else if (!cache.contains(f) && cache.size() >= cache_limit) {
            // need to remove an unused frame from the cache and free its
            // images for memory
            try {
                Frame old = cache.remove();

                if (old.changed()) {
                    if (set.autoSave())
                        old.store();
                    else {
                        DataModule.logError(DataModule.class,
                            "Action would result in data " +
                            "loss!  Please save your data, or enable " +
                            "auto-save.");
                        return false;
                    }
                }

                old.unload();
            }catch (TOOLException e) {
                DataModule.logError("Automatic data unloading failed with " +
                                    "error.", e);
                return false;
            }

        }

        // if already in cache, will remove it (it will be added to the back)
        cache.remove(f);

        try {
            f.load();
        }catch (TOOLException e) {
            DataModule.logError("Could not load frame.", e);
        }catch (OutOfMemoryError e) {
            DataModule.logError(DataModule.class,
                                "DataManager ran out of memory.  The size " +
                                "limit must be too high.", e);
        }

        if (f.loaded())
            cache.add(f);

        return f.loaded();
    }


    /**
     * Register a new ColorTableListener on this DataManager.
     * ColorTableListener will be notified of changes to the current color table
     */
    public void addColorTableListener(ColorTableListener d) {
        colorListeners.add(d);

    }

    public void ColorTableListener(DataListener d) {
        colorListeners.remove(d);
    }


    public void notifyColorTableDependants(ColorTable source,
                                           ColorTableUpdate update,
                                           ColorTableListener originator) {
        for (ColorTableListener d : colorListeners) {
            d.colorTableChanged(source, update, originator);
        }
    }


    /**
     * Register a new DataListener on this DataManager.  DataListener will be
     * notified of changes to the current active DataSet and Frame objects.
     *
     * @param d A class implementing the DataListener notification interface.
     */
    public void addDataListener(DataListener d) {
        listeners.add(d);

        if (activeSet() != null)
            notifyDependant(d);
    }

    public void removeDataListener(DataListener d) {
        listeners.remove(d);
    }

    /**
     * Notify all listeners on this DataManager of the current active Frame
     * object
     */
    public void notifyDependants(){
        if (activeSet() != null)
            notifyDependants(false);
    }

    /**
     * Notify all listeners on this DataManager of the current active Frame
     * object, and optionally the current Dataset as well.
     *
     * @param newSet True if the DataSet is new and listeners should be
     * notified of this first as well. False otherwise.
     */
    private void notifyDependants(boolean newSet) {
        for (DataListener d : listeners)
            notifyDependant(d, newSet);
    }

    private void notifyDependant(DataListener d) {
        notifyDependant(d, false);
    }

    private void notifyDependant(DataListener d, boolean newSet) {
        if (newSet) {
            d.notifyDataSet(activeSet(), activeFrame());
		}
        else {
            d.notifyFrame(activeFrame());
		}
    }

    //
    // ItemListener contract
    //

    public void itemStateChanged(ItemEvent e) {

        if (e.getStateChange() == ItemEvent.SELECTED &&
                e.getItem() instanceof DataSetObject) {

            DataSet d = ((DataSetObject)e.getItem()).getSet();
            //fireDataSetChanged(d);
            useDataSet(dataSets.indexOf(d));
        }
    }

    //
    // SourceListener contract
    //

    public void sourceChanged(DataSource source) {
        clear();

        addDataSets(source.getDataSets());
    }
}
