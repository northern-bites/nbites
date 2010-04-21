
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

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.ItemSelectable;
import java.awt.LayoutManager;
import java.awt.Rectangle;

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.Scrollable;
import javax.swing.border.Border;

import TOOL.TOOL;

public class ListPanel<T extends ListObject>
        extends JScrollPane implements ItemSelectable,
                                       MouseListener,
                                       MouseMotionListener {

    public static final int DEFAULT_ROW_HEIGHT = 45;
    public static final int DEFAULT_ROW_SPACING = 2;
    public static final Border DEFAULT_BORDER =
        BorderFactory.createEmptyBorder(0, 0, DEFAULT_ROW_SPACING, 0);

    public static final int SINGLE = 0;
    public static final int MULTIPLE = 1;

    private ListInnerPanel<T> innerPanel;
    private Vector<T> objects;
    private GridLayout gridLayout;
    private LayoutManager layout;
    
    private int rowHeight;
    private int rowSpacing;

    private int selectionType;
    private int selectedIndex;
    private boolean dragging;
    private HashSet<ItemListener> listeners;

    public ListPanel() {
        this(SINGLE);
    }

    public ListPanel(int type) {
        this(new ListInnerPanel<T>(), type);
    }

    private ListPanel(ListInnerPanel<T> thePanel, int type) {
        super(thePanel, VERTICAL_SCROLLBAR_AS_NEEDED,
            HORIZONTAL_SCROLLBAR_AS_NEEDED);


        selectionType = type;

        innerPanel = thePanel;
        innerPanel.setParent(this);

        objects = new Vector<T>();

        selectedIndex = -1;
        dragging = false;
        listeners = new HashSet<ItemListener>();
        
        rowHeight = DEFAULT_ROW_HEIGHT;
        rowSpacing = DEFAULT_ROW_SPACING;

        //gridLayout = new GridLayout(1, 1, 0, rowSpacing);
        //innerPanel.setLayout(gridLayout);
        innerPanel.setBackground(Color.gray);
        innerPanel.setBorder(DEFAULT_BORDER);
        innerPanel.addMouseListener(this);
        innerPanel.addMouseMotionListener(this);
        //outerPanel.setBackground(Color.lightGray);
        //setBackground(Color.lightGray);
    }

    public void add(T o) {
        objects.add(o);

        innerPanel.add(o);
        innerPanel.setBounds(innerPanel.getBounds());
        innerPanel.validate();
    }

    public void addAll(List<T> objs) {
        objects.addAll(objs);

        Iterator<T> itr = objs.iterator();
        while (itr.hasNext())
            innerPanel.add(itr.next());

        innerPanel.setBounds(innerPanel.getBounds());
        innerPanel.validate();
    }

    public T get(int i) {
        return objects.get(i);
    }

    public int getSelectedIndex() {
        return selectedIndex;
    }

    public T getSelected() {
        if (selectedIndex >= 0 && selectedIndex < objects.size())
            return objects.get(selectedIndex);
        return null;
    }

    public void remove(T o) {
        int i = objects.indexOf(o);
        /*
        if (selectionType == SINGLE) {
            if (i == selectedIndex)
                fireSelectionChange(selectedIndex, -1);
            else if (i < selectedIndex)
                selectedIndex--;
        }else {
            if (objects.get(i).isSelected())
                fireSelectionChange(selectedIndex);
            indexSelections.remove(i);
        }
        */

        objects.remove(o);

        innerPanel.remove(o);
        innerPanel.setBounds(innerPanel.getBounds());
        innerPanel.validate();
    }

    public void clear() {
        /*
        if (selectionType == SINGLE)
            fireSelectionChange(selectedIndex, -1);
        else {
            for (int i = 0; i < indexSelections.size(); i++)
                if (indexSelections.get(i))
                    fireSelectionChange(i);
        }
        */

        for (T o : objects)
            innerPanel.remove(o);

        objects.clear();
        innerPanel.setBounds(innerPanel.getBounds());
        innerPanel.validate();
    }

    public void select(int i) {
        if (i > objects.size())
            select(-1);

        if (selectionType == SINGLE) {
            if (i == selectedIndex)
                return;
            else
                fireSelectionChange(selectedIndex, i);
        }else
            fireSelectionChange(i);
    }

    public int length() {
        return objects.size();
    }

    private void updateScrollbars() {
        Dimension outer = getViewport().getExtentSize();
        Dimension inner = innerPanel.getSize();

        if (inner.height > outer.height) {
            setVerticalScrollBarPolicy(VERTICAL_SCROLLBAR_ALWAYS);
        }else if (inner.width > outer.width) {
            setVerticalScrollBarPolicy(VERTICAL_SCROLLBAR_NEVER);
        }else {
            setVerticalScrollBarPolicy(VERTICAL_SCROLLBAR_NEVER);
        }
    }

    public int calculateHeight() {
        return Math.max(
            (objects.size() - 1) * (rowHeight + rowSpacing) + rowHeight,
            0);
    }

    public int getRowHeight() {
	return rowHeight;
    }
    
    public void setRowHeight(int height) {
	rowHeight = height;
    }

    public int getRowSpacing() {
	return rowSpacing;
    }

    public void setRowSpacing(int spacing) {
	rowSpacing = spacing;
	gridLayout.setVgap(rowSpacing);
    }

    public int getCombinedHeight() {
        return rowHeight + rowSpacing;
    }

    public void updateInformation() {
        for (T o : objects)
            o.updateInformation();
    }

    
    //
    // ItemSelectable contract
    //

    public void addItemListener(ItemListener l) {
        listeners.add(l);
    }

    public Object[] getSelectedObjects() {
        if (selectionType == SINGLE)
            return new Object[] { objects.get(selectedIndex) };
        else {
            Vector<T> selection = new Vector<T>();
            for (T o : objects)
                if (o.isSelected())
                    selection.add(o);
            return selection.toArray();
        }
    }

    public void removeItemListener(ItemListener l) {
        listeners.remove(l);
    }

    // When in multiple selection mode, change the selection state of the item
    // at the given index (to the opposite of the current).
    private void fireSelectionChange(int i) {
        if (selectionType == SINGLE)
            return;

        if (objects.get(i).isSelected()) {
            objects.get(i).setSelected(false);

            ItemEvent desel = new ItemEvent(this, ItemEvent.ITEM_FIRST + i,
                objects.get(i), ItemEvent.DESELECTED);

            for (ItemListener l : listeners)
                l.itemStateChanged(desel);

        }else {
            objects.get(i).setSelected(true);

            ItemEvent sel = new ItemEvent(this, ItemEvent.ITEM_FIRST + i,
                objects.get(i), ItemEvent.SELECTED);

            for (ItemListener l : listeners)
                l.itemStateChanged(sel);

        }
    }

    // When in single selection mode, change the currently selected item from
    // the item at the 'old' index to the 'cur' index.
    private void fireSelectionChange(int old, int cur) {
        if (selectionType == MULTIPLE || cur == old)
            return;
        selectedIndex = cur;

        if (old != -1) {
            objects.get(old).setSelected(false);

            ItemEvent desel = new ItemEvent(this, ItemEvent.ITEM_FIRST + old,
                objects.get(old), ItemEvent.DESELECTED);
    
            for (ItemListener l : listeners)
                l.itemStateChanged(desel);
        }

        if (cur != -1) {
            objects.get(cur).setSelected(true);

            ItemEvent sel = new ItemEvent(this, ItemEvent.ITEM_FIRST + cur,
                objects.get(cur), ItemEvent.SELECTED);
    
            for (ItemListener l : listeners)
                l.itemStateChanged(sel);
        }
    }

    //
    // MouseListener contract
    //

    public void mouseClicked(MouseEvent e) {
        for (int i = 0; i < objects.size(); i++) {
            if (objects.get(i).getBounds().contains(e.getPoint())) {
                select(i);
                break;
            }
        }
    }

    public void mouseEntered(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {
        dragging = false;
    }

    public void mousePressed(MouseEvent e) {
    }

    public void mouseReleased(MouseEvent e) {
        dragging = false;
    }

    //
    // MouseMotionListener methods
    //
    
    public void mouseDragged(MouseEvent e) {
        if (dragging)
            return;

        dragging = true;
    }

    public void mouseMoved(MouseEvent e) {
    }

    private static class ListInnerPanel<S extends ListObject> 
            extends JPanel implements Scrollable {

        private ListPanel<S> parent;

        public ListInnerPanel() {
            super();
            parent = null;
            setLayout(null);
        }

        public void setParent(ListPanel<S> p) {
            parent = p;
        }

        public void validate() {
            if (parent == null)
                return;

            Rectangle r = new Rectangle(0, 0, getPreferredSize().width,
                parent.getRowHeight());

            for (S o : parent.objects) {
                o.setBounds(r);
                o.updateInformation();
                r.y += parent.getRowHeight() + parent.getRowSpacing();
            }
        }

        public Dimension getMinimumSize() {
            Dimension d = super.getMinimumSize();
            Dimension sub_d;

            if (parent != null)
                d.height = 0;

            for (Component c : getComponents()) {
                sub_d = c.getMinimumSize();
                d.width = Math.max(d.width, sub_d.width);
                if (parent != null)
                    d.height += parent.getCombinedHeight();
            }
            return d;
        }

        public Dimension getPreferredSize() {
            Dimension d = getMinimumSize();
            if (parent != null) {
                Dimension view_d = parent.getViewport().getExtentSize();
                d.width = view_d.width;
            }
            return d;
        }

        public void setBounds(int x, int y, int width, int height) {
            Dimension d = getPreferredSize();
            super.setBounds(x, y, d.width, d.height);
        }

        public void setBounds(Rectangle r) {
            // Call other overridden method signature
            setBounds(r.x, r.y, r.width, r.height);
        }

        public void setSize(int width, int height) {
            Dimension d = getPreferredSize();
            super.setSize(d.width, d.height);
            validate();
        }

        public void setSize(Dimension d) {
            setSize(d.width, d.height);
        }

        //
        // Scrollable interface contract
        //

        public Dimension getPreferredScrollableViewportSize() {
            return getPreferredSize();
        }

        public int getScrollableUnitIncrement(Rectangle visibleRect,
                int orientation, int direction) {
            int y = visibleRect.y, h = parent.getCombinedHeight();

            int diff;
            if (direction < 0)
                // scroll up by the modulus of the height
                diff = y % h;
            else
                // scroll down by the remaineder from the modulus
                diff = h - (y % h);

            if (diff == 0)
                // but if that is zero (we're scrolling up, on a block)
                // correct for it
                diff = h;

            return diff;
        }
        
        public int getScrollableBlockIncrement(Rectangle visibleRect, 
                int orientation, int direction) {
            int y = visibleRect.y, h = parent.getCombinedHeight();

            int diff;
            if (direction < 0)
                // scroll up by the modulus of the height
                diff = y % h;
            else
                // scroll down by the remaineder from the modulus
                diff = h - (y % h);

            if (diff == 0)
                // but if that is zero (we're scrolling up, on a block)
                // correct for it
                diff = h;

            return diff;
        }

        public boolean getScrollableTracksViewportHeight() {
            return false;
        }

        public boolean getScrollableTracksViewportWidth() {
            return false;
        }

    }

}

