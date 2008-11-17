
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

package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import edu.bowdoin.robocup.TOOL.TOOL;
import java.util.Vector;
import java.util.StringTokenizer;
import java.awt.Point;
import java.io.*;

public class ComponentLibrary
{
    // Constants
    private static final boolean DEBUG = true;
    private final static String AIBO_PATH = "../../trunk/dog/pycode/Coop/";
    private final static String AIBO_CONSTS = AIBO_PATH + "CoopConstants.py";
    private final static String AIBO_SUBS = AIBO_PATH + "SubRoles.py";

    // Class variables
    private String playBookPath;

    // Publicly accesible fields
    public Vector<Strategy> strategies;
    public Vector<Formation> formations;
    public Vector<Role> roles;
    public Vector<SubRole> subRoles;


    /**
     * Construct a library of playBook Components
     * @param path PlayBook directory path
     */
    public ComponentLibrary(String path)
    {
	// Set our global path
	playBookPath = path;
	readSubRoleData();
	loadAllElements();
    }

    public void reloadSubRoles()
    {
	readSubRoleData();
	try {
	    loadInSubRoles();
	} catch (IOException e){
	    System.err.println(e);
	    return;
	}
    }

    private void readSubRoleData()
    {
	// Run the script to determine the SubRole infos
	String pyCommand ="./TOOL/PlayBookEditor/readIn/aiboReadIn/PBReadIn.py";
	Process pythonProc = null;
	try {
	    pythonProc = Runtime.getRuntime().exec(pyCommand);
	    pythonProc.waitFor();
	}
	catch (java.lang.InterruptedException f) {
	    System.err.println("Reading in of subRoles was aborted");
	    System.err.println(f.getMessage());
	} catch (IOException e){
	    System.err.println(e);
	    return;
	}
    }

    public void loadAllElements()
    {
	// Load in our components
	try {
	    loadInStrategies();
	    loadInFormations();
	    loadInRoles();
	    loadInSubRoles();
	} catch (IOException e){
	    System.err.println(e);
	    return;
	}
    }


    /**
     * We read the strategy file parsing the function names into strategy names
     */
    private void loadInStrategies() throws IOException
    {
	// Find our file and setup a buffer reader
	String sFile = playBookPath.concat("Strategies.py");
	BufferedReader dataIn;
	dataIn = new BufferedReader(new FileReader(sFile));

	// Read in the component names
	strategies =  new Vector<Strategy>();

	String line, t;
	StringTokenizer st;
	boolean singQComment = false, doubQComment = false;
	LINE:
	while( (line = dataIn.readLine()) != null) {
	    st = new StringTokenizer(line, " (", false);
	    while( st.hasMoreTokens() ) {
		t = st.nextToken();
		if (t.startsWith("'''") ||
		    t.endsWith("'''"))
		    singQComment = !singQComment;

		if (t.startsWith("\"\"\"") ||
		    t.endsWith("\"\"\""))
		    doubQComment = !doubQComment;

		if (t.startsWith("#")) {
		    continue LINE;
		}
		if( t.equals("def")) {
		    String newNameStrat = st.nextToken();
		    strategies.add(new Strategy(newNameStrat));
		    if (newNameStrat.startsWith("sTest")) {
			strategies.lastElement().setUneditable();
		    }
		    if (DEBUG) System.out.println("Strategy " + newNameStrat +
						  " added");
		}
	    }
	}
	dataIn.close();
    }

    private void loadInFormations() throws IOException
    {
	// Find our file and setup a buffer reader
	String fFile = playBookPath.concat("Formations.py");
	BufferedReader dataIn;
	dataIn = new BufferedReader(new FileReader(fFile));

	// Read in the component names
	formations = new Vector<Formation>();

	String line,token;
	StringTokenizer st;
	boolean singQComment = false, doubQComment = false;
	LINE:
	while( (line = dataIn.readLine()) != null) {
	    st = new StringTokenizer(line);
	    st = new StringTokenizer(line, " (", false);
	    while( st.hasMoreTokens() ) {
		token = st.nextToken();
		if (token.startsWith("'''") ||
		    token.endsWith("'''"))
		    singQComment = !singQComment;

		if (token.startsWith("\"\"\"") ||
		    token.endsWith("\"\"\""))
		    doubQComment = !doubQComment;

		if (token.startsWith("#")) {
		    continue LINE;
		}
		if( token.equals("def")) {
		    String newFormName = st.nextToken();
		    formations.add(new Formation(newFormName));
		    if (DEBUG) System.out.println("Formation " + newFormName +
						  " added");
		}
	    }
	}
	dataIn.close();
    }

    private void loadInRoles() throws IOException
    {
	// Find our file and setup a buffer reader
	String rFile = playBookPath.concat("Roles.py");
	BufferedReader dataIn;
	dataIn = new BufferedReader(new FileReader(rFile));
	// Read in the component names
	roles = new Vector<Role>();

	String line, token;
	StringTokenizer st;
	boolean singQComment = false, doubQComment = false;
	LINE:
	while( (line = dataIn.readLine()) != null) {
	    st = new StringTokenizer(line, " (", false);
	    while( st.hasMoreTokens() ) {
		token = st.nextToken();
		if (token.startsWith("'''") ||
		    token.endsWith("'''"))
		    singQComment = !singQComment;

		if (token.startsWith("\"\"\"") ||
		    token.endsWith("\"\"\""))
		    doubQComment = !doubQComment;

		if (token.startsWith("#")) {
		    continue LINE;
		}
		if( token.equals("def")) {
		    String newRoleName = st.nextToken();
		    roles.add(new Role(newRoleName));
		    if (DEBUG) System.out.println("Role " + newRoleName +
						  " added");
		}
	    }
	}
	dataIn.close();
    }

    private void loadInSubRoles() throws IOException
    {
	// Read the data into the java system
	String srdata = "./srData.txt";
	BufferedReader dataIn = new BufferedReader(new FileReader(srdata));

	subRoles = new Vector<SubRole>();

	String name;
	StringTokenizer infos;
	int xMin, xMax, yMin, yMax, xOff, yOff;
	boolean br;
	while ((name = dataIn.readLine()) != null) {
	    SubRole sb = new SubRole(name);
	    if (DEBUG) System.out.println("SubRole " + name + " added");

	    // Read in our information
	    infos = new StringTokenizer(dataIn.readLine());
	    xMin = (int)Float.parseFloat(infos.nextToken());
	    xMax = (int)Float.parseFloat(infos.nextToken());
	    yMin = (int)Float.parseFloat(infos.nextToken());
	    yMax = (int)Float.parseFloat(infos.nextToken());
	    xOff = (int)Float.parseFloat(infos.nextToken());
	    yOff = (int)Float.parseFloat(infos.nextToken());
	    if (Integer.parseInt(infos.nextToken()) == 0)
		sb.setBallRepulsor(false);
	    else
		sb.setBallRepulsor(true);

	    // Build the associated robot position
	    RobotPosition ropo;
	    if ((xMin - xMax) == 0) {
		if ((yMin - yMax) == 0) {
		    ropo = new RobotPosition(new Point(xMin, yMin));
		} else {
		    ropo = new RobotPosition(xMin, new Range(yMin, yMax));
		    ropo.setBallOffset(xOff, yOff);
		}
	    } else if ((yMin - yMax) == 0) {
		ropo = new RobotPosition(new Range(xMin, xMax), yMin);
		ropo.setBallOffset(xOff, yOff);
	    } else {
		ropo = new RobotPosition(new Range(xMin, xMax),
					 new Range(yMin, yMax));
		ropo.setBallOffset(xOff, yOff);
	    }
	    sb.associatePosition(ropo);
	    subRoles.add(sb);
	}
    }

    /**
     * Method to write a subRole to a file.  Currently assumes Aibo subRole.
     *
     * @param toWrite The subRole to be written out.
     */
    protected void writeOutSubRole(SubRole toWrite) throws IOException
    {
	// Setup the output file
	FileWriter dataOut = new FileWriter(AIBO_SUBS, true);

	char[] toPrefix = toWrite.toString().substring(1).toCharArray();
	StringBuffer cp = new StringBuffer();
	String constPrefix;
	for (int i = 0; i < toPrefix.length ; i++) {
	    if (Character.isLowerCase(toPrefix[i])) {
		cp = cp.append(Character.toUpperCase(toPrefix[i]));
	    } else {
		if (i != 0) {
		    cp = cp.append('_');
		}
		cp = cp.append(toPrefix[i]);
	    }
	}

	constPrefix = cp.toString();

	// Write the header information
	dataOut.write("\n# Method added automatically from the PBE\n" +
		      "@add_role\n" +
		      "def "+ toWrite.toString() +"(team):\n");
	// Deal with ball repulsor
	if( toWrite.getBallRepulsor()) {
	    dataOut.write("\tteam.moveBallRepulsor()\n");
	} else {
	    dataOut.write("\tteam.removeBallRepulsor()\n");
	}

	switch (toWrite.getPosition().getType()) {
	case RobotPosition.POINT_TYPE:
	    dataOut.write("\tx = CoopConstants." + constPrefix + "_X\n");
	    dataOut.write("\ty = CoopConstants." + constPrefix + "_Y\n");
	    break;
	case RobotPosition.X_LINE_TYPE:
	    dataOut.write("\tx = team.brain.clip(team.brain.ball.x + " +
			  "CoopConstants." + constPrefix + "_X_OFF," +
			  "CoopConstants." + constPrefix + "_X_MIN," +
			  "CoopConstants." + constPrefix + "_X_MAX)\n");
	    dataOut.write("\ty = CoopConstants." + constPrefix + "_Y\n");
	    break;
	case RobotPosition.Y_LINE_TYPE:
	    dataOut.write("\tx = CoopConstants." + constPrefix + "_X\n");
	    dataOut.write("\ty = team.brain.clip(team.brain.ball.y + " +
			  "CoopConstants." + constPrefix + "_Y_OFF," +
			  "CoopConstants." + constPrefix + "_Y_MIN," +
			  "CoopConstants." + constPrefix + "_Y_MAX)\n");
	    break;
	case RobotPosition.BOX_TYPE:
	    dataOut.write("\tx = team.brain.clip(team.brain.ball.x + " +
			  "CoopConstants." + constPrefix + "_X_OFF," +
			  "CoopConstants." + constPrefix + "_X_MIN," +
			  "CoopConstants." + constPrefix + "_X_MAX)\n");
	    dataOut.write("\ty = team.brain.clip(team.brain.ball.y + " +
			  "CoopConstants." + constPrefix + "_Y_OFF," +
			  "CoopConstants." + constPrefix + "_Y_MIN," +
			  "CoopConstants." + constPrefix + "_Y_MAX)\n");
	    break;
	}

	// Return our value
	dataOut.write("\treturn (x,y)\n");
	// Clean up our mess
	dataOut.close();

	// Write out the constants to our ConstantsFile
	dataOut = new FileWriter(AIBO_CONSTS, true);
	dataOut.write("\n# Following added automatically from the PBE\n");
	switch (toWrite.getPosition().getType()) {
	case RobotPosition.POINT_TYPE:
	    dataOut.write(constPrefix + "_X = " +
			  toWrite.getPosition().xMin + "\n");
	    dataOut.write(constPrefix + "_Y = " +
			  toWrite.getPosition().yMin + "\n");
	    break;
	case RobotPosition.X_LINE_TYPE:
	    dataOut.write(constPrefix + "_X_MIN = " +
			  toWrite.getPosition().xMin +"\n");
	    dataOut.write(constPrefix + "_X_MAX = " +
			  toWrite.getPosition().xMax +"\n");
	    dataOut.write(constPrefix + "_X_OFF = " +
			  toWrite.getPosition().getXOffset() +"\n");
	    dataOut.write(constPrefix + "_Y = " +
			  toWrite.getPosition().yMin +"\n");
	    break;
	case RobotPosition.Y_LINE_TYPE:
	    dataOut.write(constPrefix + "_X = " +
			  toWrite.getPosition().xMin + "\n");
	    dataOut.write(constPrefix + "_Y_MIN = " +
			  toWrite.getPosition().yMin +"\n");
	    dataOut.write(constPrefix + "_Y_MAX = " +
			  toWrite.getPosition().yMax +"\n");
	    dataOut.write(constPrefix + "_Y_OFF = " +
			  toWrite.getPosition().getYOffset() +"\n");
	    break;
	case RobotPosition.BOX_TYPE:
	    dataOut.write(constPrefix + "_X_MIN = " +
			  toWrite.getPosition().xMin +"\n");
	    dataOut.write(constPrefix + "_X_MAX = " +
			  toWrite.getPosition().xMax +"\n");
	    dataOut.write(constPrefix + "_X_OFF = " +
			  toWrite.getPosition().getXOffset() +"\n");
	    dataOut.write(constPrefix + "_Y_MIN = " +
			  toWrite.getPosition().yMin +"\n");
	    dataOut.write(constPrefix + "_Y_MAX = " +
			  toWrite.getPosition().yMax +"\n");
	    dataOut.write(constPrefix + "_Y_OFF = " +
			  toWrite.getPosition().getYOffset() +"\n");
	    break;
	}
	dataOut.close();
	System.out.println("Finished writing SubRole "+
			   toWrite.toString() + " to file.");
    }
}