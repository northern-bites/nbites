
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

package TOOL.Learning;

import java.awt.Component;

import TOOL.TOOL;
import TOOL.TOOLModule;

/** This class fits the learning system into the basic tool module
	structure.  The main thing here is that we get a DataManager
	so we can listen to when data sets are selected or new frames
	chosen.  The code was basically cut as paste from other modules.
	@author Eric Chown, 2009
 */

public class LearningModule extends TOOLModule {
    private Learning learn;

	/** Constructor creates the main Learning class and gets the
		data manager.
		@param tool    the parent class
	 */
    public LearningModule(TOOL tool) {
        super(tool);

        learn = new Learning(tool);
        tool.getDataManager().addDataListener(learn);

        /*
        classifier = new Learning(t);
        t.getDataManager().addDataListener(classifier);*/
    }

	/** Called to show which module we are
		@return the name of the module
	 */
    public String getDisplayName() {
        return "Learning";
    }

	/** Called when this module is selected.  Returns
		the main panel.
		@return the main panel
	 */
    public Component getDisplayComponent() {
        //return classifier;
        return learn.getContentPane();
    }

	/** Returns the main object.
		@return the learn object
	 */
	public Learning getLearning() {
		return learn;
	}

}
