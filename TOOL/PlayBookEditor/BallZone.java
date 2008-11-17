package edu.bowdoin.robocup.TOOL.PlayBookEditor;
import java.util.LinkedList;

public class BallZone
{
    protected LinkedList<Zone> componentZones;

    /**
     * @param fieldZones Area of the zones we wish to hold together
     */
    public BallZone(LinkedList<Zone> fieldZones)
    {
	componentZones = fieldZones;
    }

    public LinkedList<Zone> getZones()
    {
	return componentZones;
    }
}