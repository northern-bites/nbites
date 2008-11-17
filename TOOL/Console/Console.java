
// This file is part of TOOL.
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

package edu.bowdoin.robocup.TOOL.Console;

import java.awt.FileDialog;
import java.io.File;
import javax.swing.JFileChooser;

import edu.bowdoin.robocup.TOOL.TOOL;

public class Console {

    public static final int OPEN = FileDialog.LOAD;
    public static final int SAVE = FileDialog.SAVE;
    public static final int FILE = JFileChooser.FILES_ONLY;
    public static final int DIRS = JFileChooser.DIRECTORIES_ONLY;
    public static final int BOTH = JFileChooser.FILES_AND_DIRECTORIES;

    private TOOL tool;

    public Console(TOOL t) {
        tool = t;
    }

    public void print(Object o) {
        System.out.print(o);
    }

    public void println(Object o) {
        System.out.println(o);
    }

    public void println() {
        System.out.println();
    }

    public void message(String msg) {
        System.out.println(msg);
    }

    public void error(String msg) {
        System.err.println(msg);
    }

    public void error(Throwable e) {
        e.printStackTrace();
    }

    public void error(Throwable e, String msg) {
        e.printStackTrace();
        System.err.println(msg);
    }

    public String promptOpen(String title, String curdir) {
        return openDialog(title, curdir, OPEN, BOTH);
    }

    public String promptSave(String title, String curdir) {
        return openDialog(title, curdir, SAVE, BOTH);
    }

    public String promptFileOpen(String title, String curdir) {
        return openDialog(title, curdir, OPEN, FILE);
    }

    public String promptFileSave(String title, String curdir) {
        return openDialog(title, curdir, SAVE, FILE);
    }

    public String promptDirOpen(String title, String curdir) {
        return openDialog(title, curdir, OPEN, DIRS);
    }

    public String promptDirSave(String title, String curdir) {
        return openDialog(title, curdir, SAVE, DIRS);
    }

    public String openDialog(String title, String curdir, int file_mode,
            int filter_mode) {

        if (System.getProperty("os.name").contains("Mac")) {
            FileDialog fd = new FileDialog(tool.getFrame(), title);
            if (curdir != null)
                fd.setDirectory(curdir);
            fd.setMode(file_mode);
            fd.setVisible(true);
            String fullpath = fd.getDirectory() + fd.getFile();
            fd.dispose();  

            if (fd.getFile() == null)
                return null;
            return new File(fd.getDirectory(), fd.getFile()).getPath();

        } else {
            JFileChooser chooser = new JFileChooser();
            if (curdir != null)
                chooser.setCurrentDirectory(new File(curdir));

            chooser.setDialogTitle(title);

            chooser.setFileSelectionMode(filter_mode);
            int retval;
            if (file_mode == OPEN)
                retval = chooser.showOpenDialog(tool.getFrame());
            else
                retval = chooser.showSaveDialog(tool.getFrame());

            if (retval == JFileChooser.APPROVE_OPTION)
                return chooser.getSelectedFile().getPath();
            return null;
        }
    }

}
