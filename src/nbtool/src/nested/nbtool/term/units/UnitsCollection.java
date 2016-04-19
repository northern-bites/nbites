package nbtool.term.units;

import java.util.LinkedList;
import java.util.List;

import nbtool.util.Debug;

public class UnitsCollection {
	
	private static final List<UnitParent> units = new LinkedList<>();
	
	static {
		units.add(new JsonDecodeUnit());
		units.add(new JsonEncodeUnit());
		
		units.add(new CorrelationUnit());
	}
	
	public static boolean run(String path) {
		Debug.printf("running unit tests with resources at: %s", path);
		for (UnitParent unit : units) {
			if (!unit.testAndPrint(path)) {
				return false;
			}
		}
		
		return true;
	}
	
	public static void main(String[] args) {
		UnitsCollection.run("NO PATH");
	}
}
