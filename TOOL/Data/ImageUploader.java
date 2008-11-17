package edu.bowdoin.robocup.TOOL.Data;

import java.io.IOException;
import edu.bowdoin.robocup.TOOL.Data.ClientHttpRequest;
import java.util.Vector;
import edu.bowdoin.robocup.TOOL.Image.TOOLImage;
import java.nio.ByteBuffer;
import java.io.InputStream;


public class ImageUploader {
    
    ClientHttpRequest chr;
    Vector<TOOLImage> images;
    
    
    public ImageUploader() throws IOException {
        images = new Vector<TOOLImage>();
        chr = new ClientHttpRequest("https://robocup.bowdoin.edu/depot/uploadFiles.php");
    }
    
    
    /**
    * addImage takes a TOOLImage, the file name, and the path and adds it to the request that is about to be POSTed out.
    * @param t TOOLImage that we're going to send
    * @param name File name that t will be stored as on server
    * @param path File path where t will be stored on server
    */
    public void addImage(TOOLImage t, String name, String path) throws IOException {
        images.add(t);
        ByteBuffer buf = ByteBuffer.wrap(t.getByteArray());
        InputStream is = newInputStream(buf);
        chr.setParameter("file" + images.size(), name, is);
        chr.setParameter("path" + images.size(), path);
    }
    
    
    /** Please note that I found this method on 
    * http://jug.org.ua/wiki/display/JavaAlmanac/Creating+a+Stream+on+a+ByteBuffer
    * and consequently don't quite understand what it does.  Enjoy.
    */
    public static InputStream newInputStream(final ByteBuffer buf) {
        return new InputStream() {
            public synchronized int read() throws IOException {
                if (!buf.hasRemaining()) {
                    return -1;
                }
                return buf.get();
            }
    
            public synchronized int read(byte[] bytes, int off, int len) throws IOException {
                // Read only what's left
                len = Math.min(len, buf.remaining());
                buf.get(bytes, off, len);
                return len;
            }
        };
    }
    
    public int getSize() {
        return images.size();
    }
    
    
    /**
    * send() sends all of the data across to the server, and returns the InputStream that we get back.
    * Depending on what the InputStream contains, we can determine the success of our request.
    */
    public InputStream send() throws IOException {
        chr.setParameter("numFiles", Integer.toString(getSize()));
        return(chr.post());
    }
}
