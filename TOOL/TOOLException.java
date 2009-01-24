
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

package TOOL;

public class TOOLException extends Exception {

    private TOOLModule module;

    public TOOLException(TOOLModule m) {
        super();
        module = m;
    }

    public TOOLException(TOOLModule m, String message) {
        super(message);
        module = m;
    }

    public TOOLException(TOOLModule m, Throwable cause) {
        super(cause);
        module = m;
    }

    public TOOLException(TOOLModule m, String message, Throwable cause) {
        super(message, cause);
        module = m;
    }

    public TOOLModule getModule() {
        return module;
    }

    public String toString() {
        String s = module.getDisplayName() + "::TOOLException";
        if (getMessage() == null)
            s += ": " + getMessage();
        return s;
    }

}
