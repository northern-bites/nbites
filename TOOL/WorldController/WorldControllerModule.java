package TOOL.WorldController;

// The area in which we draw things
import java.awt.Component;

import TOOL.TOOL;
import TOOL.TOOLModule;
//import TOOL.WorldController.WorldController;

/**
 * This is the World Controller module. This program has been used a lot
 * in the past. Now it's part of TOOL and it's one of the panels at the top.
 */

public class WorldControllerModule extends TOOLModule {
    private WorldController worldController;

    public WorldControllerModule(TOOL tool) {
	super(tool);

	worldController = new WorldController(t);

//	t.getDataManager().addDataListener(worldController);
}

    public String getDisplayName() {
	return "WorldController";
    }

    public Component getDisplayComponent() {
	return worldController;
    }
}