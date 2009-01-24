
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

package TOOL.PlayBookEditor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import TOOL.TOOL;
import java.io.*;
import java.util.Vector;
import java.util.Enumeration;

/**
 * Class to hold the behavior tree information for the playbook editor
 */
public class BehaviorTree extends JPanel implements ActionListener,
						    MouseListener
{
    // Constants
    protected static final int DEFAULT_ROWS = 30;
    protected static final int DEFAULT_COLUMNS = 20;
    protected static final String AIBO_TREE_TYPE = "AIBO_TREE";
    protected static final String NAO_TREE_TYPE = "NAO_TREE";
    protected static final String PBTREE_ID = "PlayBook Tree";
    protected static final String STRAT_ID = "Strategies";
    protected static final String FORM_ID = "Formations";
    protected static final String ROLE_ID = "Roles";
    protected static final String SUBROLE_ID = "SubRoles";

    // Declare our panel components
    protected JLabel title;
    protected JScrollPane treeScroller;
    protected String myTreeType, treeMode;
    protected String treeString;
    protected JTree treeView;
    protected JList stratView, formView, roleView,subRoleView;
    protected Vector<PBTreeNode> treeNodes;
    protected JComboBox displaySelector;
    protected Vector<String> selectorList;
    protected PlayBookEditor editor;

    /**
     * Main constructor
     *
     * @param treeType  A String to specify the type of tree.
     *        Protected fields of AIBO_TREE and NAO_TREE specify which one.
     */
    public BehaviorTree(String treeType)
    {
        super();
	myTreeType = treeType;
	setupWindowAndListeners();
   }

    /**
     * Default constructor builds a tree of type AIBO_TREE_TYPE
     */
    public BehaviorTree()
    {
    	this(AIBO_TREE_TYPE);
    }

    private void setupWindowAndListeners()
    {
	// Initialize Objects
	title = new JLabel("Coordinated Behavior Tree", JLabel.CENTER);
	treeScroller = new JScrollPane();
	treeNodes = new Vector<PBTreeNode>();
	treeView = new JTree(treeNodes);
	stratView = new JList();
	formView = new JList();
	roleView = new JList();
	subRoleView = new JList();

	// Setup our display selection drop down list
	selectorList = new Vector<String>();
	selectorList.add(PBTREE_ID);
	selectorList.add(STRAT_ID);
	selectorList.add(FORM_ID);
	selectorList.add(ROLE_ID);
	selectorList.add(SUBROLE_ID);
	displaySelector = new JComboBox(selectorList);
	displaySelector.addActionListener(this);
	treeScroller.getViewport().setView(treeView);
	treeMode = PBTREE_ID;
        treeScroller.setFocusable(false);
	treeScroller.addMouseListener(this);
	subRoleView.addMouseListener(this);

	// Setup Layout
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
	setLayout(gridbag);

	// Add Items
	c.fill = GridBagConstraints.BOTH;
	c.gridwidth = GridBagConstraints.REMAINDER;
	gridbag.setConstraints(title,c);
	c.gridheight = 2;
	add(title);

	gridbag.setConstraints(displaySelector, c);
	add(displaySelector);

	c.gridheight = GridBagConstraints.REMAINDER;
	gridbag.setConstraints(treeScroller, c);
	add(treeScroller);
    }

    /**
     * Method loads a PlayBook and displays its members to the screen
     * @param book The PlayBook to be loaded in
     */
    protected void loadPlayBook(PlayBookEditor editor) throws IOException
    {
	this.editor = editor;
	ComponentLibrary myCompLib = editor.getComponentLibrary();
	// We setup the various display units for the play book components
	treeView = new JTree(editor.getPlayBook());
	stratView = new JList(myCompLib.strategies);
	formView = new JList(myCompLib.formations);
	roleView = new JList(myCompLib.roles);
	subRoleView = new JList(myCompLib.subRoles);

	// Set default display to tree
	treeScroller.getViewport().setView(treeView);
    }

    public void actionPerformed(ActionEvent e)
    {
	String item = (String) displaySelector.getSelectedItem();
	treeMode = item;
	// We want to respond to the changing of the combobox selectio
	if (item == PBTREE_ID) {
	    // Display the tree
	    treeScroller.getViewport().setView(treeView);
	} else if (item == STRAT_ID) {
	    // Display strategies
	    treeScroller.getViewport().setView(stratView);
	} else if (item == FORM_ID) {
	    // Display formations
	    treeScroller.getViewport().setView(formView);
	} else if (item == ROLE_ID) {
	    // Display roles
	    treeScroller.getViewport().setView(roleView);
	} else if (item == SUBROLE_ID) {
	    // Display SubRoles
	    treeScroller.getViewport().setView(subRoleView);
	}

    }

    public void mouseClicked(MouseEvent e)
    {
	// Double Click
	if(e.getClickCount() == 2) {
	    System.out.println("Double CLIKC!!\n\n\n");
	    if (treeMode == SUBROLE_ID) {
		if(editor.getMode() ==
		   PBModePanel.SUB_ROLE_MODE){
		    editor.getSubRolePanel().loadSelected();
		} else if (editor.getMode() ==
			   PBModePanel.TEST_MODE) {
		    editor.getTestPanel().selectSubRole();
		}
	    }

	// Single Click
	} else if (e.getClickCount() == 1) {
	    return;
	}
    }
    public void mousePressed(MouseEvent e){}
    public void mouseReleased(MouseEvent e){}
    public void mouseExited(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}

    public JScrollPane getScroll() { return treeScroller;}
}
