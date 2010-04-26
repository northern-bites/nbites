
import java.io.File;

import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.awt.Frame;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.util.Iterator;
import java.util.LinkedList;


/**
 * Class for converting yuv ordered color tables into uvy tables.
 * Will read in the given table and write it out to the specified destination
 * in the new ordering.
 */
class ConvertTable {

    //current table sizes
    public static int YSHIFT = 1;
    public static int USHIFT = 1;
    public static int VSHIFT = 1;

    public static int YMAX = (int)(256>>YSHIFT);
    public static int UMAX = (int)(256>>USHIFT);
    public static int VMAX = (int)(256>>VSHIFT);

	byte[][][] yuv_colorTable;
	byte[][][] uvy_colorTable;

	/**
	 * Reads in a file in yuv order.
	 */
	public void loadDefFromFile(String fileName)
	{
        int byteCount = 0 ;
        FileInputStream fis = null;
        long startTime = System.currentTimeMillis();

        System.out.print("Loading Color Def File: " + fileName + "... ");

        try {
            fis = new FileInputStream(fileName);
        } catch (FileNotFoundException e) {
            System.out.println("table load failed.");
            return;
        }


        byte[][][] newTable = new byte[YMAX][UMAX][VMAX];

        byte[] buffer = new byte[YMAX*UMAX*VMAX];
        int byteIndex = 0;
        //read in the whole table into a really long byte array
        try {
            fis.read(buffer);
            byteCount += YMAX*UMAX*VMAX;
        } catch (IOException e) {
            System.out.println("table load failed.");
            return;
        }

        //parse the byte buffer into the new table
        for(int y=0; y<YMAX; y++) {
            for(int u=0; u<UMAX; u++) {
                for(int v=0; v<VMAX; v++){
                    newTable[y][u][v] = buffer[byteIndex];
                    ++byteIndex;
                }
            }
        }


        try {
            fis.close();
        } catch (IOException e) {
            System.out.println("table load failed.");
            return;
        }

		yuv_colorTable = newTable;

        double elapsedTime =
            (double)(System.currentTimeMillis() - startTime)/1000;
        System.out.println(byteCount/1024 + " KBs read in " +
                             elapsedTime+"Seconds");

    }

	public void convertTableToUVY()
	{
        for(int y=0; y<YMAX; y++)
            for(int u=0; u<UMAX; u++)
                for(int v=0; v<VMAX; v++){
					uvy_colorTable[u][v][y] = yuv_colorTable[y][u][v];
				}
	}

	private void writeUVYByteArray(byte[] dest)
	{
        int byteIndex = 0;
        for(int u=0; u<UMAX; u++)
            for(int v=0; v<VMAX; v++)
                for(int y=0; y<YMAX; y++){
                    dest[byteIndex] = uvy_colorTable[u][v][y];
                    byteIndex++;
                }
    }


    public void saveDefToFile(String fileName){

        int byteCount = 0 ;
        FileOutputStream fos=null;
        boolean savedOk = true;

        System.out.println("Saving Color Defs to file: " + fileName + "... ");

        try {
            fos = new FileOutputStream(fileName,true);
        }
        //catch (Exception e) {
        catch (FileNotFoundException e) {
            System.out.println(e.getMessage());
            savedOk = false;
            System.out.println(" save failed.");
			System.exit(0);
        }


        byte[] buffer = new byte[YMAX*UMAX*VMAX];
        writeUVYByteArray(buffer);

        try {
            fos.write(buffer);
            byteCount+=buffer.length;
        } catch (IOException e) {
            System.out.println(e.getMessage());
            savedOk = false;
        }

        try {
            fos.close();
        } catch (IOException e) {
            System.out.println(e.getMessage());
            savedOk = false;
        }

        if(savedOk){
            System.out.println(byteCount/1024 + " KBs written");
            System.exit(0);
        }
        else{
            System.out.println(" save failed.");
            System.exit(0);
        }
    }

	public ConvertTable()
	{
		uvy_colorTable = new byte[UMAX][VMAX][YMAX];
		yuv_colorTable = new byte[YMAX][UMAX][VMAX];
	}

	public static void main(String[] args)
	{
		ConvertTable ct = new ConvertTable();
		ct.loadDefFromFile(args[0]);
		ct.convertTableToUVY();
		ct.saveDefToFile(args[1]);
	}
}