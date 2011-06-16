package TOOL.Image;

import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.awt.Frame;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.util.Iterator;
import java.util.LinkedList;
// used for autosaving
import java.util.Calendar;
import java.text.SimpleDateFormat;
import javax.swing.JOptionPane;


import TOOL.TOOL;
import TOOL.Calibrate.ColorTableUpdate;
import TOOL.Vision.Vision;

/**
 *
 * Color table class that holds a three-dimensional array representing
 * the color table. It contains functionality to read and write to binary files
 * on disk, as well as to scale up and down the table, and also to modify the
 * table.
 *
 * "Raw" methods simply do not do any bit shifting.  They exist to reduce the
 * amount of work for the ColorEdit module.  If we did it naively and
 * ranged from 0 - 255 in each of the Y,U,V channels and then bitshifted by one
 * byte, this has the effect of making 8 times as many queries as we would
 * if we just got from 0 - 127 in each of the Y,U,V channels and do no
 * bitshifting.
 *
 ******************************* NOTE ********************************
 *
 * The table DOES NOT store the values in y,u,v order though. They are stored
 * in u,v,y order to reflect the way they are thresholded. Since Y is the most
 * frequently changing channel, it is in the least significant position.
 *
 * ********************************************************************
 *
 * @author Joho Strom
 * @author Nicholas Dunn modified for undo/redo functionality.
 * @author Jack Morrison changed yuv ordering to uvy ordering
 * @author Wils Dawson changed load/save paths to appropriate destination.
 * @see ColorTableUpdate, Calibrate, ThresholdedImage
 * @version 1.0
 */
public class ColorTable {
    //default table locations,
    //which get statically updated each time we save a new table
    public static String LOAD_TABLE_PATH = System.getProperty("user.dir")+
											"/../../data/tables";
    public static String SAVE_TABLE_PATH = System.getProperty("user.dir")+
											"/../../data/tables";

    public static final byte GREEN = Vision.GREEN;
    public static final byte BLUE = Vision.BLUE;
    public static final byte YELLOW = Vision.YELLOW;
    public static final byte ORANGE = Vision.ORANGE;
    public static final byte YELLOWWHITE = Vision.YELLOWWHITE;
    public static final byte BLUEGREEN = Vision.BLUEGREEN;
    public static final byte ORANGERED = Vision.ORANGERED;
    public static final byte ORANGEYELLOW = Vision.ORANGEYELLOW;
    public static final byte RED = Vision.RED;
    public static final byte WHITE = Vision.WHITE;
	public static final byte GREY = Vision.GREY;
    public static final byte NAVY = Vision.NAVY;

    public static final byte BIT_GREY = 0x00;
    public static final byte BIT_WHITE = 0x01;
    public static final byte BIT_GREEN = 0x02;
    public static final byte BIT_BLUE = 0x04;
    public static final byte BIT_YELLOW = 0x08;
    public static final byte BIT_ORANGE = 0x10;
    public static final byte BIT_RED = 0x20;
    public static final byte BIT_NAVY = 0x40;


    public static final int FAIL = 1;
    public static final int SUCCESS = 0;

    public static final int LOAD = 1;
    public static final int EMPTY = 0;

    protected static boolean AUTO_SAVE = false;
    protected static int NUM_MOVES_TO_AUTOSAVE = 30;
    public static String AUTO_SAVE_PATH = SAVE_TABLE_PATH + "autosave/";

    //note that these are the YUV encodings on the aibo, not the way
    //they are represented in standard YCbCr land.
    private static final int Y = 0;
    private static final int Cb = 2;
    private static final int Cr = 1;


    // We will store the last 50 updates to the color table
    public static final int MAX_NUM_UNDOS = 50;
    public static final int MAX_NUM_REDOS = MAX_NUM_UNDOS;


    protected int moveCounter = 0;

    private int yShift,uShift,vShift,yMax,uMax,vMax;

    private byte[][][] colorTable;
    /*
      private byte[][][] thisTable,newTable;
      private int thisTableYMax,thisTableUMax,thisTableVMax;
      private int newTableYMax,newTableUMax,newTableVMax;
    */

    //the minumum times a color must show up during downscaling
    //before the uses it
    private static final int MIN_OCCURENCES_PRESENT = 4;
    //the number of colors possible in a table to be downscaled
    private static final int POSSIBLE_COLORS = 14;

    private boolean softColors;

    //store for saving functionality
    private boolean notYetSaved = true; //store if it's ever been saved
    private boolean modified = false; //store if it's been modified after save

    private String fileName;

    // We need a LinkedList within the stack because a single action might
    // change multiple colors, necessitating multiple ColorTableUpdates
    private LinkedList <LinkedList <ColorTableUpdate> > undoStack, redoStack;


    public enum Dimension {
        GERMANTEAM(64,64,64),
            SMALL(32,64,64),
            LARGE(128,128,128);
        private final int yMax, uMax, vMax;
        private final int numBytes;
        Dimension(int yMax, int uMax, int vMax) {
            this.yMax = yMax;
            this.uMax = uMax;
            this.vMax = vMax;
            numBytes = yMax * uMax * vMax;
        }
        public int[] getDimensions() { return new int[] {yMax, uMax, vMax}; }
        public int getYMax() { return yMax; }
        public int getUMax() { return uMax; }
        public int getVMax() { return vMax; }
        public int getNumBytes() { return numBytes; }
        public String toString() { return name() + "[" + yMax + "][" + uMax +
                "][" + vMax + "]"; }
    }


    public ColorTable(int type, Dimension size) throws IOException {
        this(type, size.getYMax(), size.getUMax(), size.getVMax());
    }



    public ColorTable(int type, int yMax, int uMax, int vMax)
        throws IOException{

        if(type == LOAD){

            String path = TOOL.CONSOLE.
                promptFileOpen("Existing Color Table Location and Name",
                               LOAD_TABLE_PATH);

            if(path != null){
                loadDefFromFile(path, yMax,uMax,vMax);
                LOAD_TABLE_PATH = path;

            }else{
                TOOL.CONSOLE.message("No table loaded!");
                throw new IOException("couldn't load a colorTable");
            }
        }else if (type==EMPTY){
            colorTable = new byte[uMax][vMax][yMax];

            TOOL.CONSOLE.message("Created new blank color table of " +
                                 "dimensions[" +uMax + "][" + vMax + "][" +
                                 yMax + "]");
            // Takes care of updating the this.yMax variables, as well as
            // this.yShift etc.
            updateDimensions();
        }else{
            TOOL.CONSOLE.error("new color table failed- incorrect code");
        }

        undoStack = new LinkedList <LinkedList <ColorTableUpdate> >();
        redoStack = new LinkedList <LinkedList <ColorTableUpdate> >();

    }


    /**
     * CONSTRUCTOR: new color table - no path specified, so we ask the user.
     */
    public ColorTable(int type) throws IOException {
        this(Vision.YMAX, Vision.UMAX, Vision.VMAX);
    }

    /**
     * CONSTRUCTOR: new color table at the specified path
     */
    public ColorTable(String path){
       	Dimension d = null;
        if (path != null) {
            d = getSize(path);
            if (d != null) {
                loadDefFromFile(path, d.getYMax(),d.getUMax(),d.getVMax());
                LOAD_TABLE_PATH = path;
            }
        }
        // Either the path was null or user cancelled the creation process
        if (d == null || path == null) {
            TOOL.CONSOLE.message("No table loaded!");
            return;
        }

        undoStack = new LinkedList <LinkedList <ColorTableUpdate> >();
        redoStack = new LinkedList <LinkedList <ColorTableUpdate> >();
    }

    /**
     * CONSTRUCTOR: new table with a distinct shift set
     */
    public ColorTable(int yMax, int uMax, int vMax){
        String path = TOOL.CONSOLE.promptFileOpen(
                                                  "Open OLD Color Table Location Size = (" + Vision.YMAX + ","
                                                  + Vision.UMAX + "," + Vision.VMAX + ")",
                                                  LOAD_TABLE_PATH);

        if (path != null) {
            loadDefFromFile(path, Vision.YMAX, Vision.UMAX,
                            Vision.VMAX);
            LOAD_TABLE_PATH = path;
        }else{
            TOOL.CONSOLE.message("No table loaded!");
        }

        undoStack = new LinkedList <LinkedList <ColorTableUpdate> >();
        redoStack = new LinkedList <LinkedList <ColorTableUpdate> >();

    }

    /**
     * Controls whether the table is autosaved every NUM_MOVES_TO_AUTOSAVE
     * or not.
     */
    public void setAutoSave(boolean choice) {
        AUTO_SAVE = choice;
    }


    /**
     * modifies the color table according to the specified updates,
     * entries are added for all the YUV points contained within u
     *
     *@param u  a list of all the YUV triplets and a color to assign them to.
     *
     */
    public void modifyTable(ColorTableUpdate u){
        modified = true;
        byte color = u.getColor();

        if (softColors) {

            for(Iterator<int[]> i = u.getIterator();i.hasNext();){
                int[] thisPixel = i.next();
                byte oldColor = getColor(thisPixel);
                byte newColor = color;

                if ((color == GREEN && oldColor == BLUE) ||
                    (color == BLUE && oldColor == GREEN) ||
                    (color == GREEN && oldColor == BLUEGREEN) ||
                    (color == BLUE && oldColor == BLUEGREEN)) {
                    newColor = BLUEGREEN;
                }

                else if ((color == YELLOW && oldColor == WHITE) ||
                         (color == WHITE && oldColor == YELLOW) ||
                         (color == YELLOW && oldColor == YELLOWWHITE) ||
                         (color == WHITE && oldColor == YELLOWWHITE)) {
                    newColor = YELLOWWHITE;
                }

                else if ((color == ORANGE && oldColor == RED) ||
                         (color == RED && oldColor == ORANGE) ||
                         (color == ORANGE && oldColor == ORANGERED) ||
                         (color == RED && oldColor == ORANGERED)) {
                    newColor = ORANGERED;
                }

                else if ((color == ORANGE && oldColor == YELLOW) ||
						 // (color == YELLOW && oldColor == ORANGE) ||
                         (color == ORANGE && oldColor == ORANGEYELLOW) ||
                         (color == YELLOW && oldColor == ORANGEYELLOW)) {
					//System.out.println(color+" "+oldColor);
                    newColor = ORANGEYELLOW;
                    }
                setColor(thisPixel, newColor);
            }
        }
        // Softcolors are off, just overwrite what was previously there
        else {
            for(Iterator<int[]> i = u.getIterator();i.hasNext();){
                int[] thisPixel = i.next();
                setColor(thisPixel, color);
            }
        }


        tickAutoSave();
    }


    //used for external modifications directly to the color table
    //the only difference is that in this one, we attempt to auto save
    //if necessary
    public void modifyTableDirectly(ColorTableUpdate u){
        modifyRawTable(u);

        tickAutoSave();

    }


    //used for internal modifications directly to the color table
    private void modifyRawTable(ColorTableUpdate u) {
        modified = true;
        byte color = u.getColor();
        for(Iterator<int[]> i = u.getIterator();i.hasNext();){
            int[] thisPixel = i.next();
            setRawColor(thisPixel, color);
        }
    }


    //used for external modifications directly to the color table
    //the only difference is that in this one, we attempt to auto save
    //if necessary
    public void modifyTableDirectly(ColorTableUpdate u, byte newColor){
        modifyRawTable(u, newColor);

        tickAutoSave();

    }


    //used for internal modifications directly to the color table
    private void modifyRawTable(ColorTableUpdate u, byte newColor) {
        modified = true;
        for(Iterator<int[]> i = u.getIterator();i.hasNext();){
            int[] thisPixel = i.next();
            setRawColor(thisPixel, newColor);
        }
    }


    //when you change the table, you should  let the autosaver know:
    private void tickAutoSave(){
        if (AUTO_SAVE &&
            ++moveCounter % NUM_MOVES_TO_AUTOSAVE == 0) {

            // Ensure that the directory exists
            String saveDir = AUTO_SAVE_PATH +
                now("MM-dd-yyyy") + "/";
            File cur = new File(saveDir);
            cur.mkdirs();

            TOOL.CONSOLE.println("Auto saving after " + moveCounter +
                                 " moves.");
            // hours/minutes/seconds am/pm .table.mtb
            saveColorTable(saveDir + now("h:mm:ssa") +
                           ".table.mtb");
            moveCounter = 0;
        }
    }

    /**
     * @see http://www.rgagnon.com/javadetails/java-0106.html
     * Parses the format of the date and returns a string in that format.
     */
    public static String now(String dateFormat) {
        Calendar cal = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat(dateFormat);
        return sdf.format(cal.getTime());
    }




    // Methods for bitshifting
    public int getYShift() { return yShift; }

    public int getUShift() { return uShift; }

    public int getVShift() { return vShift; }

    public int getCRShift() { return vShift; }

    public int getCBShift() { return uShift; }


    // UNDO / REDO METHODS


    /**
     * Pushes a single redo onto the stack by converting it into a list
     * and then pushing subsequent list to the stack */
    public void pushRedo(ColorTableUpdate change) {
        LinkedList <ColorTableUpdate> theChange =
            new LinkedList<ColorTableUpdate>();
        theChange.add(change);

        pushRedo(theChange);
    }

    /**
     * Pushes a LinkedList of changes onto the redo stack
     */
    public void pushRedo(LinkedList <ColorTableUpdate> change) {

        // We limit our "stack" size by eliminating the oldest entry
        // when we have too many
        if (redoStack.size() == MAX_NUM_REDOS) {
            //TOOL.CONSOLE.println("Redo stack reached size " +
            //redoStack.size());
            //TOOL.CONSOLE.println("Removing: " + redoStack.getFirst());
            undoStack.removeFirst();
        }
        redoStack.addLast(change);
    }

    public void clearRedo() {
        redoStack.clear();
    }


    /**
     * Converts a single ColorTableUpdate into
     * a one element list and then adds the list to the stack.
     */
    public void pushUndo(ColorTableUpdate change) {
        LinkedList <ColorTableUpdate> theChange =
            new LinkedList<ColorTableUpdate>();
        theChange.add(change);

        pushUndo(theChange);
    }
    /** Pushes a single change to stack */
    public void pushUndo(LinkedList <ColorTableUpdate> change) {
        if (undoStack.size() == MAX_NUM_UNDOS) {
            //TOOL.CONSOLE.println("Undo stack reached size " +
            // undoStack.size());
            //TOOL.CONSOLE.println("Removing: " + undoStack.getFirst());
            undoStack.removeFirst();
        }
        undoStack.addLast(change);
    }

    public boolean canUndo() {
        return undoStack.size() > 0;
    }

    public boolean canRedo() {
        return redoStack.size() > 0;
    }

    public int getUndoSize() {
        return undoStack.size();
    }

    public int getRedoSize() {
        return redoStack.size();
    }

    /**
     * Undoes the most recent change to the color table.
     * Adds the change to the redo stack.
     * Note that a single atomic change might consist of multiple
     * ColorTableUpdate objects, since filling holes in a table
     * edits multiple color entries.
     * @precondition undoStack.size() > 0
     */
    public void undo() {
        LinkedList <ColorTableUpdate> changes = undoStack.removeLast();
        // Make sure that we can redo the move we're undoing now
        redoStack.addLast(changes);

        Iterator toUndo = changes.iterator();
        while (toUndo.hasNext()) {
            undoChanges((ColorTableUpdate)toUndo.next());
        }
    }



    public void rawUndo() {
        LinkedList <ColorTableUpdate> changes = undoStack.removeLast();
        // Make sure that we can redo the move we're undoing now
        redoStack.addLast(changes);

        Iterator toUndo = changes.iterator();
        while (toUndo.hasNext()) {
            rawUndoChanges((ColorTableUpdate)toUndo.next());
        }


    }

    /**
     * Redoes the most recent change added to redo stack.
     * Adds the change back onto undo stack.
     * @precondition redoStack.size() > 0
     */
    public void redo() {
        LinkedList <ColorTableUpdate> changes = redoStack.removeLast();
        // We can undo this move right after
        undoStack.addLast(changes);

        Iterator toRedo = changes.iterator();
        while (toRedo.hasNext()) {
            modifyTable((ColorTableUpdate)toRedo.next());
        }
    }


    public void rawRedo() {
        LinkedList <ColorTableUpdate> changes = redoStack.removeLast();
        // We can undo this move right after
        undoStack.addLast(changes);

        Iterator toRedo = changes.iterator();
        while (toRedo.hasNext()) {
            modifyRawTable((ColorTableUpdate)toRedo.next());
        }

    }


    /**
     * Undoes a single ColorTableUpdate, which comprises all changes
     * user made to one thresholded color.
     */
    public void undoChanges(ColorTableUpdate u) {

        Iterator <Byte> oldColors = u.getOldColorIterator();

        for (Iterator<int[]>i = u.getIterator(); i.hasNext();  ) {
            int[] thisPixel = i.next();
            byte oldColor = oldColors.next();
            setColor(thisPixel, oldColor);
        }
    }



    public void rawUndoChanges(ColorTableUpdate u) {
        Iterator <Byte> oldColors = u.getOldColorIterator();

        for (Iterator<int[]>i = u.getIterator(); i.hasNext();  ) {
            int[] thisPixel = i.next();
            byte oldColor = oldColors.next();

            setRawColor(thisPixel, oldColor);
        }

    }


    // April 2011 - with the switch to bits, rather than just rewrite the whole
    // tool (which would take forever) we're just going to have the table class
    // wrap all of the changes and hope the other classes don't manipulate the
    // table directly themselves

    public byte convertColorFromBits(byte col) {
        if ((col & BIT_ORANGE) > 0) {
            if ((col & BIT_RED) > 0) {
                return ORANGERED;
            }
            if ((col & BIT_YELLOW) > 0) {
                return ORANGEYELLOW;
            }
            return ORANGE;
        }
        if ((col & BIT_GREEN) > 0) {
            if ((col & BIT_BLUE) > 0) {
                return BLUEGREEN;
            }
            return GREEN;
        }
        if ((col & BIT_BLUE) > 0) {
            return BLUE;
        }
        if ((col & BIT_YELLOW) > 0) {
            if ((col & BIT_WHITE) > 0) {
                return YELLOWWHITE;
            }
            return YELLOW;
        }
        if ((col & BIT_WHITE) > 0) {
            return WHITE;
        }
        if ((col & BIT_RED) > 0) {
            return RED;
        }
        return GREY;
    }

    public byte convertFromOldColor(byte color) {
        switch (color) {
        case GREY: return BIT_GREY;
        case GREEN: return BIT_GREEN;
        case WHITE: return BIT_WHITE;
        case YELLOW: return BIT_YELLOW;
        case BLUE: return BIT_BLUE;
        case ORANGE: return BIT_ORANGE;
        case NAVY: return BIT_NAVY;
        case RED: return BIT_RED;
        case ORANGERED: return BIT_ORANGE | BIT_RED;
        case ORANGEYELLOW: return BIT_ORANGE | BIT_YELLOW;
        case YELLOWWHITE: return BIT_YELLOW | BIT_WHITE;
        case BLUEGREEN: return BIT_BLUE | BIT_GREEN;
        default: return BIT_GREY;
        }
    }

    //returns the color at a given pixel combination, specified by array
    public byte getColor(int[] pixel) {
        return convertColorFromBits(colorTable[pixel[Y ]>>uShift]
                                    [pixel[Cb]>>vShift]
                                    [pixel[Cr]>>yShift]);
    }

    public byte getColor(int _y, int _u, int _v) {
        return convertColorFromBits(colorTable[_u >> uShift]
                                    [_v >> vShift]
                                    [_y >> yShift]);
    }

    /** Gets color for given y,u,v, value without bitshifting */
    public byte getRawColor(int[] pixel) {
        return convertColorFromBits(colorTable[pixel[Cb ]]
                                    [pixel[Cr]]
                                    [pixel[Y]]);
    }

    public byte getRawColor(int _y, int _u, int _v) {
        return convertColorFromBits(colorTable[_u][_v][_y]);
    }

    public boolean isModified(){
        return modified;
    }

    public void setColor(int[] pixel, byte color) {
        color = convertFromOldColor(color);
        colorTable[pixel[Cb ] >> uShift]
            [pixel[Cr] >> vShift]
            [pixel[Y] >> yShift] = color;
    }

    public void setRawColor(int[] pixel, byte color) {
        color = convertFromOldColor(color);
        colorTable[pixel[Cb ]]
            [pixel[Cr]]
            [pixel[Y]] = color;
    }

    //dynamically 'saves', or 'saves as' depending on if
    //the color table has already been saved.
    public int saveColorTable(){
        //check to see if this table has already been saved
        if(notYetSaved){
            //then this is the first time
            //so save as
            return saveColorTableAs();
        }else{
            return saveColorTable(SAVE_TABLE_PATH);
        }

    }


    //forces a dialog to ask where to save
    public int saveColorTableAs(){
        String path = TOOL.CONSOLE.promptFileSave("Save Color Table As:",
                                                  SAVE_TABLE_PATH);

        return saveColorTable(path);
    }

	/** Intersect the current table with a user specified table.
	 */
	public void intersect(String path) {
		ColorTable c = new ColorTable(path);
		// check for like values
        for(int y=0; y<yMax; y++) {
            for(int u=0; u<uMax; u++) {
                for(int v=0; v<vMax; v++){
					if (colorTable[y][u][v] != c.colorTable[y][u][v])
						colorTable[y][u][v] = GREY;
                }
            }
        }

	}

    private int saveColorTable(String path){
        if(colorTable != null && path != null){
            SAVE_TABLE_PATH = path;
            if(saveDefToFile(path) == SUCCESS){
                notYetSaved = false;
                modified = false;
                return SUCCESS;
            }
        }
        else{
            TOOL.CONSOLE.println("Table save to " + path +
                                 " failed because table "+
                                 "was null or invalid path");
        }
        return FAIL;
    }


    /**
     * load color header from file. then calls another load
     * function to get the actual table
     * @param fileName  file name with full path
     * @param yMax      Y-color-channel dimension
     * @param uMax      U-color-channel dimension
     * @param vMax      V-color-channel dimension
     */

    private void loadDefFromFile(String fileName,
                                 int _yMax, int _uMax, int _vMax){
        int byteCount = 0 ;
        FileInputStream fis = null;
        long startTime = System.currentTimeMillis();

        System.out.print("Loading Color Def File: " + fileName + "... ");

        try {
            fis = new FileInputStream(fileName);
        } catch (FileNotFoundException e) {
            TOOL.CONSOLE.error(e.getMessage());
            TOOL.CONSOLE.println("table load failed.");
            return;
        }


        byte[][][] newTable = new byte[_uMax][_vMax][_yMax];

        byte[] buffer = new byte[_yMax*_uMax*_vMax];
        int byteIndex = 0;
        //read in the whole table into a really long byte array
        try {
            fis.read(buffer);
            byteCount +=_yMax*_uMax*_vMax;
        } catch (IOException e) {
            TOOL.CONSOLE.error(e.getMessage());
            TOOL.CONSOLE.println("table load failed.");
            return;
        }

        //parse the byte buffer into the new table
        for(int u=0; u<_uMax; u++) {
            for(int v=0; v<_vMax; v++) {
                for(int y=0; y<_yMax; y++){
                    newTable[u][v][y] = buffer[byteIndex];
                    ++byteIndex;
                }
            }
        }


        try {
            fis.close();
        } catch (IOException e) {
            TOOL.CONSOLE.error(e.getMessage());
            TOOL.CONSOLE.println("table load failed.");
            return;
        }
        //check the new table to make sure it's ok,

        colorTable = newTable;
        double elapsedTime =
            (double)(System.currentTimeMillis() - startTime)/1000;
        TOOL.CONSOLE.println(byteCount/1024 + " KBs read in " +
                             elapsedTime+"Seconds");

        updateDimensions();

        // save the file name and location of the color table
        this.fileName = fileName;
    }


    /**
     * Pops up a dialog box that determines the size of the color table
     * user wants to make.  By default, option box is on LARGE choice.
     * If user clicks cancel or x, this method returns null.  Thus the
     * calling method should check for a null before using the result of
     * this method.
     */
    public static Dimension getSize() {
        Object[] options = {
            Dimension.GERMANTEAM.toString(),
            Dimension.SMALL.toString(),
            Dimension.LARGE.toString()
        };

        String s = (String)
            JOptionPane.showInputDialog(null, "Pick a color table size",
                                        "Color table size",
                                        JOptionPane.PLAIN_MESSAGE,
                                        null, options, options[2]);
        if (s == null) { return null; }
        else if (s.equals(options[0])) {
            return Dimension.GERMANTEAM;
        }
        else if (s.equals(options[1])) {
            return Dimension.SMALL;
        }
        else if (s.equals(options[2])) {
            return Dimension.LARGE;
        }
        return null;
    }

    /**
     * Determines the size of the color table about to be loaded based
     * on the file size.  Assumes there is nothing but raw bytes in the table,
     * no header or footer information.
     * @param s a file path to a table we are about to load.
     * @return Dimension d if color table at file path s is of size d, else
     *         null if incompatible with any known file size.
     */
    public static Dimension getSize(String s) {
        int length = (int) (new File(s)).length();

        final int GT_LEN = Dimension.GERMANTEAM.getNumBytes();
        final int SMALL_LEN = Dimension.SMALL.getNumBytes();
        final int LARGE_LEN = Dimension.LARGE.getNumBytes();

        if (length == GT_LEN) {
            return Dimension.GERMANTEAM;
        }
        else if (length == SMALL_LEN) {
            return Dimension.SMALL;
        }
        else if (length == LARGE_LEN) {
            return Dimension.LARGE;
        }
        else{
            TOOL.CONSOLE.error("Error: Attempted to load a color table of " +
                               "size " + length + " which was incompatible " +
                               "with known color table sizes.");
            return null;
        }

    }

    public void writeByteArray(byte[] dest){
        int byteIndex = 0;
        for(int u=0; u<uMax; u++)
            for(int v=0; v<vMax; v++)
                for(int y=0; y<yMax; y++){
                    dest[byteIndex] = colorTable[u][v][y];
                    byteIndex++;
                }
    }

    /**
     * Saves the table to disk.
     * @param fileName  compelete path +filename of destination
     * @return 0 on success, -1 on fail
     */
    private int saveDefToFile(String fileName){

        int byteCount = 0 ;
        FileOutputStream fos=null;
        boolean savedOk = true;

        TOOL.CONSOLE.println("Saving Color Defs to file: " + fileName + "... ");

        try {
            fos = new FileOutputStream(fileName,false);
        }
        //catch (Exception e) {
        catch (FileNotFoundException e) {
            TOOL.CONSOLE.println(e.getMessage());
            savedOk = false;
            TOOL.CONSOLE.println(" save failed.");
            return FAIL;
        }


        byte[] buffer = new byte[yMax*uMax*vMax];
        writeByteArray(buffer);

        try {
            fos.write(buffer);
            byteCount+=buffer.length;
        } catch (IOException e) {
            TOOL.CONSOLE.println(e.getMessage());
            savedOk = false;
        }

        try {
            fos.close();
        } catch (IOException e) {
            TOOL.CONSOLE.println(e.getMessage());
            savedOk = false;
        }

        if(savedOk){
            TOOL.CONSOLE.println(byteCount/1024 + " KBs written");
            return SUCCESS;
        }
        else{
            TOOL.CONSOLE.println(" save failed.");
            return FAIL;
        }
    }


    /**
     * Scales up a table -- relatively straightward, since
     * you can actually have an "identical" table to a smaller one
     * @param yScalar how much to expand along the Y channel
     * @param uScalar how much to expand along the U channel
     * @param vScalar how much to expand along the V channel
     */
    public void scaleUp(int yScalar, int uScalar, int vScalar){
        modified = true;
        TOOL.CONSOLE.println("Input Y-Dimension:"+yMax+"U-Dimension"
							 +uMax+"vDimension"+vMax+":");
        TOOL.CONSOLE.println("Input Y-scalar:"+yScalar+"U-Scalar"
							 +uScalar+"V-Scalar"+vScalar+":");
        byte[][][] newTable =  new byte[uMax*uScalar][vMax*vScalar][yMax*yScalar];
        TOOL.CONSOLE.println("TEST Y-Dimension:"+yMax+"U-Dimension"+
							 uMax+"vDimension"+vMax+":");

        //scan the old table under y,u,v
        for(int u = 0; u < uMax; u++){
            for(int v = 0; v < vMax; v++){
                for(int y = 0; y < yMax; y++){

                    //for each entry in the old table, we extrapolate it according to the scalar variable
                    for(int a = u*uScalar; a < u*uScalar+uScalar; a++){
                        for(int b = v*vScalar; b <v*vScalar+vScalar; b++){
                            for(int c = y*yScalar; c <y*yScalar+yScalar; c++){
                                newTable[a][b][c] = colorTable[u][v][y];
                            }
                        }
                    }
                    //end extrapolation

                }
            }
        }
        //end scanning for yuv

        colorTable = newTable;
        updateDimensions();
    }




    /**
     * Makes a smaller version of a larger table. Inherently not possible
     * to make an exact replica in a smaller space, so we iterate through
     * the old array, counting the number of times each color is encountered
     * during a section of (yDownScale, uDownScale, vDownScale) size. We
     * find the most commonly occuring color and assign the slot in the
     * new color table that value.
     *
     * @param yDownScale how much to shrink along the Y channel
     * @param uDownScale how much to shrink along the U channel
     * @param vDownScale how much to shrink along the V channel
     * @author Andrew Lawrence, Joho Strom
     */
    public void scaleDown(int yDownScale,
                          int uDownScale, int vDownScale){
        modified = true;

        int newYDimension = yMax/yDownScale;
        int newUDimension = uMax/uDownScale;
        int newVDimension = vMax/vDownScale;

        byte[][][] newTable = new byte[newUDimension][newVDimension]
            [newYDimension];

        //loops through the new, small table
        for(int u = 0; u < newUDimension; u++){
            for(int v = 0; v < newVDimension; v++){
                for(int y = 0; y < newYDimension; y++){

                    //creates an array that will help determine the mode of
                    //values in the old table
                    int[] colorFrequency = new int[POSSIBLE_COLORS];

                    //starts values where previous compression left off.
                    //searches from that spot forward, amount determined
                    //by downscale
                    //loops through the section of the large table to compress
                    for(int i = u*uDownScale; i <u*uDownScale+uDownScale; i++){
                        for(int j=v*vDownScale; j<v*vDownScale+vDownScale;j++){
                            for(int k=y*yDownScale;k<y*yDownScale+yDownScale;k++){

                                //increments a counter in corresponding array
                                //with the byte value at (i,j,k)
                                int thisColor = (int)(colorTable[i][j][k]);
                                colorFrequency[thisColor]++;
                            }
                        }
                    }

                    //store the index and value of the largest entry in the list
                    int mostCommon = 0;
                    int mostCommonIndex =0;

                    //skips checking grey by starting at 1
                    for(int i = 1; i<colorFrequency.length;  i++) {
                        int currentVal = colorFrequency[i];
                        if(currentVal > mostCommon){
                            mostCommon = currentVal;
                            mostCommonIndex = i;
                        }
                    }
                    //makes sure the color appears in a non-random amount
                    if(mostCommon > MIN_OCCURENCES_PRESENT){
                        newTable[u][v][y] = (byte)mostCommonIndex;
                    }
                }
            }
        }

        colorTable = newTable;
        updateDimensions();

        TOOL.CONSOLE.println("Created a new table with dimensions Y-Dimension:"
                             +newYDimension+"U-Dimension"+newUDimension+"vDimension"+newVDimension+":");

    }


    /**
     * Call this to update the internal variables which keep track
     * of table sizing. Call this anytime the physical structure of
     * the color table is changed. Don't need to call when simply adding, or
     * modifying a value in the table.
     */
    public void updateDimensions() {
        uMax = colorTable.length;
        vMax = colorTable[0].length;
        yMax = colorTable[0][0].length;

        uShift = (int)Math.round(Math.log(256/yMax)/Math.log(2));
        vShift = (int)Math.round(Math.log(256/uMax)/Math.log(2));
        yShift = (int)Math.round(Math.log(256/vMax)/Math.log(2));
    }

    public int getYDimension(){
        return yMax;

    }
    public int getUDimension(){
        return uMax;
    }

    public int getVDimension(){
        return vMax;

    }



    /**
       Method to fill all the holes in the table for a given color.

       We look at a 3D box around each entry in the color table to decide
       if it should be filled or not. The default behavior is to only consider
       undefined entries for filling, and we require a certain percentage of the
       neighbors to be of the correct color. The box we look at is by default
       3 entries wide - one entry to each side of the value we are considering
       flipping

       Note: This code is very general right now, because I intend to use
       the same code again later to do some more sophisticated processing of the
       table. Eventually, we should be able to achieve some more performance
       by only looking at neighbors of the intended color (colorToFillWith).

       Also, it could be faster to not look at a full box - just the pixels
       in the cardinal directions.

       Finally, I'd note that this method works differently than the way it worked
       in the JavaConnect side.

       This method applies all the changes necessary to the table - returns the
       update for others to look at for purposes of notifying changes, etc.

       @return ColorTableUpdate for all the holes now filled with desired color
    */
    public ColorTableUpdate fillHoles(byte colorToFillWith){
        int BOX = 1; // size of the box we look for is 2*BOX + 1
        double HOLE_THRESH_PERCENTAGE =.593; //what percent we need to fill a hole
        //~16 of the 27 neighbors need to be the right color
        int HOLE_THRESH =  (int)Math.round(HOLE_THRESH_PERCENTAGE*
                                           Math.pow(2*BOX + 1,3));
        boolean DEBUG_FILL_HOLES = false;

        //store the changes in a ColorTableUpdate
        ColorTableUpdate update = new ColorTableUpdate(colorToFillWith);

        //We look through all the entries in the color table,
        // except those within BOX of the edge
        for (int cr = BOX; cr < uMax - BOX; cr++) {
            for (int cb = BOX; cb < vMax - BOX; cb++) {
                for (int y = BOX; y < yMax - BOX; y++) {

                    //gets the entry we are considering filling
                    int target_color = colorTable[cr][cb][y];

                    if(target_color != Vision.GREY){
                        continue; //only overwrite grey pixels
                    }

                    //this is an array of counters to track
                    //the frequency of colors occuring in the BOX around target
                    int[] neighbor_count = new int[POSSIBLE_COLORS];

                    int maxColor = 0; //stores the most frequent color
                    int maxVal = 0;//stores the frequency of that color

                    //now look through all the pixels BOX away from the target
                    for(int ii = cr-BOX; ii <= cr +BOX; ii++){
                        for(int jj = cb -BOX; jj <= cb + BOX; jj++ ){
                            for(int kk = y -BOX; kk <= y+BOX; kk++){

                                //skip the center pixe, since it is the target
                                if(cr==ii && cb == jj && y ==kk){
                                    continue;
                                }

                                //grab a neighbor to the target, to test
                                int test_color = colorTable[ii][jj][kk];

                                //now track how  many of this color we've seen
                                ++neighbor_count[test_color];

                                //and check to see if now it is the most frequent
                                if(neighbor_count[test_color] > maxVal){
                                    maxVal = neighbor_count[test_color];
                                    maxColor = test_color;
                                }
                            }
                        }
                    }//done looking through the BOX

                    //lets see if we should assign the target something new:
                    if (maxColor == colorToFillWith &&
                        maxColor != target_color &&
                        maxColor !=Vision.GREY &&
                        maxVal > HOLE_THRESH){

                        //note the table is stored YCrCb, yet the convention is
                        //YCbCr, so we flip it to the convention for the update
                        int[] yuv_to_fill = {y,cb,cr};
                        update.addPixel(yuv_to_fill,(byte)target_color);

                        if(DEBUG_FILL_HOLES)
                            System.out.println(yuv_to_fill[0] +","+yuv_to_fill[2]
                                               +","+yuv_to_fill[1] +
                                               ", of color " + target_color +
                                               " was replaced by " + maxColor+
                                               " with " + maxVal+ "votes");

                    }


                }
            }
        }//done looking through the whole table

        //push the changes
        pushUndo(update);

        modifyTableDirectly(update);

        System.out.println("Filled " + update.getSize() +
                           " holes of color " +
                           Vision.COLOR_STRINGS[colorToFillWith] );


        return update;
    }



    /**
       Method to get rid of all "islands" of a given color.  Basically the
	   opposite of the fillHoles method.

       We look at a 3D box around each entry in the color table to decide
       if it should be removed or not.

       @return ColorTableUpdate for all the holes now filled with desired color
    */
    public ColorTableUpdate loseIslands(byte colorToFillWith){
        int BOX = 1; // size of the box we look for is 2*BOX + 1
        double HOLE_THRESH_PERCENTAGE =.593; //what percent we need to fill a hole

        //store the changes in a ColorTableUpdate
        ColorTableUpdate update = new ColorTableUpdate(colorToFillWith);

        //We look through all the entries in the color table,
        // except those within BOX of the edge
        for (int y = BOX; y < yMax - BOX; y++) {
            for (int cr = BOX; cr < uMax - BOX; cr++) {
                for (int cb = BOX; cb < vMax - BOX; cb++) {

                    //gets the entry we are considering filling
                    int target_color = colorTable[y][cr][cb];

                    if(target_color != colorToFillWith){
                        continue;
                    }

                    //this is an array of counters to track
                    //the frequency of colors occuring in the BOX around target
                    int[] neighbor_count = new int[POSSIBLE_COLORS];

                    int maxColor = 0; //stores the most frequent color
                    int maxVal = 0;//stores the frequency of that color

                    //now look through all the pixels BOX away from the target
					boolean island = true;
                    for(int ii = y-BOX; ii <= y +BOX && island; ii++){
                        for(int jj = cr -BOX; jj <= cr + BOX && island; jj++ ){
                            for(int kk = cb -BOX; kk <= cb+BOX && island; kk++){

                                //skip the center pixe, since it is the target
                                if(y==ii && cr == jj && cb ==kk){
                                    continue;
                                }

                                //grab a neighbor to the target, to test
                                int test_color = colorTable[ii][jj][kk];
								if (test_color == colorToFillWith) {
									island = false;
								}
                            }
                        }
                    }//done looking through the BOX

                    //lets see if we should assign the target something new:
					if (island) {
                        int[] yuv_to_fill = {y,cb,cr};
						update.addPixel(yuv_to_fill,colorToFillWith);
					}
                }
            }
        }//done looking through the whole table

        //push the changes
        pushUndo(update);

        modifyTableDirectly(update, Vision.GREY);

        System.out.println("Got rid of " + update.getSize() +
                           " islands of color " +
                           Vision.COLOR_STRINGS[colorToFillWith] );


        return update;
    }



    /******DEBUGGING****/

    public void fillTable(){
        //fill the table with bogus values
        for(int i = 0; i < uMax; i++){
            for(int j = 0; j < vMax; j++){
                for(int k = 0; k <  yMax; k++){
                    colorTable[i][j][k] = (byte)(k+1);
                }
            }
        }

    }


    /**
     * @return a String documenting information about the table,
     * namely the size of the Y, U, and V channels, and the
     * shift amounts (i.e. the amount to bitshift to change a standard
     * 256 entry into this size)
     */
    public String getInfo() {
        String s =
            "yMax: " + yMax + "\n" +
            "uMax: " + uMax + "\n" +
            "vMax: " + vMax + "\n" +
            "yShift: " + yShift + "\n" +
            "uShift: " + uShift + "\n" +
            "vShift: " + vShift + "\n";

        return s;
    }

    public void setSoftColors(boolean choice) {
        softColors = choice;
    }

    public boolean getSoftColors() {
        return softColors;
    }

    public String getFileName() {
        return fileName;
    }

    public void printTable(){

        for(int y = 0; y < yMax; y++){
            TOOL.CONSOLE.println("############"+y+"############");
            for(int u = 0; u < uMax; u++){
                for(int v = 0; v < vMax; v++){
                    TOOL.CONSOLE.print(colorTable[u][v][y]+" ");

                }
                TOOL.CONSOLE.println();
            }
        }

    }
}
