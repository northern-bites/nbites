package TOOL.GUI;

public class SpecialStrings {
    
    /**
     * Special character string unicode encodings from
     * http://macbiblioblog.blogspot.com/2005/05/special-key-symbols.html
     */

    public static final String META = "\u2318";
    public static final String CTRL = "Ctrl";

    public static final String OPTION = "\u2325";
    public static final String SHIFT = "\u21E7";

    //http://www.fileformat.info/info/unicode/char/2421/index.htm
    public static final String WINDOWS_DELETE = "\u2421";
    public static final String DELETE_FORWARD = "\u2326";
    public static final String DELETE_BACKWARD = "\u232B";

    public static final String SPACE = "\u2423";

    

    public static final String getCtrlOrMeta() {
        if (isOnMac()) {
            return META;
        }
        else {
            return CTRL;
        }
    }

    public static final String getDelete() {
        if (isOnMac()) {
            return DELETE_FORWARD;
        }
        else {
            return WINDOWS_DELETE;
        }
    }

    public static final String getBackspace() {
        return DELETE_BACKWARD;
    }

    public static final boolean isOnMac() {
        return System.getProperty("os.name").contains("Mac");
    }

}