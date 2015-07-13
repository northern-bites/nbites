package controller.ui;

import data.AdvancedData;


/**
 *
 * @author Michel Bartsch
 *
 * This inteface is for a GUI.
 * Evertimes the model changes the update method will be called with the new
 * model as data.
 * 
 * In additional a GUI can get the last action that caused the change from
 * the EventHandler, but this should not be used too often to avoid
 * dependencies.
 */
public interface GCGUI
{   
    /**
     * Called evertimes the model has changed, so the GUI can update it´s
     * view.
     * 
     * @param data  The Model to view.
     */
    public void update(AdvancedData data);
}