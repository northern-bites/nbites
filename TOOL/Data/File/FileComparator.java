
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

import java.io.File;
import java.util.Comparator;


/**
 * Class which allows sorting of FRM files by different criteria.
 * Specifically, the one we'll probably be using most is NumericalOrder,
 * which simply sorts frames by their frame number rather than by their 
 * String sorted order (e.g. 1 2 3 instead of 1 10 11)
 * Specifically compares File objects, but they should be of type .FRM to use
 * with this.  Nothing to prevent it from being modified to accomodate the
 * Nao FRM files or whatever extension they may have.
 * @author Nicholas Dunn
 */
public class FileComparator {
    
    public static final Comparator<File> NumericalOrder_F =
        new Comparator<File>() {
            public int compare(File f1, File f2) {
                return FileComparator.compare(f1.getName(), f2.getName());
            }
        };
    public static final Comparator<String> NumericalOrder_S =
        new Comparator<String>() {
            public int compare(String s1, String s2) {
                return FileComparator.compare(s1, s2);
            }
        };

    public static int compare(String s1, String s2) {

        if (!FrameLoader.acceptableFormat(s1) ||
                !FrameLoader.acceptableFormat(s2))
            return s1.compareTo(s2);

        // The numerical part precedes the .FRM part
        String n1 = s1.substring(0, s1.indexOf("."));
        String n2 = s2.substring(0, s2.indexOf("."));
    
        int i, j;
        i = j = -1;
        try {
            i = Integer.parseInt(n1);
            j = Integer.parseInt(n2);
        }
        catch (NumberFormatException e) {
            return s1.compareTo(s2);
        }
                    
        return i - j;
    }
}

