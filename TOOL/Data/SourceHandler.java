
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

package edu.bowdoin.robocup.TOOL.Data;

import edu.bowdoin.robocup.TOOL.Data.DataSource;

/**
 * A SourceHandler checks and opens resources stored in a location specified
 * by a string URL.  The handler handles a specifically crafted URL to refer to
 * the type of resource it is backed by (FRM files, Database, active robots,
 * etc.).  It can check a given URL for the existence of DataSets and build a
 * DataSource object to handle loading and retrieving DataSets.
 */
public interface SourceHandler {

    public boolean checkAvailable(String url);
    
    public DataSource getSource(String path);

}

