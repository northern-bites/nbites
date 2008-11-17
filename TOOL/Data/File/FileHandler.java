
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

package edu.bowdoin.robocup.TOOL.Data.File;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;

import edu.bowdoin.robocup.TOOL.TOOLException;
import edu.bowdoin.robocup.TOOL.Data.DataModule;
import edu.bowdoin.robocup.TOOL.Data.DataSource;
import edu.bowdoin.robocup.TOOL.Data.SourceHandler;

/**
 * The FileHandler checks and opens resources stored on the system in a
 * series of .FRM and other files in folders.  The handler can check a given
 * URL for the existence of such files and build a DataSource object to handle
 * loading and retrieving DataSets.
 */
public class FileHandler implements SourceHandler {

    public boolean checkAvailable(String url) {
        File f = new File(url);
        if (f.isDirectory())
            return true;

        else if (!f.isFile()) {
            try {
                return f.createNewFile() && f.delete();
            }catch (IOException e) {
                DataModule.logError(DataModule.class, e);
                return false;
            }

        }else
            return false;
    
    /*
        File f = new File(url);

        if (!f.isDirectory())
            return false;

        File[] files = f.listFiles(FrameLoader.FILTER);
        if (files.length > 0)
            return true;
        return false;
    */
    }

    public boolean recurseAvailable(String url) {
        return recurseAvailable(new File(url));
    }

    public boolean recurseAvailable(File dir) {
        if (dir == null) { return false; }

        if (!dir.isDirectory())
            return false;

        // I/O error causes listFiles() to return null, as well as if abstract
        // file name does not denote a directory (which we check for above)
        if (dir.listFiles() == null) {
            return false;
        }

        for (File f : dir.listFiles()) {
            if (f.isDirectory()) {
                if (recurseAvailable(f))
                    return true;
            }else if (FrameLoader.acceptableFormat(f))
                return true;
        }
        return false;
    }

    public DataSource getSource(String path) {
        if (checkAvailable(path)) {
	    final String fileSeparator = System.getProperty("file.separator");
	    if (!path.endsWith(fileSeparator))
		path +=  fileSeparator;
            return new FileSource(this, path);

        }else
            return null;
    }

}

