package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import java.util.Vector;
import javax.swing.tree.*;

/**
 * Class to hold a PlayBook strategy
 */
public class PlayBook extends PBTreeNode<Strategy>
{
    private Vector<Strategy> strategies;
    
    /**
     * @param name The unique identifier for this strategy
     */
    public PlayBook(String name)
    {
	super(name);
	strategies = new Vector<Strategy>();
    }

    // Getters
    /**
     * @return A vector of the nodes children (Strategies)
     */
    public Vector<Strategy> getChildren()
    {
	return strategies;
    }

    public void addStrategy(Strategy child)
    {
	strategies.add(child);
    }

    public boolean hasChildren()
    {
	return !strategies.isEmpty();
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
	strategies.insertElementAt((Strategy)child, index);
    }

    /**
     * Method does nothing...
     * @param index Index of the object to be removed
     */
    public void remove(int index)
    {
	strategies.removeElementAt(index);
    }

    public void remove(MutableTreeNode node)
    {
	strategies.removeElement((Strategy)node);
    }

}
