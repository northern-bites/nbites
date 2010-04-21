
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

package TOOL.ColorEdit;

import java.awt.Component;

import TOOL.TOOL;
import TOOL.TOOLModule;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;


public class ColorEditModule extends TOOLModule {

    private ColorEdit colorEdit;

    public ColorEditModule(TOOL tool) {
        super(tool);

        colorEdit = new ColorEdit(t);

        t.getDataManager().addDataListener(colorEdit);
        // Adds color edit panel as a key listener
        t.getFrame().addKeyListener(colorEdit.getColorEditPanel());
    }

    public String getDisplayName() {
        return "ColorEdit";
    }

    public Component getDisplayComponent() {
        return colorEdit;
    }

    public void stateChanged(ChangeEvent e) { 
        colorEdit.refresh(); 
    }
    
}
