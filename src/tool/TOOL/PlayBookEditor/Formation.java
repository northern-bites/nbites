/**
 * Class to hold a PlayBook strategy
 */
package TOOL.PlayBookEditor;

import java.util.Vector;
import javax.swing.tree.*;

public class Formation extends PBTreeNode
{
    private Vector<Role> children;
    private boolean nonPositionBased;

    /**
     * @param name The unique identifier for this strategy
     */
    public Formation(String name)
    {
	super(name);
	children = new Vector<Role>();
	nonPositionBased = false;
    }

    /**
     * @return A vector of the nodes children (Roles)
     */
    public Vector getChildren()
    {
	return children;
    }

    public void addRole(Role toAdd)
    {
	children.add(toAdd);
    }

    /**
     * @return Is the formation based around ball position and role allocation
     */
    public boolean isPositionBased()
    {
	return !nonPositionBased;
    }

    public boolean hasChildren()
    {
	return !children.isEmpty();
    }
    public boolean getAllowsChildren()
    {
	return true;
    }

    /**
     * @param child New Node to be added
     * @param index Location in the vector to store the child
     */
    public void insert(MutableTreeNode child, int index)
    {
	children.insertElementAt((Role)child, index);
    }

    /**
     * Method does nothing...
     * @param index Index of the object to be removed
     */
    public void remove(int index)
    {
	children.removeElementAt(index);
    }
    public void remove(MutableTreeNode node)
    {
	children.remove((Role)node);
    }
}