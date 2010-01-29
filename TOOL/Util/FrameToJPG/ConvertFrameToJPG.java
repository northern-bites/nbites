package TOOL.Util.FrameToJPG;

import TOOL.Data.File.*;
import TOOL.Data.*;
import java.util.Vector;
import java.util.Iterator;
import javax.imageio.ImageIO;
import java.io.IOException;
import java.io.File;
import TOOL.TOOLException;

/**
 * class ConvertFrameToJPG
 * 
 * This is intended as a tool that extracts images from .frm files to .jpg files
 * 
 * @author Octavian Neamtu
 * @ Team Northern Bites 2009
 *
 **/
class ConvertFrameToJPG{
    
    public static final String fileSeparator = System.getProperty("file.separator");

    public static void main(String[] args){
        
        if (args.length == 0 || args[0].compareTo("help") == 0){
            help();
            return;
        }

        String path = args[0];
        File checkPath = new File(path);
        if (!checkPath.exists()) {
            System.out.println("Could not find directory at specified path!");
            return;
        }
            
        FileSet fileSet = new FileSet(null, 0, path, "name");

        String dir;
        if (args.length > 1)
            dir = args[1];
        else 
            dir = "jpegs";

        File dirPath = new File(path + fileSeparator + dir);
        if (!dirPath.exists())
            dirPath.mkdir();
        
        String imageName;
        if (args.length > 2)
            imageName = args[2];
        else
            imageName = "image";
        
        for (int k = 0; k < fileSet.size(); k++){
            try{
            fileSet.load(k);
            }catch(TOOLException e){
                System.out.println("Could not load frame");
                System.out.println(e.getMessage());
            }

            Frame f = fileSet.get(k);
            try{
                ImageIO.write(f.image().createImage(), "jpg", 
                              new File(path + dir + fileSeparator + imageName + f.index() + ".jpg"));
            }catch(IOException e){
                System.out.println("Could not write to file");
                System.out.println(e.getMessage());
            }

            try{
                fileSet.unload(k);
            }catch(TOOLException e){
                System.out.println("Could not unload frame");
                System.out.println(e.getMessage());
            }
        }
    }

    /** 
     * function help
     *
     * Prints out usage
     * @params none
     * @returns void
     **/

    public static void help(){
        System.out.println("Usage:\n[run] [source frame folder path] [destination folder path relative to the source frame folder - default \"jpegs\"] [image name tag - default \"image\"]");
    }

}