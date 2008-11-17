
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

import java.util.List;

/**
 * A DataSource represents an opened resoure location that is capable of
 * providing data.  Could be a loaded log file, database connection, etc.. 
 * Serves up information about available DataSets, and can load preliminary or
 * full information in the background.
 *
 * @author Jeremy R. Fishman
 */
public interface DataSource {

    public static final String FILE_SOURCE_TYPE = "FRM Folder";
    public static final String LOG_SOURCE_TYPE = "Log Folder";
    public static final String ROBOT_SOURCE_TYPE = "Remote Robot";
    public static final String SQL_SOURCE_TYPE = "SQL Database";

    /**
     * Retrieve the SourceHandler instance that generated this DataSource.
     */
    public SourceHandler getHandler();

    /**
     * Create a new (empty) DataSet at this source location, according to the
     * URL path specified.  The URL must be appropriate for this set type and
     * must refer to a path 'beneath' the root DataSet location (i.e. leaves
     * further along the URL branch than the DataSet root URL).
     *
     * @param path The URL location of the new DataSet
     * @return the newly created DataSet at the given location, or null on
     * error.
     */
    public DataSet createNew(String path);

    /**
     * Retrieve the DataSet at the specified position in the list.
     *
     * @param i the index fo the DataSet to retrieve
     * @return the DataSet at index i, or null
     */
    public DataSet getDataSet(int i);

    /**
     * Retrive a list os all the DataSets in this source.
     *
     * @return a java.util.List of DataSet objects
     */
    public List<DataSet> getDataSets();

    /**
     * Return the number of available DataSets from this DataSource.
     *
     * @return the number of DataSets residing in this resource location
     */
    public int numDataSets();

    /**
     * Retrieve the String representation of this DataSource.
     *
     * @return the URL directing to this resource.
     */
    public String getPath();

    /**
     * Retrive the String representation of the type of this DataSource.
     *
     * @return the name of the underlying resource organization type
     */
    public String getType();

}

