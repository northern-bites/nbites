
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

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.HashSet;
import java.util.Vector;
import javax.swing.JPanel;

import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Data.File.FileHandler;
import edu.bowdoin.robocup.TOOL.GUI.DataSourceObject;
import edu.bowdoin.robocup.TOOL.GUI.DataSourceOptionPanel;
import edu.bowdoin.robocup.TOOL.GUI.ListPanel;

/**
 * Add, remove, and process selection of sources of incoming data (resources).
 * <br>
 *
 * The SourceManager accepts URLs that direct to image file folders, log files,
 * databases, database frontends, and robots.  It spawns the correct
 * DataSource extension for the specified resource (by parsing URL and, if
 * neccessary, checking file data) and keeps track of loading and storing to
 * all open source locations.
 *
 * @author Jeremy R. Fishman
 */
public class SourceManager implements ItemListener {

    public static final int LIST_ROW_HEIGHT = 50;

    public static SourceHandler DEFAULT_NET_HANDLER = null;
    public static SourceHandler DEFAULT_SQL_HANDLER = null;
    public static SourceHandler DEFAULT_LOG_HANDLER = null;
    public static SourceHandler DEFAULT_FILE_HANDLER = new FileHandler();

    private Vector<DataSource> sources;

    private ListPanel<DataSourceObject> listPanel;
    private int dataSourceIndex;
    private HashSet<SourceListener> listeners;

    public SourceManager() {
        sources = new Vector<DataSource>();
        listeners = new HashSet<SourceListener>();

        dataSourceIndex = -1;

        listPanel = new ListPanel<DataSourceObject>();
        listPanel.setRowHeight(LIST_ROW_HEIGHT);
        listPanel.addItemListener(this);
    }

    /**
     * Retrieve the ListPanel that displays the cells for the available 
     * sources.
     *
     * @return the ListPanel object for display
     */
    public ListPanel getListPanel() {
        return listPanel;
    }

    /**
     * Retrieve the JPanel that contains components for setting and viewing
     * configurations for the selected DataSource.
     *
     * @return the JPanel object for display
     */
    public JPanel getOptionPanel() {
        if (activeSource() != null)
            return new DataSourceOptionPanel(this, activeSource());
        return null;
    }

    /**
     * Retrive the currently selected DataSource.
     *
     * @return the current DataSource, or null if none selected.
     */
    public DataSource activeSource() {
        if (dataSourceIndex < 0)
            return null;
        return getSource(dataSourceIndex);
    }

    /**
     * Retrieve the DataSource at the specified index in the list.
     *
     * @param i the index of the DataSource to retrieve
     * @return the DataSource at index i.
     */
    public DataSource getSource(int i) {
        return sources.get(i);
    }

    /**
     * Add a resource location to the list.  Will parse the URL, load the
     * correct manager, and add it to the list of available data sources.
     *
     * @param url The URL representing the location of a data resource, and
     * conforming to the expected formats.
     */
    public DataSource addSource(String url) {

        SourceHandler handler = retrieveHandler(url);
        if (handler == null) {
            TOOL.CONSOLE.error(
                "Source '" + url + "' not compatible with available handlers");
            return null;
        }

        DataSource source = handler.getSource(url);
        if (source == null) {
            TOOL.CONSOLE.error("Could not open source at '" + url + "'.");
            return null;
        }

        // add it to the list
        sources.addElement(source);
        // and add a new ListObject to the panel
        listPanel.add(new DataSourceObject(source));

        return source;
    }

    public static SourceHandler retrieveHandler(String url) {

        if (DEFAULT_NET_HANDLER != null &&
                DEFAULT_NET_HANDLER.checkAvailable(url))
            return DEFAULT_NET_HANDLER;

        else if (DEFAULT_SQL_HANDLER != null &&
                DEFAULT_SQL_HANDLER.checkAvailable(url))
            return DEFAULT_SQL_HANDLER;

        else if (DEFAULT_LOG_HANDLER != null &&
                 DEFAULT_LOG_HANDLER.checkAvailable(url))
            return DEFAULT_LOG_HANDLER;

        else if (DEFAULT_FILE_HANDLER != null &&
                 DEFAULT_FILE_HANDLER.checkAvailable(url))
            return DEFAULT_FILE_HANDLER;

        else
            return null;
    }

    /**
     * Remove the resource at index i from the list.  This will remove all
     * references to the corresponding DataSource, closing any open
     * connections.
     */
    public void remove(int i) {
        sources.remove(i);
    }

    /**
     * Select the specified DataSource for use.
     *
     * @param index the index of the DataSource to use
     */
    public void useDataSource(int index) {
        dataSourceIndex = index;
        fireSourceChanged(activeSource());
    }

    /**
     * Add a SourceListener to this SourceManager.
     *
     * @param l the SourceListener to receive future DataSource events.
     */
    public void addSourceListener(SourceListener l) {
        listeners.add(l);
    }

    /**
     * Remove a SourceListener from thie SourceManager.
     *
     * @param l the SourceListener to which to halt all future DataSource
     * events.
     */
    public void removeSourceListener(SourceListener l) {
        listeners.remove(l);
    }

    /**
     * Notify all SourceListeners on this manager of a change in active
     * DataSource.
     *
     * @param source the DataSource that is now the active source
     */
    private void fireSourceChanged(DataSource d) {
        for (SourceListener l : listeners)
            l.sourceChanged(d);
    }


    //
    // ItemListener contract
    //

    public void itemStateChanged(ItemEvent e) {
        if (e.getStateChange() == ItemEvent.SELECTED &&
            e.getItem() instanceof DataSourceObject) {

            DataSource d = ((DataSourceObject)e.getItem()).getSource();
            useDataSource(sources.indexOf(d));
        }
    }

}

