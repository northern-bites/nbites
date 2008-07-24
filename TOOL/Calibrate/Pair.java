package TOOL.Calibrate;

/**
 * Simple class that holds a pair of objects.  Uses generics
 * so that anything can be held in the pair.  Similar to anything
 * you'd find online.
 * @author Nicholas Dunn
 * @version 1.0 11/26/2007
 */

public class Pair <E, F> {
    
    private E first;
    private F second;

    public Pair(E first, F second) {
	this.first = first;
	this.second = second;
    }

    public boolean equals(Pair toCompare) {
	return (first.equals(toCompare.first) &&
		second.equals(toCompare.second));

    }

    public E getFirst() {
	return first;
    }

    public F getSecond() {
	return second;
    }

    public void setFirst(E first) {
	this.first = first;
    }

    public void setSecond(F second) {
	this.second = second;
    }

    public String toString() {
	return "First: " + first.toString() + ", Second: " 
	    + second.toString();
    }


}