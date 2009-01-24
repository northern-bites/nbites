package TOOL.PlayBookEditor;

/**
 * Simple class to hold a range of values
 */
public class Range
{
    private int min;
    private int max;

    /**
     * @param min Minimum range value
     * @param max Maximum range value
     */
    public Range(int min, int max)
    {
	this.min = min;
	this.max = max;
    }

    /**
     * @return The minimum range value
     */
    public int getMin()
    {
	return min;
    }
    /**
     * @return The maximum range value
     */
    public int getMax()
    {
	return max;
    }

}