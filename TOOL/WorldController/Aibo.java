package TOOL.WorldController;

import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

/**
 * Creators: Eric Chown, Yi Zhuang
 * Date: Aug 10, 2005
 * Time: 6:30:02 PM
 *
 * The abstract Aibo class representing an Aibo of any type.
 * You can not create objects of this class, but you can write
 * classes that extends this class.
 *
 * @author Eric Chown, Yi Zhuang, Jeremy Fishman
 * @version 1.0
 */
public  class Aibo {
    public static final int FULL_IMAGE_WIDTH = 208;
    public static final int FULL_IMAGE_HEIGHT = 160;
    public static final int GRIDW = 480;
    public static final int GRIDH = 320;
    public static final int CONVERSION = 48;
    public static final int BLANK = 32;
    public static final int PIX = 3;
    public static final int FIELDOBJECTS = 7;

    public static  int IMAGE_WIDTH=FULL_IMAGE_WIDTH;
    public static  int IMAGE_HEIGHT=FULL_IMAGE_HEIGHT;

    public static final int WHITE = 0;
    public static final int ORANGE = 1;
    public static final int BLUE = 2;
    public static final int GREEN = 3;
    public static final int YELLOW = 4;
    public static final int PINK = 5;
    public static final int BLACK = 6;
    public static final int GRAY = 7;
    public static final int RED = 8;
    public static final int DARKBLUE = 9;
    public static final int LINEWHITE = 10;
    public static final int UNDEF = 11;
    public static final int BOTH = 12;
    public static final int AIBOERS7 = 0;
    public static final int AIBO220 = 1;
    
    public final static int INITIAL = 0;
    public final static int CONNECTED = 1;
    public final static int CONNECTION_OPEN = 2;
    public final static int UNKNOWN_HOST = 3;
    public final static int IO_ERROR = 4;
    public final static int AIBO_UNKNOWN = 5;
    public final static int HOST_TIMEOUT = 6;
    public final static int WRONG_TYPE = 7;
    public final static int NO_OPEN_PORTS = 8;
    public final static int NOT_CONNECTED = 9;
    public final static String[] ERROR_MSGS = {
        "Have not connected to host",
        "Connected",
        "Connection is still open",
        "Unknown host",
        "Could not get I/O for host",
        "Aibo type not known/supported",
        "Connection timed out with host",
        "Wrong Aibo type assumed",
        "No available ports on host",
        "Not connected to host",
    };

    /**
     * A filed indicating the verbosity of the output.
     * true - verbose output
     * false - concise output
     */
    public boolean verboseOutput = false;

    /**
     * An integer indicating the current status of the connection to the aibo.
     */
    protected int status;
    protected boolean connecting;

    /**
     * The period of time aiboConnect would wait for, for a reply after sending a command.
     * The worse the network, the higher value this variable shouuld be.
     * It is in milliseconds and it is set to 100 on default.
     */
    public static final int WAIT_TIME = 500;
    public static final int SO_TIMOUT = 15000;

    /*
     * The basic commands available in AiboConnect
     */
    public static final byte[] AM_READY = {'a','m','R','e','a','d','y','M','o','t','i','o','n','\0'};
    public static final String AM_READY_STR = "amReadyMotion";
    public static final String CLOSE_STR = "close";
    public static final byte[] GET_RAW_IMAGE = {'s','e','n','d','I','m','a','g','e','\0'};
    public static final String GET_RAW_STR = "sendImage";
    public static final byte[] GET_THRESHOLDED_IMAGE = {'s','e','n','d','T','h','r','e','s','h','o','l','d','e','d','\0'};
    public static final String GET_THRESH_STR = "sendThresholded";
    public static final String GET_RLE_STR = "sendRLE";
    public static final String GET_XYH_STR = "sendXYH";
    public static final String GET_OBJECTS_STR = "sendObjects";
    public static final byte[] HELLO = {'h','e','l','l','o','?','\0'};
    public static final String HELLO_STR = "hello?";
    public static final byte[] CEASE = {'c','e','a','s','e','M','o','t','i','o','n','\0'};
    public static final String CEASE_STR = "ceaseMotion";
    public static final byte[] NUMBERS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    public static final byte[] MID = {'r','e','s','o','l','u','t','i','o','n','S','e','t',' ','1','\0'};
    public static final String MID_STR = "resolutionSet 1";
    public static final byte[] HIGH = {'r','e','s','o','l','u','t','i','o','n','S','e','t',' ','0','\0'};
    public static final String HIGH_STR = "resolutionSet 0";
    public static final byte[] LOW = {'r','e','s','o','l','u','t','i','o','n','S','e','t',' ','2','\0'};
    public static final String LOW_STR = "resolutionSet 2";
    public static final byte[] INDOOR = {'c','a','m','e','r','a','S','e','t',' ','1','\0'};
    public static final String INDOOR_STR = "cameraSet 1";
    public static final byte[] OUTDOOR = {'c','a','m','e','r','a','S','e','t',' ','2','\0'};
    public static final String OUTDOOR_STR = "cameraSet 2";
    public static final byte[] FL = {'c','a','m','e','r','a','S','e','t',' ','3','\0'};
    public static final String FL_STR = "cameraSet 2";
    public static final byte[] LOWLIGHT = {'c','a','m','e','r','a','S','e','t',' ','4','\0'};
    public static final String LOWLIGHT_STR = "cameraSet 4";
    public static final byte[] MIDLIGHT = {'c','a','m','e','r','a','S','e','t',' ','5','\0'};
    public static final String MIDLIGHT_STR = "cameraSet 5";
    public static final byte[] HILIGHT = {'c','a','m','e','r','a','S','e','t',' ','6','\0'};
    public static final String HILIGHT_STR = "cameraSet 6";
    public static final byte[] SLOW = {'c','a','m','e','r','a','S','e','t',' ','7','\0'};
    public static final String SLOW_STR = "cameraSet 7";
    public static final byte[] MED = {'c','a','m','e','r','a','S','e','t',' ','8','\0'};
    public static final String MED_STR = "cameraSet 8";
    public static final byte[] FAST = {'c','a','m','e','r','a','S','e','t',' ','9','\0'};
    public static final String FAST_STR = "cameraSet 9";
    public static final byte[] RELOAD = {'r','e','l','o','a','d','T','a','b','l','e','\0'};
    public static final String RELOAD_STR = "reloadTable";
    public static final byte[] GEORGE_GAIT = {'s','w','a','p','G','a','i','t',' ','1',' ','5','0','\0'};
    public static final String CLOSE_0_STR = "close 1";

    public static final String GEORGE_GAIT_STR = "swapGait 1 50";
    public static final String GAIT_STR = "sGait";
    public static final String MOTION_STR = "motion";
    public static final byte[] NEUT_HEAD = {'n','e','u','t','H','e','a','d','\0'};
    public static final String NEUT_HEAD_STR = "neutHead";
    public static final byte[] DEBUG_AIBO_CONNECT = {'d','A','i','b','o','C','o','n','n','e','c','t','\0'};
    public static final String DEBUG_AIBO_CONNECT_STR = "debugAiboConnect";
    public static final byte[] DEBUG_RECEIVE_STRING = {'d','e','b','u','g','R','e','c','e','i','v','e','S','t','r','i','n','g','\0'};
    public static final String DEBUG_RECEIVE_STRING_STR = "debugReceiveString";
    public static final byte[] DEBUG_RECEIVE_CMD = {'d','e','b','u','g','R','e','c','e','i','v','e','C','m','d','\0'};
    public static final String DEBUG_RECEIVE_CMD_STR = "debugReceiveCmd";
    public static final byte[] DEBUG_RECEIVE_MOTION = {'d','e','b','u','g','R','e','c','e','i','v','e','M','o','t','i','o','n','\0'};
    public static final String DEBUG_RECEIVE_MOTION_STR = "debugReceiveMotion";
    public static final byte[] DEBUG_RECEIVE_HEAD = {'d','e','b','u','g','R','e','c','e','i','v','e','H','e','a','d','\0'};
    public static final String DEBUG_RECEIVE_HEAD_STR = "debugReceiveHead";

    private static Font FONT = new Font("Lucida Grande",0,20);
    
    public static final int START_PORT = 23000;
    public static final int NUM_PORTS = 4;
    public static final int TOPSIZE = 40;
    public static final int PIXELSIZE = 2;

    protected String hostName;
    protected int model, res, wb, gain, shutter, port;
    protected int dogX, dogY, dogH;
    
    //The class that handles the physical implementation of aibo communication
    protected AiboCommunicator comm;

    /**
     * the size of an image in bytes
     */
    protected int totalBytes = IMAGE_WIDTH * IMAGE_HEIGHT *3;
    /**
     * the size of a thresholded image in bytes
     */
    protected  int totalThresholdedBytes = (IMAGE_WIDTH * IMAGE_HEIGHT * 4) / 8;

    /**
     * a color table used by the thresholding system of the aibo
     */
    public static Color[] COLORS=new Color[16];

    /**
     * The maximum number of bytes the computer can read from the network
     * at one time. As the aibo can send at most 65536 bytes at a time, this
     * value should not be above 65536
     */
    public static final int MAX_BYTES_THAT_CAN_BE_READ_IN_ONE_TIME = 65536;
    public static final int MAX_BUFFER_SIZE = 110000;

    private byte[] tempBytes = new byte[MAX_BYTES_THAT_CAN_BE_READ_IN_ONE_TIME];
    private ByteArrayOutputStream byteArray = new ByteArrayOutputStream();

    //rawImage stores the raw bytes representing an Image sent back by an Aibo
    //Every three bytes represent a pixel
    private byte[] rawImage = new byte[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

    private byte[] rawObjects = new byte[280];
    public int[] objects = new int[280];

    //rawThresholdedImage stores the raw bytes representing an Thresholded Image sent back by an Aibo
    //Every three bits represent a pixel
    private byte[] rawThresholdedImage = new byte[(IMAGE_WIDTH * IMAGE_HEIGHT * 3) / 8];

    /**
     * an array representing the YUV image displayed on the screen
     */
    public byte[][][] image = new byte[IMAGE_WIDTH][IMAGE_HEIGHT][3];

    /**
     * an array representing the YUV image displayed on the screen after color thresholding
     */
    public int[][] thresholdedImage = new int[IMAGE_WIDTH][IMAGE_HEIGHT];

    /*
     * A window for displaying images
     */
    private JFrame imageWindow;

    private int baseHeight = IMAGE_HEIGHT;
    private int baseWidth = IMAGE_WIDTH;
    private int pixelSize = 6;
//     private boolean working = false;

    /**
     * constructor of the class.
     */
    public Aibo() {
        //Initialze color table for thresholded image
        COLORS[0]=Color.white;
        COLORS[1]=Color.orange;
        COLORS[2]=Color.blue;
        COLORS[3]=Color.green;
        COLORS[4]=Color.yellow;
        COLORS[5]=Color.pink;
        COLORS[6]=Color.black;
        COLORS[7]=Color.gray;
        COLORS[8]=Color.red;
        COLORS[9]=Color.blue.darker();
        // the next batch are colors we don't actually use
        COLORS[10] = new Color(100, 100, 100);
        COLORS[11] = Color.magenta;
        COLORS[12] = new Color(200, 200, 200);
        COLORS[13] = new Color(200, 200, 200);
        COLORS[14] = new Color(200, 200, 200);
        COLORS[15] = Color.gray;
        
        status = INITIAL;
        comm = new AiboCommunicator();
    }

    /**
     * Set the verbosity of the output
     * @param verboseOutput  true - verbose output, false - concise output
     */
    public void setVerboseOutput(boolean verboseOutput) {
        this.verboseOutput = verboseOutput;
    }

    public void updateByteInfo() {
        totalBytes = IMAGE_WIDTH * IMAGE_HEIGHT *3;
        totalThresholdedBytes = (totalBytes +IMAGE_WIDTH * IMAGE_HEIGHT) / 8;
        rawThresholdedImage = new byte[totalThresholdedBytes];
        rawObjects = new byte[600];
    }

    /**
     * In JAVA, a byte is always considered as signed when converted
     * to another type (-127 to 127).
     * But aibo use bytes to represent YUV values (0-255).
     * This method converts a signed java byte to unsigned 0-255
     *
     * @param b  The Java byte to be converted
     * @return   An integer containing the unsigned value
     */
    public static int signedByteToUnsigned(byte b){
        if (b>0)
            return b;
        return ((int)b+255);
    }

    public int unsignedByteToInt(byte b) {
        return (int) b & 0xFF;
    }
    
    public boolean isConnected() {
        return (status == CONNECTED);
    }
    
    public int getStatus() {
        return status;
    }
    
    public AiboCommunicator getComm() {
        if (comm == null) {
            System.err.println("Comm was null? Rebuiding");
            comm = new AiboCommunicator();
        }
        return comm;
    }
    
    /**
     * get connection to the aibo specified
     */
    public int connectTo(String hostName) {
        this.hostName = hostName;
        port = START_PORT;
        connecting = true;
    
        for (int i = 0; i<NUM_PORTS && connecting; i++) {
            connectDog(port);
            if (status == CONNECTED) {
                aiboTypeCheck();
                break;
            }else if (!connecting || status == UNKNOWN_HOST)
                break;
            else if (i == NUM_PORTS-1) {
                status = NO_OPEN_PORTS;
                System.err.println(ERROR_MSGS[status] + " " + hostName);
            }
            else
                port++;
        }
        connecting = false;
        return status;
    }

    protected int connectDog (int tryPort) {
        try{
            AiboCommunicator temp = new AiboCommunicator(hostName,tryPort);
            comm.close();
            comm = temp;
        }catch(UnknownHostException e){
            if (connecting) {
                status = UNKNOWN_HOST;
                error(e);
                System.err.println(ERROR_MSGS[status] + " " + hostName);
            }
        }catch(IOException e){
            if (connecting) {
                status = IO_ERROR;
                System.err.println(ERROR_MSGS[status] + " " + hostName +
                        ":" + tryPort);
            }
        }
        return status;
    }
    
    public boolean isConnecting() {
        return connecting;
    }
    
    public void stopConnecting() {
        connecting = false;
    }

    /**
     * Close the connection to the current connected Aibo
     * Do nothing if connectionStatus is 0 (not connected)
     */
    public void disconnect() {
        if(status == INITIAL)
            return;
        
	comm.send(CLOSE_STR);

	status = INITIAL;
        if (comm != null)
            comm.close();
    }

    /**
     * send a command to aibo and get its response
     * "sendImage" command should not be sent using the method
     * @param command    the command to send (coded in a byte sequence)
     * @return  a sequence of bytes returned by the aibo
     */

    public String sendCommand(String command) {
        Handler<String> h = new Handler<String>() {
            public String handle(BufferedReader data) throws IOException{
                return data.readLine();
            }
        };
        return comm.sendAndRecv(command,h);
    }
    
    /**
     * Creates a window for displaying images.
     */

    public void createImageWindow() {
        imageWindow = new JFrame("AIBOCONNECT");
        imageWindow.setSize(IMAGE_WIDTH*2,IMAGE_HEIGHT*PIX);
        imageWindow.setVisible(true);
        Container contentPane = imageWindow.getContentPane();
        contentPane.setSize(IMAGE_WIDTH*PIX,IMAGE_HEIGHT*PIX+10);

    }

    /**
     * Creates a window for displaying images with user specified dimensions.
     */

    public void createImageWindow(int width, int height) {
        imageWindow = new JFrame("AIBOCONNECT");
        imageWindow.setSize(width, height);
        imageWindow.setVisible(true);
        Container contentPane = imageWindow.getContentPane();
        contentPane.setSize(width,height+10);
    }

    /**
     * Figure out what kind of Aibo we're connected to and adjust accordingly
     */
    public void aiboTypeCheck()  {
        if(status == INITIAL)
            return;

        if (verboseOutput)
            System.out.println("requesting for aibo type");
            
        Handler<Integer> h = new Handler<Integer>() {
            public Integer handle(BufferedReader data) throws IOException {
                String s = data.readLine();
                if (s==null || s.length()<9)
                    return -1;
                // we've just read a string of single digits as characters
                model = (int)s.charAt(0) - CONVERSION;
                res = (int)s.charAt(2) - CONVERSION;
                wb = (int)s.charAt(4) - CONVERSION;
                gain = (int)s.charAt(6) - CONVERSION;
                shutter = (int)s.charAt(8) - CONVERSION;
        //                 System.out.println("Settings are: "+model+" "+res+" "+wb+" "+gain+" "+shutter);
                
                if (verboseOutput)
                    System.out.println("aibo: "+ s);
                
                if(model == AIBOERS7){
        //                     System.out.println("Connected to an Aibo ERS-7");
                    IMAGE_WIDTH = 208;
                    IMAGE_HEIGHT = 160;
                    baseHeight = IMAGE_HEIGHT;
                    baseWidth = IMAGE_WIDTH;
                }else if (model == AIBO220) {
        //          System.out.println("Connected to an Aibo 220");
                    IMAGE_WIDTH=176;
                    IMAGE_HEIGHT=144;
                    baseHeight = IMAGE_HEIGHT;
                    baseWidth = IMAGE_WIDTH;
                } else {
                    return -1;
        //                      System.err.println("Sorry, Only ERS-7s and 220s are supported by this aiboConnect");
        //                      System.exit(1);
                }
                if (res != 0) {
                    highResolution();
                    res = 0;
                }
                updateByteInfo();
                return model;
            }
        };
        comm.sendAndRecv(HELLO_STR,h);
        
    }

    /**
     * get the current image from aibo
     * do nothing if not connected to an aibo
     * @return  bytes losts. Frame should be discarded if this value is not 0
     */
    public int getImage() {
        if (!isConnected())
            return -1;
//         while (working) {}
//         working = true;
        if (status == INITIAL)
            return -1;

        if (verboseOutput)
            System.out.println("requesting image");
        
        Handler<Integer> h = new Handler<Integer>(true) {
            public Integer handle(ByteArrayInputStream data) {
                int bytesReceived = data.read(rawImage,0,totalBytes);
                int bytesLost = totalBytes - bytesReceived;
                
                if((bytesLost !=0) && verboseOutput)
                    System.out.println(bytesLost + " bytes were lost while " +
                                       "transferring image.");
//                 else if (bytesLost == 0)
                parseRawImage();
                return bytesLost;
            }
        };
        if (!isConnected())
            return -1;
        try {
            return comm.sendAndRecv(GET_RAW_STR,h);
        }catch (NullPointerException e) {
            return -1;
        }
    }


    /**
     * convert a rawImage byte sequence into the image[IMAGE_HEIGHT][IMAGE_WIDTH][3] YUV array.
     * Should be called after getImage() and before drawImage(JFrame).
     */
    public void parseRawImage(){
        int i,j,k,counter=0;
        for (i=0; i<IMAGE_HEIGHT; i++)
            for(j=0; j<IMAGE_WIDTH; j++)
                for(k=0; k<3; k++)
                    image[j][i][k]=rawImage[counter++];

        if (verboseOutput) {
            System.out.println("Image array created");
        }
    }

    /**
     * get the current image from aibo
     * do nothing if not connected to an aibo
     * @return  bytes losts. Frame should be discarded if this value is not 0
     */
    public int getObjects() {
        if (!isConnected())
            return -1;
        int bytesLost = 0;
        if(status == 0) return -1;

        if (verboseOutput)
                System.out.println("requesting objects");
        
        Handler<Integer> h = new Handler<Integer>(true) {
            public Integer handle(ByteArrayInputStream data) {
                int bytesReceived = data.read(rawObjects,0,rawObjects.length);//totalBytes);
                int bytesLost = 0;
                if (verboseOutput)
                    System.out.println("Read " + bytesReceived + " byte/s " + 
                                       "of object data");
                parseRawObjects();
                return bytesLost;
            }
        };
        return comm.sendAndRecv(GET_OBJECTS_STR,h);
    }


    /**
     * convert a rawImage byte sequence into the image[IMAGE_HEIGHT][IMAGE_WIDTH][3] YUV array.
     * Should be called after getImage() and before drawImage(JFrame).
     */
    public void parseRawObjects(){
        int o = 0;
        int i = 0;
        int a, b, c;
        while (o < FIELDOBJECTS * 4) {
            a = rawObjects[i++];
            b = rawObjects[i++];
            c = rawObjects[i++];
            i++;
            objects[o] = (a - CONVERSION) * 100 + (b - CONVERSION) * 10 + (c - CONVERSION);
            o++;
        }
    }

    public void drawObjects(JFrame drawingWindow) {
        Container contentPane = drawingWindow.getContentPane();
        Graphics g = contentPane.getGraphics();
        int w = drawingWindow.getWidth() / IMAGE_WIDTH;
        int h = drawingWindow.getHeight() / IMAGE_HEIGHT;
        g.setColor(Color.black);
        g.clearRect(0,0,contentPane.getWidth(),contentPane.getHeight());

        int i,j;
        for (i=0; i<FIELDOBJECTS; i++) {
            if (objects[i * 4] > 0) {
                switch(i) {
                    case 0: g.setColor(Color.yellow);
                        break;
                    case 1: g.setColor(Color.blue);
                        break;
                    case 2: g.setColor(Color.pink);
                        break;
                    case 3: g.setColor(Color.green);
                        break;
                    case 4: g.setColor(Color.red);
                        break;
                    case 5: g.setColor(Color.magenta);
                        break;
                    case 6: g.setColor(Color.orange);
                        break;
                }
                g.fillRect(objects[i*4] * w, objects[i*4 + 1] * h, 
                        objects[i*4 + 3] * w, objects[i*4 + 2] * h);
            }
        }
    }

    /**
     * draw image onto a JFrame object
     * @param drawingWindow the JFrame to be painted. should be larger than IMAGE_WIDTH*2, IMAGE_HEIGHT*2
     */
    public void drawImage(JFrame drawingWindow){
        Container contentPane = drawingWindow.getContentPane();
        Graphics g = contentPane.getGraphics();
        int w = drawingWindow.getWidth() / IMAGE_WIDTH;
        int h = drawingWindow.getHeight() / IMAGE_HEIGHT;
        g.setColor(Color.black);
        g.fillRect(0,0,contentPane.getWidth(),contentPane.getHeight());

        int i,j;
        for (i=0; i<IMAGE_HEIGHT; i++) {
            for(j=0; j<IMAGE_WIDTH; j++) {
                g.setColor(new Color(signedByteToUnsigned(image[j][i][1]),
                        signedByteToUnsigned(image[j][i][0]),signedByteToUnsigned(image[j][i][2])));
                g.fillRect(j*w,i*h,w,h);
            }
        }
        if (verboseOutput) {
            System.out.println("Image successfully drawn to screen");
            System.out.println();
        }
    }

    /**
     * Gets and draws the thresholded image.  Provided as a programming convenience.
     */
    public void getAndDrawThresholdedImage() {
        if (imageWindow == null)
            createImageWindow();
        int count = getThresholdedImage(1);
        if (count == 0)
        drawThresholdedImage(imageWindow);
    }

    /**
     * Gets and draws the unthresholded image.  Provided as a programming convenience.
     */
    public void getAndDrawImage() {
        if (imageWindow == null)
            createImageWindow();
        int count = getImage();
        if (count == 0)
            drawImage(imageWindow);
    }

    /**
     * get the current thresholded image (THRESH OR RLE) from aibo
     * do nothing if not connected to an aibo
     * @return  bytes losts. Frame should be discarded if this value is not 0
     */
    public int getThresholdedImage(int view) {
        if (!isConnected())
            return -1;
        int bytesLost = 0;
        if(status != CONNECTED)
            return -1;
        
        if (verboseOutput)
            System.out.println("requesting thresholded image");
        
        Handler<Integer> h = new Handler<Integer>(true) {
            public Integer handle(ByteArrayInputStream data) {
                int bytesLost = totalThresholdedBytes;
		int bytesReceived = data.available();
                bytesLost -= bytesReceived;
                data.read(rawThresholdedImage,0,rawThresholdedImage.length);
                if (verboseOutput) {
                    System.out.println(bytesReceived + " bytes received " + 
                                       "totally. " + bytesLost + "byte/s lost");
                    System.out.println("Thresholded image raw bytes received");
                }
                if (bytesLost == 0)
                    parseThresholdedRawImage();
                return bytesLost;
            }
        };
        if (view==1)
            return comm.sendAndRecv(GET_THRESH_STR,h);
        else if (view == 2)
            return comm.sendAndRecv("sendRLE",h);
        else {
            System.err.println("not using getThresholdedImage properly");
            return -1;
        }
    }

    // gets the bit value at a certain position in the given byte.
    private int readBit (byte b, int pos) {
        return (b >> (pos)  ) & 1;
    }

    /**
     * convert a rawThresholdedImage byte sequence into the thresholded image array.
     * Should be called after getThresholdedImage() and before drawThresholdedImage(JFrame).
     */
    public void parseThresholdedRawImage(){
        int currentBit = 0;
        int currentByte = 0;
        int i, j, shift;
        int tempByte;

        for (j = 0; j < IMAGE_HEIGHT; j++) {
            for (i = 0; i < IMAGE_WIDTH; i++) {
                tempByte = 0;
                for (shift = 0; shift < 4 && currentByte < rawThresholdedImage.length; shift++) {
                    tempByte = (tempByte | (readBit(rawThresholdedImage[currentByte],
                            currentBit) << shift));
                    currentBit++;
                    if (currentBit == 8) {
                        currentBit = 0;
                        currentByte++;
                    }
                }
                thresholdedImage[i][j] = tempByte;
            }
        }
    }

    /**
     * draw the thresholded image onto a JFrame object
     * @param drawingWindow the JFrame to be painted. should be larger than IMAGE_WIDTH*2, IMAGE_HEIGHT*2
     */
    public void drawThresholdedImage(JFrame drawingWindow){
        Container contentPane = drawingWindow.getContentPane();
        Graphics g = contentPane.getGraphics();
        int w = drawingWindow.getWidth() / IMAGE_WIDTH;
        int h = drawingWindow.getHeight() / IMAGE_HEIGHT;
        //g.setColor(Color.blue);
        //g.clearRect(0,0,IMAGE_WIDTH * 3, IMAGE_HEIGHT * 3);

        int i,j;
        for (i=0; i<IMAGE_HEIGHT; i++) {
            for(j=0; j<IMAGE_WIDTH; j++){
        //System.out.print(thresholdedImage[j][i]+" ");
        /*if (thresholdedImage[j][i] < 10) {
        System.out.println("non "+thresholdedImage[j][i]+" "+j+" "+i);
        }*/
                g.setColor(COLORS[thresholdedImage[j][i]]);
                g.fillRect(j*w,i*h,w,h);
            }
    //System.out.println(" ");
        }
        if (verboseOutput) {
            System.out.println("Image successfully drawn to screen");
            System.out.println();
        }
    }

    /**
     * draw a rected angle on the thresholded image
     * @param x upperleft x
     * @param y upperleft y
     * @param width  width of the rectangle
     * @param height  height of the rectangle
     * @param color   color of the rectangle, must be between 0-6
     */
    public void drawRectOnThresholdedImage(int x,int y,int width,int height, int color){
        if( (x<0) || (y<0) || (width<0) || (height<0) ){
            System.err.println("drawRect does not accept negative arguments");
            System.exit(1);
        }

        if(color<0 || color >6){
            System.err.println("color must be between 0-6");
            System.exit(1);
        }

        if(  ((x+width)>IMAGE_WIDTH) || (y+height)>IMAGE_HEIGHT ) {
            System.err.println("x plus width must not exceed image width (208)");
            System.err.println("y plus height must not exceed image hight (160)");
            System.err.println("or you will be drawing on the outside of the window");
            System.exit(1);
        }

        int y_plus_height = y+height;
        int x_plus_width  = x+width;
        for (int i=x; i< x_plus_width; i++){
            thresholdedImage[i][y]=color;
            thresholdedImage[i][y_plus_height]=color;
        }

        for (int j=y; j< y_plus_height; j++){
            thresholdedImage[x][j]=color;
            thresholdedImage[x_plus_width][j]=color;
        }

    }

    /**
     * draw a rected angle on the thresholded image
     * @param x upperleft x
     * @param y upperleft y
     * @param width  width of the rectangle
     * @param height  height of the rectangle
     */
    public void drawRectOnImage(int x,int y,int width,int height){
        if( (x<0) || (y<0) || (width<0) || (height<0) ){
            System.err.println("drawRect does not accept negative arguments");
            System.exit(1);
        }

        if(  ((x+width)>IMAGE_WIDTH) || (y+height)>IMAGE_HEIGHT ) {
            System.err.println("x plus width must not exceed image width (208)");
            System.err.println("y plus height must not exceed image hight (160)");
            System.err.println("or you will be drawing on the outside of the window");
            System.exit(1);
        }

        int y_plus_height = y+height;
        int x_plus_width  = x+width;
        for (int i=x; i< x_plus_width; i++) {
            image[i][y][0] = 0;
            image[i][y][1] = 0;
            image[i][y][2] = 0;
            image[i][y_plus_height][0]=0;
            image[i][y_plus_height][1]=0;
            image[i][y_plus_height][2]=0;
        }

        for (int j=y; j< y_plus_height; j++) {
            image[x][j][0]=0;
            image[x][j][1]=0;
            image[x][j][2]=0;
            image[x_plus_width][j][0]=0;
            image[x_plus_width][j][1]=0;
            image[x_plus_width][j][2]=0;
        }

    }

    /* Retrieve the width of the image at the aibo's current resolution.
     * @return width
     */

    public int getImageWidth() {
        return IMAGE_WIDTH;
    }

    /* Retrieve the height of the image at the aibo's current resolution.
     * @return height
     */

    public int getImageHeight() {
        return IMAGE_HEIGHT;
    }

    /* Tell the aibo to stop moving.
     */

    public void ceaseMotion() {
        comm.send(CEASE_STR);
    }

    /* Change the camera settings. 
       @param an int value specifying the new setting
     */
    
    public void changeCamera(int setting) {
//         byte[] temp;
        String temp;
        switch(setting) {
            case 1: temp = INDOOR_STR;
                break;
            case 2: temp = OUTDOOR_STR;
                break;
            case 3: temp = FL_STR;
                break;
            case 4: temp = LOWLIGHT_STR;
                break;
            case 5: temp = MIDLIGHT_STR;
                break;
            case 6: temp = HILIGHT_STR;
                break;
            case 7: temp = SLOW_STR;
                break;
            case 8: temp = MED_STR;
                break;
            default: temp = FAST_STR;
                break;
        }
        comm.send(temp);
//         try {
//             outStream.write(temp);
//         } catch (IOException e) {}
    }

    /* Put a new color table on the dog.  Right now it only works in conjunction with
     * running the "ftp_up" script.
     */

    public void reloadTable() {
        comm.send(RELOAD_STR);
//         try {
//             outStream.write(RELOAD);
//         } catch (IOException e) {}
    }

    /* Change to the low resolution camera setting.
     */

    public void lowResolution() {
        comm.send(LOW_STR);
//         try {
//             outStream.write(LOW);
//         } catch (IOException e) {}
        IMAGE_HEIGHT = baseHeight / 4;
        IMAGE_WIDTH = baseWidth / 4;
        updateByteInfo();
    }

    /* Change to the mid resolution camera setting.
     */

    public void midResolution() {
        comm.send(MID_STR);
//         try {
//             outStream.write(MID);
//         } catch (IOException e) {}
        IMAGE_HEIGHT = baseHeight / 2;
        IMAGE_WIDTH = baseWidth / 2;
        updateByteInfo();
    }

    /* Change to the high resolution camera setting.
     */

    public void highResolution() {
        comm.send(HIGH_STR);
//         try {
//             outStream.write(HIGH);
//         } catch (IOException e) {}
        IMAGE_HEIGHT = baseHeight;
        IMAGE_WIDTH = baseWidth;
        updateByteInfo();
    }

    /* Bundle some ints into a single command.  Rather tedious because we have to conver
     * to individual characters.
     */
    public byte[] bundleHeadCommand(int x, int y, int z, int m) {
        int count = 0;
        int size = 8; // 4 ints plus 3 spaces plus terminating character
        if (Math.abs(x) > 9) size++;
        if (Math.abs(y) > 9) size++;
        if (Math.abs(m) > 9) size++;
        if (Math.abs(z) > 9) size++;
        if (x < 0) size++;
        if (y < 0) size++;
        if (z < 0) size++;
        if (m < 0) size++;
        byte[] command = new byte[size];
        if (x < 0) {
            command[0] = '-';
            count++;
        }
        x = Math.abs(x);
        int div;
        if (x > 9) {
            div = x % 10;
            command[count] = NUMBERS[(x - div)/10];
            command[count+1] = NUMBERS[div];
            count += 2;
        } else {
            command[count] = NUMBERS[x];
            count++;
        }
        command[count] = ' ';
        count++;
        if (y < 0) {
            command[count] = '-';
            count++;
        }
        y = Math.abs(y);
        if (y > 9) {
            div = y % 10;
            command[count] = NUMBERS[(y - div)/10];
            command[count+1] = NUMBERS[div];
            count += 2;
        } else {
            command[count] = NUMBERS[y];
            count++;
        }
        command[count] = ' ';
        count++;
        if (z < 0) {
            command[count] = '-';
            count++;
            z = Math.abs(z);
        }
        if (z > 9) {
            div = z % 10;
            command[count] = NUMBERS[(z - div)/10];
            command[count+1] = NUMBERS[div];
            count += 2;
        } else  {
            command[count] = NUMBERS[z];
            count++;
        }
        command[count] = ' ';
        count++;
        if (m < 0) {
            command[count] = '-';
            count++;
            m = Math.abs(m);
        }       
        if (m > 9) {
            div = m % 10;
            command[count] = NUMBERS[(m - div)/10];
            count++;
            command[count] = NUMBERS[div];
            count++;
        } else {
            command[count] = NUMBERS[m];
            count++;
        } 
        command[count] = '\0';
        return command;
    } 

    /* Bundle some ints into a single command.  Rather tedious because we have to conver to individual characters.

       THIS IS JUST FOR WALK COMMANDS (z needs to be a decimal double)
     */
    public byte[] bundleWalkCommand(int x, int y, int z/*, int t*/) {
    int count = 0;
    int size = 9; // 3 ints plus 2 spaces plus one '.' plus terminating character
    if (Math.abs(x) > 9) size++;
    if (Math.abs(y) > 9) size++;
    if (z % 2 != 0) size++;
    //if (Math.abs(t) > 9) size++;
    if (x < 0) size++;
    if (y < 0) size++;
    //if (t < 0) size++;
    byte[] command = new byte[size];
    if (x < 0) {
        command[0] = '-';
        count++;
    }
    x = Math.abs(x);
    int div;
    if (x > 9) {
        div = x % 10;
        command[count] = NUMBERS[(x - div)/10];
        command[count+1] = NUMBERS[div];
        count += 2;
    } else {
        command[count] = NUMBERS[x];
        count++;
    }
    command[count] = ' ';
    count++;
    if (y < 0) {
        command[count] = '-';
        count++;
    }
    y = Math.abs(y);
    if (y > 9) {
        div = y % 10;
        command[count] = NUMBERS[(y - div)/10];
        command[count+1] = NUMBERS[div];
        count += 2;
    } else {
        command[count] = NUMBERS[y];
        count++;
    }
    command[count] = ' ';
    count++;
    if (z < 0) {
        command[count] = '-';
        count++;
        z = Math.abs(z);
    }
    command[count] = '.';
    count++;
    command[count] = NUMBERS[0];
    count++;
    switch (z) {
    case 1:
        command[count] = NUMBERS[0];
        command[count+1] = NUMBERS[5];
        count += 2;
        break;
    case 2:
        command[count] = NUMBERS[1];
        count++;
        break;
    case 3:
        command[count] = NUMBERS[2];
        count++;
        break;
    case 4:
        command[count] = NUMBERS[3];
        count++;
        break;
    case 5:
        command[count] = NUMBERS[4];
        count++;
        break;
    case 6:
        command[count] = NUMBERS[5];
        count++;
        break;
    case 7:
        command[count] = NUMBERS[6];
        count++;
        break;
    case 8:
        command[count] = NUMBERS[7];
        count++;
        break;
    case 9:
        command[count] = NUMBERS[8];
        count++;
        break;
    case 10:
        command[count] = NUMBERS[9];
        count++;
        break;
    }
        command[count] = '\0';
        return command;
    } 


    /* Send new values to the aibo's head.
     */

    public void sendHeadValues(int neck, int pan, int yaw, int mouth) {
        
        // force the values to be ok
        if (neck < -75) neck = -75;
        if (neck > 0) neck = 0;
        if (pan < -90) pan = -90;
        if (pan > 90) pan = 90;
        if (yaw < -15) yaw = -15;
        if (yaw > 45) yaw = 45;
        if (mouth < -55) mouth = -55;
        if (mouth > -3) mouth = -3;
        String msg = "heads " + 
            new String(bundleHeadCommand(neck,pan,yaw,mouth));
        comm.send(msg);
    }

    public void sendGaitCommand(int param, int value) {
//         byte[] command = bundleGaitCommand(param, value);
//         byte[] gait = new byte[6+command.length];
//         gait[0] = 's';
//         gait[1] = 'G';
//         gait[2] = 'a';
//         gait[3] = 'i';
//         gait[4] = 't';
//         gait[5] = ' ';
//         for (int i = 0; i < command.length; i++) {
//             gait[6 + i] = command[i];
//         }
        comm.send(GAIT_STR + " " + param + " " + value);
//         sendCommand(gait);
    }


    /* Make the aibo walk.  Use's Penn's basic motion engine.
       @param x  the forward velocity, basically varies from -10 to 10
       @param y  the sideways velocity, ditto
       @param spin how much the aibo should spin
       @param time how long this command should last
     */
    public void walk(int x, int y, int spin/*, int time*/) {
        byte[] response;
        if (x < -10) x = -10;
        if (x > 10) x = 10;
        if (y < -10) y = -10;
        if (y > 10) y = 10;
        if (spin < -10) spin = -10;
        if (spin > 10) spin = 10;
//         /*if (time > 30) time = 30;
//           if (time < 0) time = 0;*/
//         byte[] command = bundleWalkCommand(x, y, spin/*, time*/);
//         byte[] head = new byte[7+command.length];
//         head[0] = 'm';
//         head[1] = 'o';
//         head[2] = 't';
//         head[3] = 'i';
//         head[4] = 'o';
//         head[5] = 'n';
//         head[6] = ' ';
//         for (int i = 0; i < command.length; i++)
//             head[7 + i] = command[i];
//         
        comm.send(MOTION_STR + " " +  new String(
            bundleWalkCommand(x,y,spin)));// " " + x + " " + y + " " + spin);
//         sendCommand(head);
    }

    /**
     * get the current X, Y, H information from the aibo.
     * do nothing if not connected to an aibo
     */
    public void getXYH() {
//         while (working) {}
//         working = true;
//         if(status != CONNECTED)
//             return;
        
        if (verboseOutput)
            System.out.println("requesting xyh");
        
        Handler<Object> h = new Handler<Object>(true) {
            public Object handle(ByteArrayInputStream data) {
                int bytesReceived = data.read(rawObjects,0,rawObjects.length);
                System.out.println("Read " + bytesReceived + " byte/s of " +
                                   "XYH data");
                parseXYH();
                return null;
            }
        };
        comm.sendAndRecv(GET_XYH_STR,h);
//         try {
//             byte[] bt = {'s','e','n','d','X','Y','H','\0'};
//             outStream.write(bt);
//             
//             //wait for a period of WAIT_TIME, default is 100ms
//             long time_in_ms = System.currentTimeMillis();
//             while(System.currentTimeMillis() - time_in_ms < WAIT_TIME);
//             
//             
//             byteArray.reset();
//             while(inStream.available()!=0) {
//                 int count;
//                 count = inStream.read(tempBytes);
//                 if (verboseOutput)
//                     System.out.println(count+" bytes received from aibo");
//             
//                 byteArray.write(tempBytes,0,count);
//                 long delay = System.currentTimeMillis();
//                 while(System.currentTimeMillis() - delay < WAIT_TIME);
//             }
//             
//             rawObjects = byteArray.toByteArray();
//         } catch (IOException e) {}
//         parseXYH();
//         working = false;
    }


    /* Convert the byte stream information into X, Y, H coordinates.
     */
    public void parseXYH() {
        int o = 0;
        int i = 0;
        int a, b, c;
        while (o < 2) {
            a = rawObjects[i++];
            b = rawObjects[i++];
            if (b != BLANK) {
            c = rawObjects[i++];
            if (c != BLANK)
                objects[o] = (a - CONVERSION) * 100 + (b - CONVERSION) * 10 + (c - CONVERSION);
            else
                objects[o] = (a - CONVERSION) * 10 + (b - CONVERSION);
            } else
            objects[o] = (a - CONVERSION);
            i++;
            o++;
        } 
        dogX = objects[0];
        dogY = objects[1];
        //dogH = objects[2];
    }

    /* Draw the field with the dog on it.  Currently doesn't do anything to indicate heading.
       @param drawingWindow - the frame to draw on
     */

    public void drawXYH(JFrame drawingWindow) {
        Container contentPane = drawingWindow.getContentPane();
        Graphics g = contentPane.getGraphics();
        int w = drawingWindow.getWidth();
        int h = drawingWindow.getHeight() - 20;
        int F_B = 20;
        int F_W = w - F_B;
        int F_H = h - F_B * 2;
        int G_W = F_W * 1 / 3;    //Replace with actual widths * SCALAR
        int B_W = F_W * 2 / 3;
        int C_W = F_W * 1 / 5;
        int D_W = F_W / GRIDW;   //Dog width
        int D_H = F_H / GRIDH;
        int A_L = 10;   //Arrow length
        
        Point ORIGIN = new Point(F_B,F_B+F_H);
        Point OPP_ORIGIN = new Point(F_B+F_W,F_B);
        //Filling in the green field
        g.setColor(Color.green);
        g.fillRect(F_B,F_B,F_W,F_H);
        //Drawing the field outline
        g.setColor(Color.white);
        g.drawRect(F_B,F_B,F_W,F_H);
        //Then fill the red and blue goals
        g.setColor(Color.blue);
        g.fillRect(F_B+(F_W-G_W)/2,F_B+F_H,G_W,2*F_B/3);
        g.setColor(Color.yellow);    // The top goals' color is switched around
        g.fillRect(F_B+(F_W-G_W)/2,F_B/3,G_W,2*F_B/3);
        //Then the goal outlines
        g.setColor(Color.white);
        g.drawRect(F_B+(F_W-G_W)/2,F_B/3,G_W,2*F_B/3);
        g.drawRect(F_B+(F_W-G_W)/2,F_B+F_H,G_W,2*F_B/3);
        //Then the goal boxes
        g.drawRect(F_B+(F_W-B_W)/2,F_B,B_W,2*F_B);
        g.drawRect(F_B+(F_W-B_W)/2,F_H-F_B,B_W,2*F_B);
        //And finally the center line and circle
        g.drawLine(F_B,F_B+F_H/2,F_B+F_W,F_B+F_H/2);
        g.drawOval(F_B+F_W/2-C_W/2,F_B+F_H/2-C_W/2,C_W,C_W);
        g.setFont(FONT);
        // now draw the dog
        g.setColor(Color.red);
        g.fillRect(F_B + D_W * dogY - 5, F_B + D_H * dogX - 5, 10, 10);
    }
    
    public void error(Exception e) {
        System.err.println(e.getMessage());
    }
    
    public class AiboCommunicator {
        private boolean working;
        private ByteBuffer bytes;
        private Socket socket;
        private BufferedWriter writer;
        private BufferedInputStream input;
        private AiboCommunicator() {
            socket = null;
            writer = null;
            input = null;
            status = INITIAL;
            working = false;
            bytes= null;
        }
        private AiboCommunicator(String host, int port) throws 
                UnknownHostException, IOException {
            if (verboseOutput)
                System.out.println("Creating new communications object");
            socket = new Socket(host,port);
            socket.setSoTimeout(SO_TIMOUT);
            writer = new BufferedWriter(new OutputStreamWriter(
                    socket.getOutputStream()));
            input = new BufferedInputStream(socket.getInputStream());
            status = CONNECTED;
            working = false;
            bytes = ByteBuffer.allocate(MAX_BUFFER_SIZE);
            bytes.mark();
        }
        public void close() {
            if (socket == null)
                return;
            try {
                //writer.write(CLOSE_STR + "\0");
                writer.flush();
                writer.close();
                input.close();
                socket.close();
            }catch (IOException e) {
                error(e);
            }//finally {
               // socket = null;
            //}
        }
        private synchronized ByteBuffer sendStringGetResponse(String msg) {
            while (working) {
                try {
                    if (!isConnected() || socket==null || msg == null)
                        return null;
                    wait();
                }catch (InterruptedException e) {}
            }
            if (!isConnected() || socket==null || msg==null) {
                if (verboseOutput)
                    System.out.println("socket not connected, can't send: " + msg);
                return null;
            }
            working = true;
            try {
                input.skip(input.available());
                if (verboseOutput)
                    System.out.println("sending message '" + msg + "'");
                writer.write(msg + "\0\n");
                writer.flush();
                
                bytes.reset();
                int b;
                while ((b=input.read())!=-1 && 
                    (b!='\0' || input.available()!=0))
                    if (bytes.position()<bytes.limit())
                        bytes.put((byte)b);
                if (verboseOutput) {
                    System.out.println("done getting data");
                    System.out.println(bytes.position() + " bytes received");
                }
                return bytes;
            }catch (Exception e) {
                error(e);
                disconnect();
                return null;
            }finally {
                working = false;
            }
        }
        
        public synchronized <T> T sendAndRecv(String msg, Handler<T> h) {
            ByteBuffer bb = sendStringGetResponse(msg);
            if (bb==null || h==null)
                return null;
            if (h.usesBytes())
                return h.handle(new ByteArrayInputStream(bb.array(),bb.arrayOffset(),bb.position()));
            else {
                try {
                    return h.handle(new BufferedReader(new InputStreamReader(new ByteArrayInputStream(bb.array(),bb.arrayOffset(),bb.position()))));
                }catch (IOException e) {
                    error(e);
                    return null;
                }
            }
        }
        
        public synchronized void send(String msg) {
	    System.out.println("comm.send");
            sendAndRecv(msg,null);
        }
    }
    
}

class Handler<T> {
    private boolean bytes;
    public Handler() {
        bytes = false;
    }
    public Handler(boolean useBytes) {
        bytes = useBytes;
    }
    public boolean usesBytes() {
        return bytes;
    }
    public T handle(ByteArrayInputStream data) {
        return null;
    }
    public T handle(BufferedReader data) throws IOException {
        return null;
    }
}
