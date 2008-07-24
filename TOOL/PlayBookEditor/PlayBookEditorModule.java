
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

package TOOL.PlayBookEditor;

import java.awt.Component;
import java.awt.event.KeyEvent;
import java.awt.KeyboardFocusManager;

import TOOL.TOOL;
import TOOL.TOOLModule;

public class PlayBookEditorModule extends TOOLModule {

    private PlayBookEditor editor;

    public PlayBookEditorModule(TOOL tool) {
        super(tool);
        editor = new PlayBookEditor(t);
        t.getFrame().addKeyListener(editor.getViewer());
        // listens for tabs
        KeyboardFocusManager.getCurrentKeyboardFocusManager().
            addKeyEventDispatcher(editor.getViewer());
    }

    public String getDisplayName() {
        return "PlayBook Editor";
    }

    public Component getDisplayComponent() {
        return editor;
    }


}
