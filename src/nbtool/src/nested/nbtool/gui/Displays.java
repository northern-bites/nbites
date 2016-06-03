package nbtool.gui;

import java.util.LinkedList;

import javax.swing.SwingUtilities;

public final class Displays {

	private Displays(){}
	
	protected static final LinkedList<ToolDisplayHandler> displays =
			new LinkedList<>();
	
	protected static void addNew(ToolDisplayHandler handler) {
		synchronized(displays) {
			displays.add(handler);
		}
	}
	
	public static void requestAnotherDisplay() {
		
		SwingUtilities.invokeLater(new Runnable(){

			@Override
			public void run() {
				synchronized(displays) {
					for (ToolDisplayHandler handler : displays) {
						if (!handler.isVisible()) {
							handler.show(true);
							return;
						}
					}
				}
				
				/* no existing displays are hidden, create a new one */
				ToolDisplayHandler handler = new ToolDisplayHandler();
				addNew(handler);
				handler.show(true);
			}
			
		});
			
	}
	
}
