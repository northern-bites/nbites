
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

package TOOL.SQL;

import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.ResultSet;
import java.util.Vector;


import TOOL.TOOL;
import TOOL.TOOLException;
import TOOL.Data.AbstractDataSet;
import TOOL.Data.DataSet;
import TOOL.Data.DataSource;
import TOOL.Data.Frame;

public class SQLSet extends AbstractDataSet {

    public static final String FRM_ID_COLUMN = "frm_id";
    public static final String ROBOT_TYPE_COLUMN = "robot_type";

    private String initial_statement;
    private Connection db_conn;
    private Vector<Long> frm_ids;
    private Vector<String> robot_types;

    public SQLSet(DataSource src, int i, String path, String statement)
            throws TOOLException {
        super(src, i, path);

        frm_ids = new Vector<Long>();
        robot_types = new Vector<String>();

        // record creating SQL statement
        initial_statement = statement;
        // preform SQL SELECT on data for frame ids (frm_id)
        performInitialSelect(initial_statement);
        // init the AbstractDataSet backend
        init("SQLSet " + i, statement, frm_ids.size());
    }

    private void ensureConnection() throws TOOLException {
        try {
            if (db_conn == null || db_conn.isClosed())
                db_conn = DriverManager.getConnection(path());
        }catch (SQLException e) {
            SQLModule.raiseError(SQLModule.class, e);
        }
    }
    
    private void close() {
        if (db_conn != null) {
            try {
                db_conn.close();
            }catch (SQLException e) {
                // ignore errors on close
            }
        }
    }

    private void performInitialSelect(String stmt) 
            throws TOOLException {

        // initiate connection to the database
        ensureConnection();

        Statement query = null;
        ResultSet rs = null;
        try {
            query = db_conn.createStatement();
        }catch (SQLException e) {
            // close database connection
            close();
            SQLModule.raiseError(SQLModule.class, e);
        }
        try{
            // Execute query, get results
            rs = query.executeQuery(stmt);
        }catch (SQLException e) {
            try {
                query.close();
            }catch (SQLException e2) {
                // ignore errors on close
            }
            close();
            SQLModule.raiseError(SQLModule.class, e);
        }

        try{
            if (rs.first()) {
                for ( ; !rs.isAfterLast(); rs.next()) {
                    long id = rs.getLong(FRM_ID_COLUMN);
                    String type = rs.getString(ROBOT_TYPE_COLUMN);
                    System.out.println("Frame " + id + ": " + type);
                    frm_ids.add(id);
                    robot_types.add(type);
                }
            }
        }catch (SQLException e) {
            try {
                query.close();
                rs.close();
            }catch (SQLException e2) {
                // ignore errors on close
            }
            close();
            SQLModule.raiseError(SQLModule.class, e);
        }
    }

    public void load(int i) throws TOOLException {
        ensureConnection();
    }

    public void store(int i, String path) throws TOOLException {
        ensureConnection();
    }

}

