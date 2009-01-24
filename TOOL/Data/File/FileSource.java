
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

package TOOL.Data.File;

import java.io.File;
import java.util.Collections;
import java.util.List;
import java.util.HashMap;
import java.util.Vector;

import TOOL.TOOL;
import TOOL.Data.DataSet;
import TOOL.Data.DataSource;
import TOOL.Data.SourceHandler;

/**
 * Implementation of the DataSource interface on folders of .FRM files.
 * A DataSource represents an opened resoure location that is capable of
 * providing data.  Could be a loaded log file, database connection, etc.. 
 * Serves up information about available DataSets, and can load preliminary or
 * full information in the background.
 *
 * @author Jeremy R. Fishman
 */
public class FileSource implements DataSource {

    private SourceHandler handler;
    private Vector<String> setPaths;
    private HashMap<String, FileSet> sets;
    private String base_path;

    public FileSource(SourceHandler hdlr, String p) {
        base_path = p;

        setPaths = new Vector<String>();
        sets = new HashMap<String, FileSet>();

        addSets(base_path);
    }

    private void addSets(String path) {
        File f = new File(path);
        addSets(f, true);

        Collections.sort(setPaths);
    }

    private void addSets(File dir, boolean recursive) {
        if (dir.isDirectory()) {
            boolean current = false;

            // I/O error causes listFiles() to return null, as well as if 
            // abstract file name does not denote a directory (which we 
            // check for above)
            File[] files = dir.listFiles();
            if (files == null)
                return;

            for (File f : files) {

                // Check for frame files, if their add a DataSet for this
                // directory
                if (!current && FrameLoader.acceptableFormat(f)) {
		    current = true;
		    createNew(dir.getPath() +
			      System.getProperty("file.separator"));
                }
                // Recursively find files in sub-directories
                else if (f.isDirectory() && !f.getName().equals(".svn"))
                    addSets(f, true);
            }
        }
    }


    //
    // DataSource contract
    //

    /**
     * Retrieve the SourceHandler instance that generated this DataSource.
     */
    public SourceHandler getHandler() {
        return handler;
    }

    /**
     * Create a new (empty) DataSet at this source location, according to the
     * URL path specified.  The URL must be appropriate for this set type and
     * must refer to a path 'beneath' the root DataSet location (i.e. leaves
     * further along the URL branch than the DataSet root URL).
     *
     * @param url The URL path to the new DataSet
     * @return the newly created DataSet at the given location, or null on
     * error.
     */
    public DataSet createNew(String url) {
        if (!url.startsWith(base_path)) {
            TOOL.CONSOLE.error("Invalid url (" + url + ") for base path (" + 
                base_path + ")");
            return null;
        }

        File f = new File(url);
        String name = url.substring(base_path.length());
        FileSet set = new FileSet(this, sets.size(), url, name);
        setPaths.add(url);
        sets.put(url, set);

        return set;
    }

    /**
     * Retrieve the DataSet at the specified position in the list.
     *
     * @param i the index fo the DataSet to retrieve
     * @return the DataSet at index i, or null
     */
    public DataSet getDataSet(int i) {
        return sets.get(setPaths.get(i));
    }

    /**
     * Retrive a list os all the DataSets in this source.
     *
     * @return a java.util.List of DataSet objects
     */
    public List<DataSet> getDataSets() {
        Vector<DataSet> v = new Vector<DataSet>();
        for (String path : setPaths)
            v.add(sets.get(path));
        return v;
    }

    /**
     * Return the number of available DataSets from this DataSource.
     *
     * @return the number of DataSets residing in this resource location
     */
    public int numDataSets() {
        return sets.size();
    }

    /**
     * Retrieve the String representation of this DataSource.
     *
     * @return the URL directing to this resource.
     */
    public String getPath() {
        return base_path;
    }

    /**
     * Retrive the String representation of the type of this DataSource.
     *
     * @return the name of the underlying resource organization type
     */
    public String getType() {
        return DataSource.FILE_SOURCE_TYPE;
    }

}

