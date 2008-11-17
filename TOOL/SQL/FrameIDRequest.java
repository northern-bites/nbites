
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

package edu.bowdoin.robocup.TOOL.SQL;

import java.util.Iterator;
import java.util.Vector;

import edu.bowdoin.robocup.TOOL.TOOL;
import edu.bowdoin.robocup.TOOL.Data.RobotDef;

public class FrameIDRequest extends SQLRequest {

    protected static final String TABLE = "Frames";
    protected static final String TBL_ALIAS = "f";
    protected static final String COLUMN = "frm_id";

    private boolean specificRobotType;
    private boolean locationNeeded;
    private boolean jointsNeeded;
    private boolean sensorsNeeded;
    private boolean objectsNeeded;
    private boolean runsNeeded;
    private boolean statesNeeded;
    private boolean tagsNeeded;

    private int robotType;
    private long locID;
    private Vector<Long> tagIDs;


    public FrameIDRequest() {
        // don't generate SQL until the following variables are set
        super(false);

        specificRobotType = false;
        locationNeeded = false;
        jointsNeeded = false;
        sensorsNeeded = false;
        objectsNeeded = false;
        runsNeeded = false;
        statesNeeded = false;
        tagsNeeded = false;

        robotType = RobotDef.AIBO_ERS7;
        locID = 0;
        tagIDs = new Vector<Long>();
        tagIDs.add(1L);

        regenerate();
    }

    protected void generateSelect(StringBuffer cmd) {
        cmd.append("SELECT `f`.`frm_id`, `r`.`robot_type`");
    }

    protected void generateFrom(StringBuffer cmd) {
        cmd.append("FROM `Frames` AS `f` LEFT JOIN (");
        cmd.append("`Robots` AS `r`");

        if (locationNeeded)
            cmd.append(", `Locations` AS `l`");
        if (jointsNeeded) {
            if (!specificRobotType || robotType == RobotDef.AIBO ||
                                      robotType == RobotDef.AIBO_ERS7 ||
                                      robotType == RobotDef.AIBO_220)
                cmd.append(", `AiboJoints` AS `aj`");

            if (!specificRobotType || robotType == RobotDef.NAO ||
                                      robotType == RobotDef.NAO_RL ||
                                      robotType == RobotDef.NAO_SIM)
                cmd.append(", `NaoJoints` AS `nj`");
        }
        if (sensorsNeeded)
            cmd.append(", `Sensors` AS `se`");
        if (objectsNeeded) 
            cmd.append(", `Frame_objs` AS `o`");
        if (runsNeeded)
            cmd.append(", `Runs` AS `run`");
        if (tagsNeeded)
            cmd.append(", `Frame_tags` AS `t`");
        if (statesNeeded)
            cmd.append(", `States` AS `st`");

        cmd.append(")");

        if (isPretty())
            cmd.append("\n  ");

        cmd.append("ON ( `f`.`robot_id`=`r`.`robot_id`");
        if (locationNeeded)
            cmd.append(" AND `f`.`loc_id`=`l`.`loc_id`");
        if (jointsNeeded) {
            if (!specificRobotType || robotType == RobotDef.AIBO ||
                                      robotType == RobotDef.AIBO_ERS7 ||
                                      robotType == RobotDef.AIBO_220)
                cmd.append(" AND `f`.`frm_id`=`aj`.`frm_id`");

            if (!specificRobotType || robotType == RobotDef.NAO ||
                                      robotType == RobotDef.NAO_RL ||
                                      robotType == RobotDef.NAO_SIM)
                cmd.append(" AND `f`.`frm_id`=`nj`.`frm_id`");
        }
        if (sensorsNeeded)
            cmd.append(" AND `f`.`frm_id`=`se`.`frm_id`");
        if (objectsNeeded) 
            cmd.append(" AND `f`.`frm_id`=`o`.`frm_id`");
        if (runsNeeded) {
        }
        if (tagsNeeded)
            cmd.append(" AND `f`.`frm_id`=`t`.`frm_id`");
        if (statesNeeded)
            cmd.append(" AND `f`.`frm_id`=`st`.`frm_id`");

        cmd.append(")");
    }

    protected void generateWhere(StringBuffer cmd) {
        cmd.append("WHERE 1");

        if (specificRobotType) {
            System.out.println("foo");
            if (robotType == RobotDef.AIBO)
                cmd.append(" AND `r`.`robot_type` IN ('" +
                    RobotDef.ROBOT_TYPES[RobotDef.AIBO_ERS7] + "','" +
                    RobotDef.ROBOT_TYPES[RobotDef.AIBO_220] + "')");
            else if (robotType == RobotDef.NAO)
                cmd.append(" AND `r`.`robot_type` IN ('" +
                    RobotDef.ROBOT_TYPES[RobotDef.NAO_RL] + "','" +
                    RobotDef.ROBOT_TYPES[RobotDef.NAO_SIM] + "')");
            else
                cmd.append(" AND `r`.`robot_type`='" +
                    RobotDef.ROBOT_TYPES[robotType] + "'");
        }

        if (tagsNeeded && !tagIDs.isEmpty()) {
            cmd.append(" AND `t`.`tag_id` IN (");
            for (Iterator<Long> itr = tagIDs.iterator(); itr.hasNext(); ) {
                cmd.append("'" + itr.next() + "'");
                if (itr.hasNext())
                    cmd.append(',');
            }
            cmd.append(')');
        }

    }

    protected void generateGroup(StringBuffer cmd) {
        cmd.append("GROUP BY `f`.`frm_id`");
    }

    protected void generateOrder(StringBuffer cmd) {
        cmd.append("ORDER BY `f`.`frm_id`");
    }
}


