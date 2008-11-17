
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

package edu.bowdoin.robocup.TOOL.SQL;

import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.List;
import java.util.Vector;

import edu.bowdoin.robocup.TOOL.TOOLException;
import edu.bowdoin.robocup.TOOL.Data.DataSet;
import edu.bowdoin.robocup.TOOL.Data.DataSource;
import edu.bowdoin.robocup.TOOL.Data.SourceHandler;

public class SQLSource implements DataSource {

    private SourceHandler handler;
    private String db_path;
    private Vector<SQLSet> datasets;

    public SQLSource(SourceHandler hdlr, String path) {
        handler = hdlr;

        db_path = path;
        datasets = new Vector<SQLSet>();
    }

    //
    // DataSource contract
    //
    
    public SourceHandler getHandler() {
        return handler;
    }

    /**
     * This createNew() methods varies slightly from those of other
     * DataSources, as the String argument here is NOT an URL representation of
     * the location of the DataSet, but a SQL statement that performs the
     * initial SELECT operation for the DataSet.  This statement should be in
     * the form emitted by a FrameIDRequest (the toString() method, in the
     * SQLRequest base-class).
     */
    public DataSet createNew(String statement) {
        try {
            SQLSet set = new SQLSet(this, datasets.size(), db_path,
                                    statement);
            datasets.add(set);
            return set;
        }catch (TOOLException e) {
            SQLModule.logError("Unable to initialize SQL DataSet", e);
            return null;
        }
    }
    
    public int numDataSets() {
        return datasets.size();
    }

    public DataSet getDataSet(int i) {
        return datasets.get(i);
    }

    public List<DataSet> getDataSets() {
        return new Vector<DataSet>(datasets);
    }

    public String getPath() {
        String[] sections = db_path.split("/");
        String path = "";
        for (int i = 0; i < 3 && i < sections.length; i++)
            path += sections[i] + '/';
        return path;
    }

    public String getType() {
        return SQL_SOURCE_TYPE;
    }
}
