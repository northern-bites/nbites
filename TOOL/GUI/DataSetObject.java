
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

package TOOL.GUI;

import java.awt.BorderLayout;

import javax.swing.JLabel;

import TOOL.Data.DataSet;

public class DataSetObject extends ListObject {

    private DataSet set;

    public DataSetObject(DataSet s) {
        super();

        set = s;

        updateInformation();
    }

    public void updateInformation() {
        removeAll();
    
        add(new JLabel(set.name()), BorderLayout.LINE_START);
        add(new JLabel(set.desc()), BorderLayout.LINE_END);
        if (set.size() == 1)
            add(new JLabel(set.size() + " image"), BorderLayout.SOUTH);
        else
            add(new JLabel(set.size() + " images"), BorderLayout.SOUTH);

        validate();
    }

    public DataSet getSet() {
        return set;
    }

    public String getText() {
        return set.name();
    }

}
