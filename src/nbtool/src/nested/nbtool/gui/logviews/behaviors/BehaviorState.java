package nbtool.gui.logviews.behaviors;

class BehaviorState {
	private int counter = 0;
	private String name = "";

	public BehaviorState() {
		//
	}

	public BehaviorState(String name) {
		this.name = name;
	}

	public void setName(String newName) {
		this.name = newName;
	}

	public String getName() {
		return this.name;
	}

	public void increment() {
		this.counter++;
	}

	public int getCounter() {
		return this.counter;
	}

	public String getCounterStr() {
		return this.counter + "";
	}
}