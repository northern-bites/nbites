/**
 * Class to hold a PlayBook strategy
 */
package TOOL.PlayBookEditor;

import java.util.Vector;
import java.util.HashMap;
import javax.swing.tree.*;

public class Strategy extends PBTreeNode<Formation>
{
    private Vector<Formation> children;
    private HashMap<String, Zone> formationZones;

    /**
     * @param name The unique identifier for this strategy
     */
    public Strategy(String name)
    {
	super(name);
	children = new Vector<Formation>();
	formationZones = new HashMap<String, Zone>();
    }

    /**
     * @return A vector of the nodes children (Formations)
     */
    public Vector<Formation> getChildren()
    {
	return children;
    }

    public boolean hasChildren()
    {
	return !children.isEmpty();
    }

    /**
     * @param key The formation whose associated zone will be returned
     * @return The Zone associated with
     */
    public Zone getAssociatedZone(Formation key)
    {
	return formationZones.get(key.getID());
    }

    /**
     * Method to add a new formation to the strategy
     *
     * @param newFormation The formation to be added
     * @param newZone The zone to be associated with the formation
     */
    public void addFormation(Formation newFormation, Zone newZone)
    {
	children.add(newFormation);
	formationZones.put(newFormation.toString(), newZone);
    }
    public boolean getAllowsChildren()
    {
	return true;
    }
    /**
     * @param child New Node to be added
     * @param index Ignored - only used for interface compatability
     */
    public void insert(MutableTreeNode child, int index)
    {
	children.insertElementAt((Formation)child, index);
    }

    /**
     * @param index Index of the object to be removed
     */
    public void remove(int index)
    {
	formationZones.remove(children.get(index).toString());
	children.removeElementAt(index);
    }
    public void remove(MutableTreeNode node)
    {
	children.removeElement((Formation) node);
	formationZones.remove(((Formation)node).toString());
    }
}
