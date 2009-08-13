
package TOOL.VisionTester;

import java.awt.Component;

import TOOL.TOOL;
import TOOL.TOOLModule;

public class VisionTesterModule extends TOOLModule {

	private VisionTester visionTester;

	public VisionTesterModule(TOOL t){
		super(t);
		visionTester = new VisionTester(t);

	}

	public String getDisplayName() {
		return "Vision Tester";
	}

	public Component getDisplayComponent() {
		return visionTester.getContentPane();
	}
}