/**
 * Class to hold a PlayBook strategy
 */
package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import java.util.Vector;
import java.util.HashMap;
import javax.swing.tree.*;

public class Role extends PBTreeNode<SubRole>
{
    private Vector<SubRole> children;
    private HashMap<String, Zone> subRoleZones;
    /**
     * @param name The unique identifier for this strategy
     */
    public Role(String name)
    {
	super(name);
	children = new Vector<SubRole>();
	subRoleZones = new HashMap<String, Zone>();
    }

    /**
     * @return A vector of the nodes children (SubRoles)
     */
    public Vector<SubRole> getChildren()
    {
	return children;
    }

    public void addSubRole(SubRole toAdd, Zone newZone)
    {
	children.add(toAdd);
	subRoleZones.put(toAdd.toString(), newZone);
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
     * Adds a SubRole to the specified positoin
     * Note you must explicitlly associate a zone with this subRole, if one
     * is not already specified
     *
     * @param child New Node to be added
     * @param index Index of where the subRole should be added
     */
    public void insert(MutableTreeNode child, int index)
    {
	children.insertElementAt((SubRole)child, index);
    }

    /**
     * Method does nothing...
     * @param index Index of the object to be removed
     */
    public void remove(int index)
    {
	subRoleZones.remove(children.get(index).toString());
	children.removeElementAt(index);
    }
    public void remove(MutableTreeNode node)
    {
	children.removeElement((SubRole)node);
	subRoleZones.remove(((SubRole)node).toString());
    }
}
