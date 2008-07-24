
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

package TOOL.Data.File;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Collections;
import java.util.Vector;

import TOOL.TOOLException;
import TOOL.Data.AbstractDataSet;
import TOOL.Data.DataSource;
import TOOL.Data.Frame;
import TOOL.Data.File.FileComparator;
import TOOL.Image.TOOLImage;

/**
 * Holds the contents of one set of Frame data points.  On the back end
 * a DataSet could be stored in FRM files, log files, a database, streamed from
 * a robot, etc.
 *
 * This is an an extension of the abtract DataSet implementation, specific for
 * loaded data from .FRM, .NSFRM or .raw image files.
 */
public class FileSet extends AbstractDataSet {

    private File fpath;
    private Vector<File> frms;

    protected FileSet(DataSource src, int i, String path) {
        super(src, i, path);
        fpath = new File(path);

        frms = new Vector<File>();

        loadFrmPaths();
    }

    public FileSet(DataSource src, int i, String path, String name) {
        super(src, i, path);
        fpath = new File(path);

        frms = new Vector<File>();
		
        loadFrmPaths();
        init(name, "FRM file set", frms.size());
    }

    private void loadFrmPaths() {
        if (!fpath.exists())
            fpath.mkdir();

        for (File f : fpath.listFiles(FrameLoader.FILTER))
            frms.add(f);

        Collections.sort(frms, FileComparator.NumericalOrder_F);
		
    }

    public Frame add(boolean l, boolean c) {
        Frame f = super.add(l, c);
        if (f != null)
            frms.add(new File(fpath, f.index() + ".FRM"));
        return f;
    }

    public void load(int i) throws TOOLException {
        if (loaded(i))
            return;

        File f = frms.get(i);
        Frame frm = get(i);

        // load frame from the specified file
        FrameLoader.loadFrame(f, frm);
        // mark that the frame is now loaded from the stored data
        frameLoaded.set(i, true);
        frameChanged.set(i, false); // and therefore is unmodified
    }

    public void store(int i, String p) throws TOOLException {
        if (!loaded(i) || !changed(i))
            return;

        Frame frm = get(i);

        // need to create file name here, for access to path and basename
        String baseName = i + FrameLoader.ROBOT_EXTS[frm.type()];
        File f = new File(p, baseName);

        // store frame in specified file
        FrameLoader.storeFrame(f, frm);
        // mark that the frame is no longer modifed form the stored data
        frameChanged.set(i, false);
    }
}
