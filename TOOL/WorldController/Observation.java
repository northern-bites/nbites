package TOOL.WorldController;

public class Observation {

	private int id;
	private float distance, bearing;
	public Observation(int _id, float _distance, float _bearing)
	{
		id = _id;
		distance = _distance;
		bearing = _bearing;
	}

	public int getID() { return id; }
	public float getDistance() { return distance; }
	public float getBearing() { return bearing; }
}