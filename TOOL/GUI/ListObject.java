      
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
import java.awt.Color;
import java.awt.Component;
import java.awt.LayoutManager;
import java.awt.datatransfer.DataFlavor;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.TransferHandler;
import javax.swing.border.Border;

public class ListObject extends JPanel {

    public static final Color SELECTED_BACKGROUND = new Color(180, 180, 230);
    public static final Color UNSELECTED_BACKGROUND = Color.white;
    public static final LayoutManager DEFAULT_LAYOUT = new BorderLayout(5, 5);
    public static final Border DEFAULT_BORDER = 
        BorderFactory.createEmptyBorder(0, 5, 0, 5);


    public static final DataFlavor FLAVOR = new DataFlavor(ListObject.class,
            "ListObject");

    private String text;
    private boolean selected;

    public ListObject() {
        this("Plain List Object");
    }

    public ListObject(String label) {
        super();

        text = label;

        setTransferHandler(new TransferHandler(""));
        setBackground(UNSELECTED_BACKGROUND);
        setLayout(DEFAULT_LAYOUT);
        setBorder(DEFAULT_BORDER);

        add(new JLabel(text), BorderLayout.LINE_START);
    }


    public void updateInformation() {
        removeAll();

        add(new JLabel(text), BorderLayout.LINE_START);
        validate();
    }

    public void validate() {
        for (Component c : getComponents())
            c.invalidate();
        super.validate();
    }

    public boolean isSelected() {
        return selected;
    }

    public void setSelected(boolean amSelected) {
        selected = amSelected;
        if (selected)
            setBackground(SELECTED_BACKGROUND);
        else
            setBackground(UNSELECTED_BACKGROUND);
    }

    public String getText() {
        return text;
    }

}
