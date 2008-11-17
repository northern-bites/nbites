package edu.bowdoin.robocup.TOOL.PlayBookEditor;
import java.util.Vector;
import java.util.Enumeration;
import javax.swing.tree.*;

abstract public class PBTreeNode<T extends PBTreeNode>
    implements MutableTreeNode
{
    private String id;
    private boolean editable;

    /**
     * Main constructor
     *
     * @param name - Name of the treenNode
     */
    public PBTreeNode(String name)
    {
	id = name;
	editable = true;
    }

    /**
     * Get a vector containing all of the nodes children
     * @return A vector with all of the nodes direct children
     */
    abstract Vector<T> getChildren();

    /**
     * @return True if the node has children
     */
    abstract boolean hasChildren();

    /**
     * Method to get the node's identifier
     * @return The Nodes id as specified when created
     */
    public String getID()
    {
	return id;
    }

    public String toString()
    {
	return getID();
    }

    /**
     * @return True if the component is editable
     */
    public boolean isEditable()
    {
	return editable;
    }

    // Setters
    /**
     * Edits the 
     */
    protected void setUneditable()
    {
	editable = false;
    }
    protected void setEditable()
    {
	editable = true;
    }

    // TreeNode interface methods
    public Enumeration children()
    {
	return getChildren().elements();
    }

    public  TreeNode getChildAt(int childIndex)
    {
	return getChildren().elementAt(childIndex);
    }
    public int getChildCount()
    {
	return getChildren().size();
    }
    public int getIndex(TreeNode node)
    {
	return getChildren().indexOf(node);
    }

    public boolean isLeaf()
    {
	return !hasChildren();
    }

    public abstract boolean getAllowsChildren();
    public abstract void insert(MutableTreeNode child, int index);
    public abstract void remove(int index);
    public abstract void remove(MutableTreeNode nod);

    // Not sure what this method is actualyl supposed to do...
    public void setUserObject(Object object){}

    // We Don't do parents, because we want to have resusable sub graphs...
    public TreeNode getParent()
    {
	return null;
    }
    public void setParent(MutableTreeNode newParent){}
    public void removeFromParent(){}


}
    
