
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

package edu.bowdoin.robocup.TOOL.GUI;

import java.awt.BorderLayout;

import javax.swing.JLabel;

import edu.bowdoin.robocup.TOOL.Data.DataSource;

public class DataSourceObject extends ListObject {

    private DataSource source;

    public DataSourceObject(DataSource s) {
        super();

        source = s;

        updateInformation();
    }

    public void updateInformation() {
        removeAll();

        add(new JLabel(source.getPath()), BorderLayout.LINE_START);
        add(new JLabel(source.getType()), BorderLayout.LINE_END);
        add(new JLabel(source.numDataSets() + " DataSets"), BorderLayout.SOUTH);

        validate();
    }

    public DataSource getSource() {
        return source;
    }

    public String getText() {
        return source.getPath();
    }

}
