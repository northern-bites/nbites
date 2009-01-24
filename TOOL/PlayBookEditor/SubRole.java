/**
 * Class to hold a PlayBook strategy
 */
package TOOL.PlayBookEditor;

import java.util.Vector;
import javax.swing.tree.*;

public class SubRole extends PBTreeNode
{
    // Our position on the field
    private RobotPosition myPos;
    private boolean ballRepulsor;
    /**
     * @param name The unique identifier for this strategy
     */
    public SubRole(String name)
    {
	super(name);
	ballRepulsor = true;
	myPos = null;
    }

    /**
     * @param position The RobotPosition to be associated with this SubRole
     */
    public void associatePosition(RobotPosition position)
    {
	myPos = position;
    }

    public RobotPosition getPosition() { return myPos;}

    // Ball repulsor junk
    public void setBallRepulsor(boolean use) {ballRepulsor = use;}
    public boolean getBallRepulsor() {return ballRepulsor;}
    /**
     * @return Returns null.
     */
    public Vector<PBTreeNode> getChildren()
    {
	return null;
    }

    public boolean hasChildren()
    {
	return false;
    }
    public boolean getAllowsChildren()
    {
	return false;
    }
    /**
     * @param child New Node to be added
     * @param index Ignored - only used for interface compatability
     */
    public void insert(MutableTreeNode child, int index){}

    /**
     * Method does nothing...
     * @param index Index of the object to be removed
     */
    public void remove(int index){}
    public void remove(MutableTreeNode nod){}

}