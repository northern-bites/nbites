
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

package TOOL.GUI;

import TOOL.TOOL;
import TOOL.TOOLModule;

import java.awt.Component;
import java.awt.GridLayout;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import javax.swing.*;
import javax.swing.JPanel;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;



public class MultiTabbedPane extends JPanel implements ChangeListener {

    private JPanel lonePanel; // if only one panel is desired
    private Vector<JSplitPane> panels;
    private Vector<TabPane> tabs;
    private Vector<String> names;
    private Map<String, Component> components;
    private Vector<Integer> selections;
    private TOOL tool;


    public MultiTabbedPane(TOOL t, Map<String, Component> map) {
        this(t, 2, map);
    }

    public MultiTabbedPane(TOOL t, int numPanels, Map<String, Component> map) {
        tool = t;

        // Initialize data structures
        panels = new Vector<JSplitPane>();
        tabs = new Vector<TabPane>();
        components = map;
        names = new Vector<String>(map.keySet());
        Collections.sort(names);
        selections = new Vector<Integer>();
        for (int i = 0; i < numPanels; i++)
            selections.add(i);

        setLayout(new GridLayout(1,1));

        updatePanels(numPanels);
        updateTabs();
        updateComponents();
    }
    
    public int getNumTabbedPanes() {
        return tabs.size();
    }


    public void addTab(int paneNumber, String componentName) {
        tabs.get(paneNumber).add(componentName, components.get(componentName));
        
        System.out.println(components.get(componentName));
        updateTabs();
        //updateComponents();
    }


    private void updatePanels(int numPanels) {

        // since 1 split pane holds 2 panels
        // use only 1 lone JPanel
        if (numPanels == 1) {
            removeAll();
            panels.clear();

            lonePanel = new JPanel();
            lonePanel.setLayout(new GridLayout(1,1));
            add(lonePanel);
            return;
        }else if (lonePanel != null) {
            lonePanel.removeAll();
            lonePanel = null;
        }
        numPanels--;

        int num = panels.size();

        if (num < numPanels) {
            for (int i = num; i < numPanels; i++) {
                JSplitPane p = new JSplitPane();
                if (i > 0)
                    p.setLeftComponent(panels.get(panels.size() - 1));

                p.setResizeWeight(.5);

                panels.add(p);
            }
        }else
            for (int i = panels.size() - 1; i >= numPanels; i--)
                panels.remove(i);

        for (int i = 0; i < panels.size(); i++)
            panels.get(i).setResizeWeight(1 - 1.0/(i+2));

        removeAll();
        add(panels.get(panels.size() - 1));
    }

    private void updateTabs() {

        int numPanels = panels.size() + 1;
        if (numPanels == 1) {
            TabPane t;
            if (tabs.size() > 0)
                t = tabs.get(0);
            else {
                t = new TabPane();
                t.setFocusable(false);
            }
            tabs.clear();

            tabs.add(t);
            lonePanel.removeAll();
            lonePanel.add(t);
            return;
        }

        if (tabs.size() < numPanels) {
            for (int i = 0; i < numPanels; i++) {
                if (i >= tabs.size())
                    tabs.add(new TabPane());
                TabPane t = tabs.get(i);
                t.setFocusable(false);

                JSplitPane p;
                if (i == 0) {
                    p = panels.get(0);
                    p.setLeftComponent(t);
                }else
                    panels.get(i - 1).setRightComponent(t);
            }
        }else
            for (int i = tabs.size() - 1; i >= numPanels; i++)
                tabs.remove(i);
    }

    private void updateComponents() {
        for (int i = 0; i < tabs.size(); i++) {
            TabPane t = tabs.get(i);
            // We don't want to get notified of the updates that occur as
            // we make our own changes
            t.removeChangeListener(this);
            
            // Remove the old components, as the mapping may have completely
            // changed
            t.removeAll();
            
            // Add an empty JPanel for each component
            for (String s : names)
                t.addTab(s, new JPanel(new GridLayout(1,1)));

            if (i >= names.size()) {
                // we don't have enough components to utilize this tabbed pane
                t.setEnabled(false);
                continue;
            }

            // change selection
            t.setSelectedIndex(selections.get(i));
            // add new component to different wrapping panel
            ((JPanel)t.getSelectedComponent()).add(
                    components.get(names.get(i)));

            // start listening to change events again
            t.addChangeListener(this);
        }
    }

    private int lowestUnselected() {
        // for each possible index, from lowest to highest
        for (int i = 0; i < names.size(); i++) {
            // check to see if index is selected
            int j;
            for (j = 0; j < selections.size(); j++)
                if (selections.get(j) == i)
                    break;
            // if no tabs have this index selected, return it
            if (j == selections.size())
                return i;
        }

        return -1;
    }

    public void performTabSelection(int tab, int component) {
        if (tab >= tabs.size() || component >= components.size())
            return;
        tabs.get(tab).setSelectedIndex(component);
    }

    public void performTabSelection(int tab, String name) {
        if (tab >= tabs.size() || !components.containsKey(name))
            return;
        tabs.get(tab).setSelectedIndex(names.indexOf(name));
    }

    /**
     * @param index which of the tabbed panes the tab is a part of
     */
    public String getSelectedTabName(int index) {
        // Which of the tabbed panes are we dealing with?
        TabPane t = tabs.get(index);
        return t.getTitleAt(t.getSelectedIndex());
    }


    public void handleTabSelection(int index) {
      
        TabPane t = tabs.get(index);

        int component = t.getSelectedIndex();
        int oldComponent = selections.get(index);
        JPanel p = (JPanel)t.getSelectedComponent();

        // remove any other selections of this component
        for (int i = 0; i < selections.size(); i++)
            if (selections.get(i) == component)
                selections.set(i, oldComponent);
        // set selection on this tab
        selections.set(index, component);
        // give new selection to any tab that has had it's selection usurped
        for (int i = 0; i < selections.size(); i++)
            if (selections.get(i) == -1)
                selections.set(i, lowestUnselected());

        // remove change listeners, so we don't get notified of our own changes
        for (int i = 0; i < selections.size(); i++)
            tabs.get(i).removeChangeListener(this);

        // update the references for the selected tab
        ((JPanel)t.getComponentAt(oldComponent)).removeAll();
        ((JPanel)t.getSelectedComponent()).add(
                components.get(names.get(component)));

        // remove all old references (to make sure no double reference)
        for (int i = 0; i < selections.size(); i++)
            if (tabs.get(i).getSelectedIndex() != selections.get(i))
                ((JPanel)tabs.get(i).getSelectedComponent()).removeAll();

        // update selections and new references
        for (int i = 0; i < selections.size(); i++) {
            if (tabs.get(i).getSelectedIndex() != selections.get(i)) {
                tabs.get(i).setSelectedIndex(selections.get(i));
                ((JPanel)tabs.get(i).getSelectedComponent()).add(
                    components.get(names.get(selections.get(i))));
            }
        }

        // add change listeners once more
        for (int i = 0; i < selections.size(); i++) {
            tabs.get(i).addChangeListener(this);
        }
    }

    public int numPanes() {
        return panels.size() + 1;
    }

    public void addPane() {
        selections.add(numPanes());
        updatePanels(numPanes() + 1);
        updateTabs();
        updateComponents();
    }

    public void removePane() { 
        if (numPanes() > 1) {
            selections.remove(selections.size() - 1);
            updatePanels(numPanes() - 1);
            updateTabs();
            updateComponents();
        }
    }

    //
    // ChangeListener contract
    //

    public void stateChanged(ChangeEvent e) {
        int paneIndex = tabs.indexOf(e.getSource());
        handleTabSelection(paneIndex);
        int tabIndex = tabs.get(paneIndex).getSelectedIndex();

        // alert the proper tool module of the selection of its tabs
        TOOLModule selected = tool.getModule(names.get(tabIndex));
        if (selected != null) {
            selected.stateChanged(e);
        }
    }


    public boolean canAddTab(int paneNumber) {
        return tabs.get(paneNumber).getTabCount() < components.size();
    }


    //http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=4465870
    private class TabPane extends JTabbedPane {
        public TabPane() {
            super();
        }


        protected void processMouseEvent(MouseEvent evt) {
            if (evt.getID() == MouseEvent.MOUSE_PRESSED
                && evt.getButton() == MouseEvent.BUTTON3)
                {
                    int index = indexAtLocation(evt.getX(), evt.getY());
                    if (index == -1) { return; }
                    getTabPopup(index).show(this, evt.getX(), evt.getY());
                }
            else
                {
                    super.processMouseEvent(evt);
                }
        }

        private JPopupMenu getTabPopup(final int index)
        {
            final String title = getTitleAt(index);

            JPopupMenu pm = new JPopupMenu();
            JMenuItem close = new JMenuItem("Remove " + title);
            close.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent evt)
                    {
                        TOOLModule selected = tool.getModule(title);
                        System.out.println(selected);
                        remove(index);
                    }
                });
            pm.add(close);

            return pm;
        }
    }

}

