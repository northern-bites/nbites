package common;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager;
import java.awt.Rectangle;
import java.util.LinkedList;
import javax.swing.JComponent;

/**
 *
 * @author Michel Bartsch
 * 
 * This LayoutManager places components without any dependencies between each
 * other. Scaling the layouted container will make every component scaled
 * proportional.
 */
public class TotalScaleLayout implements LayoutManager
{
    /** The container to layout. */
    Container parent;
    /** The containers preferred size. */
    Dimension preferredSize;
    /** The containers minimum size. */
    Dimension minimumSize;
    /** List of all components to layout on the container */
    LinkedList<TotalScaleComponent> comps = new LinkedList<TotalScaleComponent>();
    
    
    /**
     * Creates a new TotalScaleLayout.
     * 
     * @param parent     The container to layout.
     * 
     */
    public TotalScaleLayout(Container parent)
    {
        this.parent = parent;
        preferredSize = parent.getPreferredSize();
        minimumSize = parent.getMinimumSize();
    }
    
    
    /**
     * This will add a Component to the container to be layout. Use this instead
     * of the add-method of the container itself.
     * 
     * @param x        Left-edge position on the container between 0 and 1.
     * @param y        Top-edge position on the container between 0 and 1.
     * @param width    Width on the container between 0 and 1.
     * @param height   Height on the container between 0 and 1.
     * @param comp     Component to be added.
     * 
     */
    public void add(double x, double y, double width, double height, JComponent comp)
    {
        parent.add(comp);
        comps.add(new TotalScaleComponent(x, y, width, height, comp));
    }
    
    /**
     * Not supported because of own add-method.
     */
    @Override
    public void addLayoutComponent(String name, Component comp) {}

    /**
     * Gets called to remove Component.
     * 
     * @param comp      Component to remove.
     */
    @Override
    public void removeLayoutComponent(Component comp)
    {
        for (TotalScaleComponent tscomp : comps) {
            if (tscomp.comp == comp) {
                comps.remove(tscomp);
                break;
            }
        }
    }

    /**
     * Because all of the Components`s sizes are adapted to the containers size
     * this will allways return the size of the container when the layout was
     * created.
     * 
     * @param parent      Container this layout belongs to.
     * 
     * @return            This layouts preferred size.
     */
    @Override
    public Dimension preferredLayoutSize(Container parent)
    {
        return preferredSize;
    }

    /**
     * Because all of the Components`s sizes are adapted to the containers size
     * this will allways return the size of the container when the layout was
     * created.
     * 
     * @param parent      Container this layout belongs to.
     * 
     * @return            This layouts minimum size.
     */
    @Override
    public Dimension minimumLayoutSize(Container parent)
    {
        return minimumSize;
    }

    /**
     * This gets called automatically to adapt position and size of all
     * components.
     * 
     * @param parent      Container to layout.
     */
    @Override
    public void layoutContainer(Container parent)
    {
        Rectangle parentBounds = parent.getBounds();
        for (TotalScaleComponent comp : comps) {
            comp.comp.setBounds(
                    (int)(comp.x*parentBounds.width),
                    (int)(comp.y*parentBounds.height),
                    (int)(comp.width*parentBounds.width),
                    (int)(comp.height*parentBounds.height));
        }
    }
    
    /**
    * This class simply wraps some attributes like a struct.
    */
    class TotalScaleComponent
    {
        /* Left-edge position on the container between 0 and 1. */
        double x;
        /* Top-edge position on the container between 0 and 1. */
        double y;
        /* Width on the container between 0 and 1. */
        double width;
        /* Height on the container between 0 and 1. */
        double height;
        /* Component to be layouted on the container. */
        Component comp;
        
        /**
        * Creates a new TotalScaleComponent.
        * 
        * @param x        Left-edge position on the container between 0 and 1.
        * @param y        Top-edge position on the container between 0 and 1.
        * @param width    Width on the container between 0 and 1.
        * @param height   Height on the container between 0 and 1.
        * @param comp     Component to be layouted on the container.
        * 
        */
        TotalScaleComponent(double x, double y, double width, double height, Component comp)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
            this.comp = comp;
        }
    }
}