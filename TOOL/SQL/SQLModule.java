
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

import java.awt.GridLayout;
import java.awt.event.ActionEvent;

import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.List;
import java.util.Vector;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTextArea;

import TOOL.TOOL;
import TOOL.TOOLModule;
import TOOL.Data.DataSet;
import TOOL.Data.DataSource;
import TOOL.Data.SourceHandler;
import TOOL.GUI.ListPanel;
import TOOL.GUI.DataSourceObject;
import TOOL.GUI.DataSetObject;

public class SQLModule extends TOOLModule implements SourceHandler {

    private static final String JDBC_BASE = "jdbc:mysql:";
    private static final String DB_ADDR = "localhost";
                                        // port = default = 3306
    private static final String DB_USER = "tool_admin";
    private static final String DB_PASS = "9yW56B:J9pCAWKdt";
    private static final String DB_NAME = "HiveMind";
    private static final String DEFAULT_DB_PATH = 
        createSourceURL(DB_ADDR, DB_NAME, DB_USER, DB_PASS);

    private JPanel display;

    private Vector<SQLSource> sources;
    private ListPanel<DataSourceObject> list;
    private JTextArea textArea;

    public SQLModule(TOOL t) {
	super(t);

        sources = new Vector<SQLSource>();

        initDisplay();

        try {
            // The newInstance() call is a work around for some
            // broken Java implementations

            Class.forName("com.mysql.jdbc.Driver").newInstance();
        } catch (Exception ex) {
            // handle the error
            TOOL.CONSOLE.error(ex, "Unable to load MySQL Connector/J JDBC driver.");
        }

        t.getSourceManager().DEFAULT_SQL_HANDLER = this;
        t.getSourceManager().addSource(createSourceURL(DB_ADDR, DB_NAME,
            DB_USER, DB_PASS));
    }

    public String getDisplayName() {
	return "SQL";
    }

    public JComponent getDisplayComponent() {
	return display;
    }

    private void initDisplay() {

	display = new JPanel();
        display.setLayout(new GridLayout(1, 2));

        list = new ListPanel<DataSourceObject>();
        display.add(list);

        JPanel subPanel = new JPanel();
        subPanel.setLayout(new BoxLayout(subPanel, BoxLayout.LINE_AXIS));
        
        JButton button = new JButton("Query");
        button.setActionCommand("query");
        button.addActionListener(this);
        subPanel.add(button);

        textArea = new JTextArea();
        subPanel.add(textArea);
        display.add(subPanel);

    }

    //
    // ActionListener contract
    //

    public void actionPerformed(ActionEvent e) {
        if (e.getActionCommand().equals("query")) {
            //String sql_query = textArea.getText();
            FrameIDRequest r = new FrameIDRequest();
            System.out.println(r.prettyString());

            DataSourceObject list_object = list.getSelected();
            if (list_object != null) {
                DataSource source = list_object.getSource();
                DataSet set = source.createNew(r.toString());

                if (set != null)
                    System.out.println(set.size());
            }
        }
    }

    // 
    // SourceHandler contract
    //

    public boolean checkAvailable(String url) {
        try {
            if (url.startsWith(JDBC_BASE)) {
                DriverManager.getConnection(url).close();
                return true;
            }
        }catch (SQLException e) {
            // ignore errors - they will show up as "no available handler"
        }

        return false;
    }

    public DataSource getSource(String url) {
        if (url.equals(JDBC_BASE))
            url = DEFAULT_DB_PATH;
    
        SQLSource source = new SQLSource(this, url);
        sources.add(source);
        list.add(new DataSourceObject(source));
        return source;
    }

    //
    // Static methods
    //

    public static String createSourceURL(String address, String database,
            String user, String passwd) {
        return JDBC_BASE + "//" + address + "/" + database + "?user=" + user +
            "&password=" + passwd;
    }

}
