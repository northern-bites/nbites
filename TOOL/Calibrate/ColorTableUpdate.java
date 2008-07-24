package TOOL.Calibrate;

import java.util.Vector;
import java.util.Iterator;

import TOOL.TOOL;

/**
 * Class is a data structure to hold changes to the color table. 
 * 
 * @author Joho Strom
 * @see ColorTable, Calibrate
 */


public class ColorTableUpdate{
    private static final int NUM_YUV = 3;
    // Backwards from normal. 
    private static final int Y = 0;
    private static final int U = 2;
    private static final int V = 1;
    
    //mode variables
    private byte color; //what is the color of the values?
    private int size; //stores how many pixels are in this DS
    //main data structure
    private Vector<int[]> pixels;
    private Vector<Byte> oldColors;
  
    /**
     * initialize a data structure to get ready
     * to hold a series of pixels
     */
    public ColorTableUpdate(byte _color){
	color = _color;
	size = 0;
	pixels =  new Vector<int[]>();
	oldColors = new Vector<Byte>();
    }


    public byte getColor(){
	return color;
    }

    public void addPixel(int[] newPixel, byte oldColor){
	if(newPixel.length == NUM_YUV){
	    pixels.add(newPixel);
	    oldColors.add(new Byte(oldColor));
	    ++size;
	}else
            TOOL.CONSOLE.error("Tried to add bad pixel");
    }

    public Iterator<int[]> getIterator(){
	return pixels.iterator();
    }
    
    public Iterator<Byte> getOldColorIterator() {
	return oldColors.iterator();
    }

    public int getSize(){
	return size;
    }

    public int getAverageYChannel(byte color) {
        return getAverageChannel(Y, color);
    }
    public int getAverageUChannel(byte color) {
        return getAverageChannel(U, color);
    }
    public int getAverageVChannel(byte color) {
        return getAverageChannel(V, color);
    }

    public int getAverageChannel(int channel, byte color) {
        if (channel < 0 || channel > NUM_YUV) {
            System.err.println("Error, got channel " + channel + " in " + 
                               "ColorTableUpdate.getAverageChannel(); "
                               + "expected 0, 1, or 2");
            return - 1;
        }
        if (this.color != color) {
            return 0;
        }
        int sum = 0;
        // We have the correct color, so total up the values
        for (int i = 0; i < size; i++) {
            sum += pixels.get(i)[channel];
        }

        //System.out.println("Returning an average value of " + sum/size);

        return sum/size;

    }

}
