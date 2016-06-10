package teamcomm.gui.drawings;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.glu.GLU;
import com.jogamp.opengl.glu.GLUquadric;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.FloatBuffer;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.ParsePosition;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Set;
import javax.xml.namespace.QName;
import javax.xml.stream.XMLEventReader;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.events.Attribute;
import javax.xml.stream.events.Characters;
import javax.xml.stream.events.StartElement;
import javax.xml.stream.events.XMLEvent;
import teamcomm.gui.drawings.TextureLoader.Texture;

/**
 * Class for parsing ros2 scene files. Instances of this class correspond to
 * named scene elements and can be instantiated to RoSi2Drawabled which may be
 * drawn in a OpenGL context.
 *
 * @author Felix Thielke
 */
public class RoSi2Element {

    /**
     * References to all named elements
     */
    private final Map<String, RoSi2Element> namedElements;

    /**
     * Path of the file in which this element is declared.
     */
    private final File filepath;

    /**
     * Immediate children of this element.
     */
    private final List<RoSi2Element> children = new LinkedList<>();

    /**
     * Set variable bindings for this element and its children.
     */
    private final Map<String, String> vars = new HashMap<>();

    /**
     * Attributes of this element.
     */
    private final Map<String, String> attributes = new HashMap<>();

    /**
     * Name of this element or null if it is unnamed.
     */
    private final String name;

    /**
     * Tag of this element.
     */
    private final String tag;

    /**
     * Textual content of this element.
     */
    private final StringBuilder content = new StringBuilder();

    /**
     * Drawable instance of this element. Only set if the element is constant,
     * i.e. it and its children reference no variables, and it was instantiated
     * as a drawable at least once.
     */
    private RoSi2Drawable constantInstance;

    private RoSi2Element(final File path, final String tag, final Map<String, RoSi2Element> namedElements) {
        this(path, tag, null, null, namedElements);
    }

    private RoSi2Element(final File path, final String tag, final String name, final Iterator<Attribute> iter, final Map<String, RoSi2Element> namedElements) {
        this.filepath = path;
        this.tag = tag;
        this.name = name;
        this.namedElements = namedElements;

        if (iter != null) {
            while (iter.hasNext()) {
                final Attribute attr = iter.next();
                attributes.put(attr.getName().getLocalPart(), attr.getValue());
            }
        }
    }

    /**
     * Returns the name of this element.
     *
     * @return name
     */
    public String getName() {
        return name;
    }

    /**
     * Returns the child element with the given name from the element. Elements
     * are searched via breadth-first search.
     *
     * @param name Name of the child
     * @return Child element or null if no matching element was found
     */
    public RoSi2Element findElement(final String name) {
        if (name == null) {
            return null;
        }

        final LinkedList<RoSi2Element> elems = new LinkedList<>(children);

        while (!elems.isEmpty()) {
            final RoSi2Element cur = elems.pollFirst();
            if (name.equals(cur.name)) {
                return cur;
            } else {
                elems.addAll(cur.children);
            }
        }

        return null;
    }

    /**
     * Returns all child elements with one of the given names from the element.
     * Elements are searched via breadth-first search.
     *
     * @param names Names of the children
     * @return List containing found elements
     */
    public List<RoSi2Element> findElements(final Collection<String> names) {
        final List<RoSi2Element> foundElems = new LinkedList<>();

        if (names.isEmpty()) {
            return foundElems;
        }

        final Set<String> searchedNames = new HashSet<>(names);
        final LinkedList<RoSi2Element> elems = new LinkedList<>(children);

        while (!elems.isEmpty()) {
            final RoSi2Element cur = elems.pollFirst();
            if (cur.name != null && searchedNames.contains(cur.name)) {
                foundElems.add(cur);
                searchedNames.remove(cur.name);
                if (searchedNames.isEmpty()) {
                    return foundElems;
                }
            }

            elems.addAll(cur.children);
        }

        return foundElems;
    }

    /**
     * Instantiates this element as a drawable on the given OpenGL context.
     *
     * @param gl OpenGL context
     * @return drawable
     * @throws teamcomm.gui.drawings.RoSi2Element.RoSi2ParseException if
     * instantiated attributes of the element could not be parsed
     */
    public RoSi2Drawable instantiate(final GL2 gl) throws RoSi2ParseException {
        return instantiate(gl, null);
    }

    /**
     * Instantiates this element as a drawable on the given OpenGL context.
     *
     * @param gl OpenGL context
     * @param vars variable assignments to use
     * @return drawable
     * @throws teamcomm.gui.drawings.RoSi2Element.RoSi2ParseException if
     * instantiated attributes of the element could not be parsed
     */
    public RoSi2Drawable instantiate(final GL2 gl, final Map<String, String> vars) throws RoSi2ParseException {
        return instantiate(gl, vars, null);
    }

    private RoSi2Drawable instantiate(final GL2 gl, final Map<String, String> vars, final List<RoSi2Drawable> refChilds) throws RoSi2ParseException {
        // The instantiation is constant unless it references a variable
        boolean constant = true;

        // Return the constant instance if it exists
        if (constantInstance != null) {
            return constantInstance;
        }

        // Merge the given variable bindings with those of the element
        final Map<String, String> varBindings;
        if (vars != null) {
            varBindings = vars;
            for (final Map.Entry<String, String> entry : this.vars.entrySet()) {
                if (!varBindings.containsKey(entry.getKey())) {
                    varBindings.put(entry.getKey(), entry.getValue());
                }
            }
        } else {
            varBindings = new HashMap<>(this.vars);
        }

        // Instantiate all child elements
        final List<RoSi2Drawable> childInstances = (refChilds != null) ? new LinkedList<>(refChilds) : new LinkedList<RoSi2Drawable>();
        for (final RoSi2Element child : children) {
            final RoSi2Drawable childInst = child.instantiate(gl, varBindings, null);
            if (childInst != null) {
                // If a child instance is not constant, the instance of this
                // element is neither
                if (childInst != child.constantInstance) {
                    constant = false;
                }

                childInstances.add(childInst);
            }
        }

        // Check if this element references another and in that case instantiate
        // the referenced element
        final String ref = getAttributeValue(varBindings, "ref", false);
        if (ref != null) {
            final RoSi2Element referenced = namedElements.get(tag + "#" + ref);
            if (referenced == null) {
                throw new RoSi2ParseException("Referenced element cannot be found: " + ref);
            }
            return referenced.instantiate(gl, varBindings, childInstances);
        }

        // Instantiate this element
        final RoSi2Drawable instance;
        switch (tag) {
            case "Compound":
                instance = new Compound(gl, childInstances);
                break;
            case "Body":
                instance = new Body(gl, childInstances);
                break;
            case "Translation":
                instance = new Translation(new float[]{
                    getLength(varBindings, "x", false, 0.0f),
                    getLength(varBindings, "y", false, 0.0f),
                    getLength(varBindings, "z", false, 0.0f)
                });
                break;
            case "Rotation":
                instance = new Rotation(new float[]{
                    getAngle(varBindings, "x", false, 0.0f),
                    getAngle(varBindings, "y", false, 0.0f),
                    getAngle(varBindings, "z", false, 0.0f)
                });
                break;
            case "Appearance":
                instance = new Appearance(gl, childInstances);
                break;
            case "BoxAppearance":
                instance = new BoxAppearance(gl, childInstances,
                        getLength(varBindings, "width", true, 0.0f),
                        getLength(varBindings, "height", true, 0.0f),
                        getLength(varBindings, "depth", true, 0.0f));
                break;
            case "SphereAppearance":
                instance = new SphereAppearance(gl, childInstances,
                        getLength(varBindings, "radius", true, 0.0f));
                break;
            case "CylinderAppearance":
                instance = new CylinderAppearance(gl, childInstances,
                        getLength(varBindings, "height", true, 0.0f),
                        getLength(varBindings, "radius", true, 0.0f));
                break;
            case "CapsuleAppearance":
                instance = new CapsuleAppearance(gl, childInstances,
                        getLength(varBindings, "height", true, 0.0f),
                        getLength(varBindings, "radius", true, 0.0f));
                break;
            case "ComplexAppearance":
                instance = new ComplexAppearance(gl, childInstances);
                break;
            case "Vertices": {
                final double unit = getUnit(varBindings, "unit", false, 1.0f);
                final ArrayList<Vertices.Vertex> vs = new ArrayList<>();
                final String str = content.toString();
                final DecimalFormat fmt = new DecimalFormat();
                fmt.setGroupingUsed(false);
                final DecimalFormatSymbols sym = (DecimalFormatSymbols) DecimalFormatSymbols.getInstance().clone();
                sym.setDecimalSeparator('.');
                fmt.setDecimalFormatSymbols(sym);
                int pos = 0;
                final double[] vertex = new double[3];
                int i = 0;
                // parse vertices
                parsing:
                while (pos < str.length()) {
                    while (Character.isWhitespace(str.charAt(pos))) {
                        pos++;
                        if (pos == str.length()) {
                            break parsing;
                        }
                    }
                    while (str.charAt(pos) == '#') {
                        do {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        } while (str.charAt(pos) != '\n' && str.charAt(pos) != '\r');
                        while (Character.isWhitespace(str.charAt(pos))) {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        }
                    }
                    final ParsePosition p = new ParsePosition(pos);
                    final Number n = fmt.parse(str, p);
                    if (n == null) {
                        throw new RoSi2ParseException("Vertex coordinate is not a number: " + str.substring(pos, pos + 1));
                    }
                    pos = p.getIndex();
                    vertex[i++] = n.doubleValue();

                    if (i == 3) {
                        vs.add(new Vertices.Vertex((float) (vertex[0] * unit), (float) (vertex[1] * unit), (float) (vertex[2] * unit)));
                        i = 0;
                    }
                }
                vs.trimToSize();
                instance = new Vertices(vs);
                break;
            }
            case "Normals": {
                final ArrayList<Normals.Normal> ns = new ArrayList<>();
                final String str = content.toString();
                final DecimalFormat fmt = new DecimalFormat();
                fmt.setGroupingUsed(false);
                final DecimalFormatSymbols sym = (DecimalFormatSymbols) DecimalFormatSymbols.getInstance().clone();
                sym.setDecimalSeparator('.');
                fmt.setDecimalFormatSymbols(sym);
                int pos = 0;
                final float[] normal = new float[3];
                int i = 0;
                // parse normals
                parsing:
                while (pos < str.length()) {
                    while (Character.isWhitespace(str.charAt(pos))) {
                        pos++;
                        if (pos == str.length()) {
                            break parsing;
                        }
                    }
                    while (str.charAt(pos) == '#') {
                        do {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        } while (str.charAt(pos) != '\n' && str.charAt(pos) != '\r');
                        while (Character.isWhitespace(str.charAt(pos))) {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        }
                    }
                    final ParsePosition p = new ParsePosition(pos);
                    final Number n = fmt.parse(str, p);
                    if (n == null) {
                        throw new RoSi2ParseException("Normal coordinate is not a number");
                    }
                    normal[i++] = n.floatValue();
                    pos = p.getIndex();

                    if (i == 3) {
                        ns.add(new Normals.Normal(normal[0], normal[1], normal[2], 1));
                        i = 0;
                    }
                }
                ns.trimToSize();
                instance = new Normals(ns);
                break;
            }
            case "TexCoords": {
                final ArrayList<TexCoords.TexCoord> ts = new ArrayList<>();
                final String str = content.toString();
                final DecimalFormat fmt = new DecimalFormat();
                fmt.setGroupingUsed(false);
                final DecimalFormatSymbols sym = (DecimalFormatSymbols) DecimalFormatSymbols.getInstance().clone();
                sym.setDecimalSeparator('.');
                fmt.setDecimalFormatSymbols(sym);
                int pos = 0;
                final float[] coord = new float[2];
                int i = 0;
                // parse texture coordinates
                parsing:
                while (pos < str.length()) {
                    while (Character.isWhitespace(str.charAt(pos))) {
                        pos++;
                        if (pos == str.length()) {
                            break parsing;
                        }
                    }
                    while (str.charAt(pos) == '#') {
                        do {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        } while (str.charAt(pos) != '\n' && str.charAt(pos) != '\r');
                        while (Character.isWhitespace(str.charAt(pos))) {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        }
                    }
                    final ParsePosition p = new ParsePosition(pos);
                    final Number n = fmt.parse(str, p);
                    if (n == null) {
                        throw new RoSi2ParseException("Texture coordinate is not a number");
                    }
                    coord[i++] = n.floatValue();
                    pos = p.getIndex();

                    if (i == 2) {
                        ts.add(new TexCoords.TexCoord(coord[0], coord[1]));
                        i = 0;
                    }
                }
                ts.trimToSize();
                instance = new TexCoords(ts);
                break;
            }
            case "Triangles":
            case "Quads": {
                final LinkedList<Integer> vs = new LinkedList<>();
                final String str = content.toString();
                final DecimalFormat fmt = new DecimalFormat();
                fmt.setParseIntegerOnly(true);
                int pos = 0;
                // parse vertex indices
                parsing:
                while (pos < str.length()) {
                    while (Character.isWhitespace(str.charAt(pos))) {
                        pos++;
                        if (pos == str.length()) {
                            break parsing;
                        }
                    }
                    while (str.charAt(pos) == '#') {
                        do {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        } while (str.charAt(pos) != '\n' && str.charAt(pos) != '\r');
                        while (Character.isWhitespace(str.charAt(pos))) {
                            pos++;
                            if (pos == str.length()) {
                                break parsing;
                            }
                        }
                    }
                    final ParsePosition p = new ParsePosition(pos);
                    final Number n = fmt.parse(str, p);
                    if (n == null) {
                        throw new RoSi2ParseException("Vertex index is not a number");
                    }
                    vs.add(n.intValue());
                    pos = p.getIndex();
                }
                instance = new PrimitiveGroup(tag.equals("Triangles") ? GL.GL_TRIANGLES : GL2.GL_QUADS, vs);
                break;
            }
            case "Surface":
                final String texturePath = getAttributeValue(varBindings, "diffuseTexture", false);
                final Texture texture;
                if (texturePath == null) {
                    texture = null;
                } else {
                    try {
                        texture = TextureLoader.getInstance().loadTexture(gl, new File(filepath.getParentFile(), texturePath));
                    } catch (IOException ex) {
                        throw new RoSi2ParseException("Texture not found: " + texturePath);
                    }
                }
                final String shininessStr = getAttributeValue(varBindings, "shininess", false);
                Float shininess;
                if (shininessStr == null) {
                    shininess = null;
                } else {
                    try {
                        shininess = Float.valueOf(shininessStr);
                        if (shininess < 0.0f || shininess > 128.0f) {
                            throw new RoSi2ParseException("Shininess value must be between 0 and 128, found: " + shininess);
                        }
                    } catch (final NumberFormatException e) {
                        shininess = null;
                    }
                }
                instance = new Surface(
                        getColor(varBindings, "diffuseColor", true),
                        getColor(varBindings, "ambientColor", false),
                        getColor(varBindings, "specularColor", false),
                        getColor(varBindings, "emissionColor", false),
                        shininess,
                        texture);
                break;
            default:
                return null;
        }

        // Store the instance if it is constant
        if (constant) {
            constantInstance = instance;
        }

        return instance;
    }

    private float[] getColor(final Map<String, String> varBindings, final String key, final boolean required) throws RoSi2ParseException {
        final String val = getAttributeValue(varBindings, key, required);
        if (val == null) {
            return null;
        }
        final float f1_255 = 1.0f / 255.0f;
        final float f1_15 = 1.0f / 15.0f;

        if (val.charAt(0) == '#') {
            // html style #rrggbb, #rgb
            // + self invented #rrggbbaa, #rgba
            switch (val.length()) {
                case 4:
                    return new float[]{
                        f1_15 * hexDigit(val.charAt(1)),
                        f1_15 * hexDigit(val.charAt(2)),
                        f1_15 * hexDigit(val.charAt(3)),
                        1.0f
                    };
                case 5:
                    return new float[]{
                        f1_15 * hexDigit(val.charAt(1)),
                        f1_15 * hexDigit(val.charAt(2)),
                        f1_15 * hexDigit(val.charAt(3)),
                        f1_15 * hexDigit(val.charAt(4))
                    };
                case 7:
                    return new float[]{
                        f1_255 * ((hexDigit(val.charAt(1)) << 4) | hexDigit(val.charAt(2))),
                        f1_255 * ((hexDigit(val.charAt(3)) << 4) | hexDigit(val.charAt(4))),
                        f1_255 * ((hexDigit(val.charAt(5)) << 4) | hexDigit(val.charAt(6))),
                        1.0f
                    };
                case 9:
                    return new float[]{
                        f1_255 * ((hexDigit(val.charAt(1)) << 4) | hexDigit(val.charAt(2))),
                        f1_255 * ((hexDigit(val.charAt(3)) << 4) | hexDigit(val.charAt(4))),
                        f1_255 * ((hexDigit(val.charAt(5)) << 4) | hexDigit(val.charAt(6))),
                        f1_255 * ((hexDigit(val.charAt(7)) << 4) | hexDigit(val.charAt(8)))
                    };
            }
        } else if (val.charAt(val.length() - 1) == ')') {
            if (val.startsWith("rgb(")) {
                // css style rgb color (rgb(r,g,b) with r,g,b\in[0..255]\cup[0%,..,100%])
                final String[] values = val.substring(4, val.length() - 1).replace(" ", "").split(",");
                if (values.length == 3) {
                    final float[] color = new float[4];
                    for (int i = 0; i < 3; i++) {
                        if (values[i].charAt(values[i].length() - 1) == '%') {
                            color[i] = Double.valueOf(values[i].substring(0, values[i].length() - 1)).floatValue() * 0.01f;
                        } else {
                            color[i] = Double.valueOf(values[i]).floatValue() * f1_255;
                        }
                    }
                    color[3] = 1.0f;
                    return color;
                }
            } else if (val.startsWith("rgba(")) {
                // css3 style rgba color (rgba(r,g,b,a) with r,g,b\in[0..255]\cup[0%,..,100%] and a\in[0..1])
                final String[] values = val.substring(5, val.length() - 1).replace(" ", "").split(",");
                if (values.length == 4) {
                    final float[] color = new float[4];
                    for (int i = 0; i < 3; i++) {
                        if (values[i].charAt(values[i].length() - 1) == '%') {
                            color[i] = Double.valueOf(values[i].substring(0, values[i].length() - 1)).floatValue() * 0.01f;
                        } else {
                            color[i] = Double.valueOf(values[i]).floatValue() * f1_255;
                        }
                    }
                    color[3] = Double.valueOf(values[3]).floatValue();
                    return color;
                }
            }
        }

        throw new RoSi2ParseException("invalid color format: " + val);
    }

    private static int hexDigit(final char ch) {
        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        } else if (ch >= 'a' && ch <= 'f') {
            return (ch - 'a') + 10;
        }

        throw new IllegalArgumentException();
    }

    private float getLength(final Map<String, String> varBindings, final String key, final boolean required, final float defaultValue) throws RoSi2ParseException {
        float[] value = new float[1];
        String[] unit = new String[1];
        if (!getFloatAndUnit(varBindings, key, required, value, unit)) {
            return defaultValue;
        }

        if (unit[0].isEmpty() || unit[0].equals("m")) {
            return value[0];
        } else if (unit[0].equals("mm")) {
            return value[0] * 0.001f;
        } else if (unit[0].equals("cm")) {
            return value[0] * 0.01f;
        } else if (unit[0].equals("dm")) {
            return value[0] * 0.1f;
        } else if (unit[0].equals("km")) {
            return value[0] * 1000.0f;
        }

        throw new RoSi2ParseException("Unexpected unit \"" + unit[0] + " (expected one of \"mm, cm, dm, m, km\")");
    }

    private float getUnit(final Map<String, String> varBindings, final String key, final boolean required, final float defaultValue) throws RoSi2ParseException {
        final String unit = getAttributeValue(varBindings, key, required);
        if (unit == null || unit.isEmpty()) {
            return defaultValue;
        }

        switch (unit) {
            case "m":
                return 1.0f;
            case "mm":
                return 0.001f;
            case "cm":
                return 0.01f;
            case "dm":
                return 0.1f;
            case "km":
                return 1000.0f;
        }

        throw new RoSi2ParseException("Unexpected unit \"" + unit + " (expected one of \"mm, cm, dm, m, km\")");
    }

    private float getAngle(final Map<String, String> varBindings, final String key, final boolean required, final float defaultValue) throws RoSi2ParseException {
        float[] value = new float[1];
        String[] unit = new String[1];
        if (!getFloatAndUnit(varBindings, key, required, value, unit)) {
            return defaultValue;
        }

        if (unit[0].isEmpty() || unit[0].equals("radian")) {
            return value[0];
        } else if (unit[0].equals("degree")) {
            return value[0] * (float) Math.PI / 180.0f;
        }

        throw new RoSi2ParseException("Unexpected unit \"" + unit[0] + " (expected one of \"degree, radian\")");
    }

    private boolean getFloatAndUnit(final Map<String, String> varBindings, final String key, final boolean required, final float[] value, final String[] unit) throws RoSi2ParseException {
        final String val = getAttributeValue(varBindings, key, required);
        if (val == null) {
            return false;
        }

        ParsePosition pos = new ParsePosition(0);
        final DecimalFormat fmt = new DecimalFormat();
        fmt.setGroupingUsed(false);
        final DecimalFormatSymbols sym = (DecimalFormatSymbols) DecimalFormatSymbols.getInstance().clone();
        sym.setDecimalSeparator('.');
        fmt.setDecimalFormatSymbols(sym);
        final Number v = fmt.parse(val, pos);
        if (v != null) {
            value[0] = v.floatValue();
            unit[0] = val.substring(pos.getIndex()).trim();
            return true;
        }

        return false;
    }

    private String getAttributeValue(final Map<String, String> varBindings, final String key, final boolean required) throws RoSi2ParseException {
        final String raw = attributes.get(key);
        if (raw == null) {
            if (required) {
                throw new RoSi2ParseException("Missing attribute: " + key);
            }
            return null;
        }
        int varStart = raw.indexOf('$');
        if (varStart < 0) {
            return raw;
        }

        final StringBuilder value = new StringBuilder(raw.length());
        int varEnd = 0;
        while (varStart >= 0) {
            value.append(raw.substring(varEnd, varStart));
            if (varStart + 1 == raw.length()) {
                varEnd = varStart;
                break;
            }

            final char c = raw.charAt(varStart + 1);
            final String varName;
            if (c == '(' || c == '{') {
                final char cEnd = c == '(' ? ')' : '}';
                varEnd = raw.indexOf(cEnd, varStart + 2);
                if (varEnd < 0) {
                    throw new RoSi2ParseException("Invalid attribute format: missing " + cEnd);
                }
                varName = raw.substring(varStart + 2, varEnd);
                varEnd++;
            } else {
                varEnd = varStart + 1;
                while (varEnd < raw.length() && Character.isLetterOrDigit(raw.charAt(varEnd))) {
                    varEnd++;
                }
                varName = raw.substring(varStart + 1, varEnd);
            }

            final String binding = varBindings.get(varName);
            if (binding == null) {
                value.append(raw.substring(varStart, varEnd));
            } else {
                value.append(binding);
            }

            if (varEnd == raw.length()) {
                break;
            } else {
                varStart = raw.indexOf('$', varEnd);
            }
        }
        if (varEnd < raw.length()) {
            value.append(raw.substring(varEnd));
        }

        return value.toString();
    }

    /**
     * Abstract base class for drawable instantiations of elements read from
     * ros2 files.
     */
    public static abstract class RoSi2Drawable {

        private final GL2 gl;

        /**
         * Instantiated children of this element.
         */
        protected final List<RoSi2Drawable> children;

        private final Rotation rotation;
        private final Translation translation;

        /**
         * Constructor which leaves the attributes uninitialized. Useful for
         * drawables containinng only data.
         */
        protected RoSi2Drawable() {
            gl = null;
            children = new LinkedList<>();
            rotation = null;
            translation = null;
        }

        private RoSi2Drawable(final GL2 gl, final List<RoSi2Drawable> children) throws RoSi2ParseException {
            this.gl = gl;
            this.children = children;

            // Find children defining a transformation
            Translation t = null;
            Rotation r = null;
            ListIterator<RoSi2Drawable> iter = children.listIterator();
            while (iter.hasNext()) {
                final RoSi2Drawable cur = iter.next();
                if (cur instanceof Translation) {
                    if (t != null) {
                        throw new RoSi2ParseException("More than one Translation element");
                    }
                    iter.remove();
                    t = (Translation) cur;
                } else if (cur instanceof Rotation) {
                    if (r != null) {
                        throw new RoSi2ParseException("More than one Rotation element");
                    }
                    iter.remove();
                    r = (Rotation) cur;
                }
            }

            translation = t;
            rotation = r;
        }

        /**
         * Draws this element and its children.
         */
        public final void draw() {
            // Apply transformation
            if (translation != null || rotation != null) {
                gl.glPushMatrix();

                if (translation != null) {
                    gl.glTranslatef(translation.translation[0], translation.translation[1], translation.translation[2]);
                }
                if (rotation != null) {
                    gl.glRotated(Math.toDegrees(rotation.rotation[0]), 1, 0, 0);
                    gl.glRotated(Math.toDegrees(rotation.rotation[1]), 0, 1, 0);
                    gl.glRotated(Math.toDegrees(rotation.rotation[2]), 0, 0, 1);
                }
            }

            // Draw this element
            render(gl);

            // Draw children
            for (final RoSi2Drawable child : children) {
                child.draw();
            }

            // Reset transformation
            if (translation != null || rotation != null) {
                gl.glPopMatrix();
            }
        }

        /**
         * Draws this element using the given GL object.
         *
         * @param gl GL object
         */
        protected abstract void render(final GL2 gl);

        /**
         * Creates a display list on the given GL object which renders the
         * element. The display list will not be destroyed, so this object may
         * safely be garbage collected afterwards.
         *
         * @return number of the created display list
         */
        public final int createDisplayList() {
            final int listId = gl.glGenLists(1);

            gl.glNewList(listId, GL2.GL_COMPILE);
            draw();
            gl.glEndList();

            return listId;
        }
    }

    private static class Compound extends RoSi2Drawable {

        public Compound(final GL2 gl, final List<RoSi2Drawable> children) throws RoSi2ParseException {
            super(gl, children);
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class Body extends RoSi2Drawable {

        public Body(final GL2 gl, final List<RoSi2Drawable> children) throws RoSi2ParseException {
            super(gl, children);
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }
    }

    private static class Translation extends RoSi2Drawable {

        public final float[] translation;

        public Translation(final float[] translation) {
            this.translation = translation;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class Rotation extends RoSi2Drawable {

        public final float[] rotation;

        public Rotation(final float[] rotation) {
            this.rotation = rotation;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class Appearance extends RoSi2Drawable {

        protected final Surface surface;

        public Appearance(final GL2 gl, final List<RoSi2Drawable> children) throws RoSi2ParseException {
            super(gl, children);

            Surface s = null;
            ListIterator<RoSi2Drawable> iter = this.children.listIterator();
            while (iter.hasNext()) {
                final RoSi2Drawable cur = iter.next();
                if (cur instanceof Surface) {
                    if (s != null) {
                        throw new RoSi2ParseException("More than one Surface element");
                    }
                    s = (Surface) cur;
                    iter.remove();
                }
            }
            if (s == null && !getClass().equals(Appearance.class)) {
                throw new RoSi2ParseException(getClass().getSimpleName() + " element needs a Surface element");
            }
            surface = s;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class BoxAppearance extends Appearance {

        /**
         * The width of the box (cy).
         */
        private final float width;

        /**
         * The height of the box (cz).
         */
        private final float height;

        /**
         * The depth of the box (cx).
         */
        private final float depth;

        public BoxAppearance(final GL2 gl, final List<RoSi2Drawable> children, final float width, final float height, final float depth) throws RoSi2ParseException {
            super(gl, children);

            this.width = width;
            this.height = height;
            this.depth = depth;
        }

        @Override
        protected void render(final GL2 gl) {
            surface.set(gl);

            final float lx = depth * 0.5f;
            final float ly = width * 0.5f;
            final float lz = height * 0.5f;

            // -y-side
            gl.glBegin(GL2.GL_TRIANGLE_FAN);
            gl.glNormal3f(0, -1, 0);
            gl.glVertex3f(lx, -ly, -lz);
            gl.glVertex3f(lx, -ly, lz);
            gl.glVertex3f(-lx, -ly, lz);
            gl.glVertex3f(-lx, -ly, -lz);
            gl.glEnd();

            // y-side
            gl.glBegin(GL2.GL_TRIANGLE_FAN);
            gl.glNormal3f(0, 1, 0);
            gl.glVertex3f(-lx, ly, lz);
            gl.glVertex3f(lx, ly, lz);
            gl.glVertex3f(lx, ly, -lz);
            gl.glVertex3f(-lx, ly, -lz);
            gl.glEnd();

            // -x-side
            gl.glBegin(GL2.GL_TRIANGLE_FAN);
            gl.glNormal3f(-1, 0, 0);
            gl.glVertex3f(-lx, -ly, -lz);
            gl.glVertex3f(-lx, -ly, lz);
            gl.glVertex3f(-lx, ly, lz);
            gl.glVertex3f(-lx, ly, -lz);
            gl.glEnd();

            // x-side
            gl.glBegin(GL2.GL_TRIANGLE_FAN);
            gl.glNormal3f(1, 0, 0);
            gl.glVertex3f(lx, -ly, -lz);
            gl.glVertex3f(lx, ly, -lz);
            gl.glVertex3f(lx, ly, lz);
            gl.glVertex3f(lx, -ly, lz);
            gl.glEnd();

            // bottom
            gl.glBegin(GL2.GL_TRIANGLE_FAN);
            gl.glNormal3f(0, 0, -1);
            gl.glVertex3f(-lx, -ly, -lz);
            gl.glVertex3f(-lx, ly, -lz);
            gl.glVertex3f(lx, ly, -lz);
            gl.glVertex3f(lx, -ly, -lz);
            gl.glEnd();

            // top
            gl.glBegin(GL2.GL_TRIANGLE_FAN);
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(-lx, -ly, lz);
            gl.glVertex3f(lx, -ly, lz);
            gl.glVertex3f(lx, ly, lz);
            gl.glVertex3f(-lx, ly, lz);
            gl.glEnd();

            surface.unset(gl);
        }

    }

    private static class SphereAppearance extends Appearance {

        /**
         * The radius of the sphere.
         */
        private final float radius;

        public SphereAppearance(final GL2 gl, final List<RoSi2Drawable> children, final float radius) throws RoSi2ParseException {
            super(gl, children);

            this.radius = radius;
        }

        @Override
        protected void render(final GL2 gl) {
            final GLU glu = GLU.createGLU(gl);

            surface.set(gl);

            final GLUquadric q = glu.gluNewQuadric();
            glu.gluSphere(q, radius, 16, 16);
            glu.gluDeleteQuadric(q);

            surface.unset(gl);
        }

    }

    private static class CylinderAppearance extends Appearance {

        /**
         * The height of the cylinder.
         */
        private final float height;

        /**
         * The radius.
         */
        private final float radius;

        public CylinderAppearance(final GL2 gl, final List<RoSi2Drawable> children, final float height, final float radius) throws RoSi2ParseException {
            super(gl, children);

            this.height = height;
            this.radius = radius;
        }

        @Override
        protected void render(final GL2 gl) {
            final GLU glu = GLU.createGLU(gl);

            surface.set(gl);

            final GLUquadric q = glu.gluNewQuadric();
            gl.glTranslatef(0.f, 0.f, height * -0.5f);
            glu.gluCylinder(q, radius, radius, height, 16, 1);
            gl.glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
            glu.gluDisk(q, 0, radius, 16, 1);
            gl.glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
            gl.glTranslatef(0, 0, height);
            glu.gluDisk(q, 0, radius, 16, 1);
            gl.glTranslatef(0.f, 0.f, height * -0.5f);
            glu.gluDeleteQuadric(q);

            surface.unset(gl);
        }

    }

    private static class CapsuleAppearance extends Appearance {

        /**
         * The height of the capsule.
         */
        private final float height;

        /**
         * The radius.
         */
        private final float radius;

        public CapsuleAppearance(final GL2 gl, final List<RoSi2Drawable> children, final float height, final float radius) throws RoSi2ParseException {
            super(gl, children);

            this.height = height;
            this.radius = radius;
        }

        @Override
        protected void render(final GL2 gl) {
            final GLU glu = GLU.createGLU(gl);

            surface.set(gl);

            final GLUquadric q = glu.gluNewQuadric();
            final float cylinderHeight = height - radius - radius;
            gl.glTranslatef(0.f, 0.f, cylinderHeight * -0.5f);
            glu.gluCylinder(q, radius, radius, cylinderHeight, 16, 1);
            glu.gluSphere(q, radius, 16, 16);
            gl.glTranslatef(0, 0, cylinderHeight);
            glu.gluSphere(q, radius, 16, 16);
            glu.gluDeleteQuadric(q);

            surface.unset(gl);
        }

    }

    private static class ComplexAppearance extends Appearance {

        private final Vertices vertices;
        private final Normals normals;
        private final boolean normalsDefined;
        private final TexCoords texCoords;
        private final List<PrimitiveGroup> primitiveGroups;

        public ComplexAppearance(final GL2 gl, final List<RoSi2Drawable> children) throws RoSi2ParseException {
            super(gl, children);

            // scan children for elements defining the complex appearance
            primitiveGroups = new LinkedList<>();
            Vertices v = null;
            Normals n = null;
            TexCoords t = null;
            final ListIterator<RoSi2Drawable> iter = this.children.listIterator();
            while (iter.hasNext()) {
                final RoSi2Drawable child = iter.next();
                if (child instanceof PrimitiveGroup) {
                    primitiveGroups.add((PrimitiveGroup) child);
                    iter.remove();
                } else if (child instanceof Vertices) {
                    if (v != null) {
                        throw new RoSi2ParseException("More than one Vertices element.");
                    }
                    v = (Vertices) child;
                    iter.remove();
                } else if (child instanceof Normals) {
                    if (n != null) {
                        throw new RoSi2ParseException("More than one Normals element.");
                    }
                    n = (Normals) child;
                    iter.remove();
                } else if (child instanceof TexCoords) {
                    if (t != null) {
                        throw new RoSi2ParseException("More than one TexCoords element.");
                    }
                    t = (TexCoords) child;
                    iter.remove();
                }
            }

            if (v == null) {
                throw new RoSi2ParseException("ComplexAppearance element requires a Vertices element");
            } else if (primitiveGroups.isEmpty()) {
                throw new RoSi2ParseException("ComplexAppearance element requires a Triangles or Quads element");
            }
            vertices = v;

            if (n == null) {
                normalsDefined = false;
                n = computeNormals();
            } else {
                normalsDefined = true;
            }
            normals = n;

            texCoords = t;
        }

        private Normals computeNormals() {
            ArrayList<Normals.Normal> ns = new ArrayList<>(vertices.vertices.size());
            for (Vertices.Vertex v : vertices.vertices) {
                ns.add(new Normals.Normal());
            }

            for (PrimitiveGroup primitiveGroup : primitiveGroups) {
                ListIterator<Integer> iter = primitiveGroup.vertices.listIterator();
                while (iter.hasNext()) {
                    int i1 = iter.next();
                    if (i1 >= ns.size()) {
                        iter.set(0);
                        i1 = 0;
                    }
                    int i2 = iter.next();
                    if (i2 >= ns.size()) {
                        iter.set(0);
                        i2 = 0;
                    }
                    int i3 = iter.next();
                    if (i3 >= ns.size()) {
                        iter.set(0);
                        i3 = 0;
                    }
                    int i4 = 0;
                    if (primitiveGroup.mode == GL2.GL_QUADS) {
                        i4 = iter.next();
                        if (i4 >= ns.size()) {
                            iter.set(0);
                        }
                        i4 = 0;
                    }

                    final Vertices.Vertex p1 = vertices.vertices.get(i1);
                    final Vertices.Vertex p2 = vertices.vertices.get(i2);
                    final Vertices.Vertex p3 = vertices.vertices.get(i3);

                    final Vertices.Vertex u = new Vertices.Vertex(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
                    final Vertices.Vertex v = new Vertices.Vertex(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
                    final Normals.Normal n = new Normals.Normal(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x, 1);
                    double len = Math.sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
                    len = len == 0 ? 1.f : 1.f / len;
                    n.x *= len;
                    n.y *= len;
                    n.z *= len;

                    ns.get(i1).add(n);
                    ns.get(i2).add(n);
                    ns.get(i3).add(n);
                    if (primitiveGroup.mode == GL2.GL_QUADS) {
                        ns.get(i4).add(n);
                    }
                }
            }

            for (Normals.Normal i : ns) {
                if (i.length > 0) {
                    final float mult = 1.0f / (float) i.length;
                    i.x *= mult;
                    i.y *= mult;
                    i.z *= mult;
                }
            }

            return new Normals(ns);
        }

        @Override
        protected void render(final GL2 gl) {
            surface.set(gl, texCoords == null);

            for (final PrimitiveGroup primitiveGroup : primitiveGroups) {
                gl.glBegin(primitiveGroup.mode);
                final Iterator<Integer> iter = primitiveGroup.vertices.iterator();

                while (iter.hasNext()) {
                    final int i = iter.next();
                    if (texCoords != null && i < texCoords.coords.size()) {
                        gl.glTexCoord2f(texCoords.coords.get(i).x, texCoords.coords.get(i).y);
                    }
                    if (normalsDefined) {
                        if (iter.hasNext()) {
                            final Normals.Normal n = normals.normals.get(iter.next());
                            gl.glNormal3f(n.x, n.y, n.z);
                        } else {
                            break;
                        }
                    } else {
                        gl.glNormal3f(normals.normals.get(i).x, normals.normals.get(i).y, normals.normals.get(i).z);
                    }
                    gl.glVertex3f(vertices.vertices.get(i).x, vertices.vertices.get(i).y, vertices.vertices.get(i).z);
                }
                gl.glEnd();
            }

            surface.unset(gl, texCoords == null);
        }

    }

    private static class Vertices extends RoSi2Drawable {

        public static class Vertex {

            public float x;
            public float y;
            public float z;

            public Vertex(final float x, final float y, final float z) {
                this.x = x;
                this.y = y;
                this.z = z;
            }
        }

        public final List<Vertex> vertices;

        public Vertices(final List<Vertex> vertices) {
            this.vertices = vertices;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class Normals extends RoSi2Drawable {

        public static class Normal extends Vertices.Vertex {

            public int length;

            public Normal() {
                this(0, 0, 0, 0);
            }

            public Normal(final float x, final float y, final float z, final int length) {
                super(x, y, z);
                this.length = length;
            }

            /**
             * Addition of another normal to this one.
             *
             * @param other The other normal that will be added to this one
             */
            void add(final Normal other) {
                x += other.x;
                y += other.y;
                z += other.z;
                length += other.length;
            }
        }

        public final List<Normal> normals;

        public Normals(final List<Normal> normals) {
            this.normals = normals;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class TexCoords extends RoSi2Drawable {

        /**
         * A point on a texture.
         */
        public static class TexCoord {

            /**
             * The x-component of the point.
             */
            public float x;
            /**
             * The y-component of the point.
             */
            public float y;

            /**
             * Constructs a point of a texture
             *
             * @param x The x-component of the point
             * @param y The y-component of the point
             */
            public TexCoord(final float x, final float y) {
                this.x = x;
                this.y = y;
            }
        };

        public final List<TexCoord> coords;

        public TexCoords(final List<TexCoord> coords) {
            this.coords = coords;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class PrimitiveGroup extends RoSi2Drawable {

        private final int mode;
        private final List<Integer> vertices;

        public PrimitiveGroup(final int mode, final List<Integer> vertices) {
            this.mode = mode;
            this.vertices = vertices;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

    }

    private static class Surface extends RoSi2Drawable {

        public final float[] diffuseColor;
        public final float[] ambientColor;
        public final float[] specularColor;
        public final float[] emissionColor;
        public final float shininess;
        public final Texture texture;

        public Surface(final float[] diffuseColor, final float[] ambientColor, final float[] specularColor, final float[] emissionColor, final Float shininess, final Texture texture) {
            this.diffuseColor = diffuseColor;
            this.ambientColor = ambientColor;

            if (specularColor != null) {
                this.specularColor = specularColor;
            } else {
                this.specularColor = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
            }

            if (emissionColor != null) {
                this.emissionColor = emissionColor;
            } else {
                this.emissionColor = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
            }

            if (shininess != null) {
                this.shininess = shininess;
            } else {
                this.shininess = 0.0f;
            }

            this.texture = texture;
        }

        @Override
        protected void render(final GL2 gl) {
            // Do nothing
        }

        public void set(final GL2 gl) {
            set(gl, true);
        }

        public void set(final GL2 gl, final boolean defaultTextureSize) {
            if (ambientColor != null) {
                gl.glColorMaterial(GL2.GL_FRONT, GL2.GL_DIFFUSE);
                gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_AMBIENT, FloatBuffer.wrap(ambientColor));
            } else {
                gl.glColorMaterial(GL2.GL_FRONT, GL2.GL_AMBIENT_AND_DIFFUSE);
            }
            gl.glColor4fv(FloatBuffer.wrap(diffuseColor));

            gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_SPECULAR, FloatBuffer.wrap(specularColor));
            gl.glMaterialf(GL2.GL_FRONT, GL2.GL_SHININESS, shininess);
            gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_EMISSION, FloatBuffer.wrap(emissionColor));

            if (texture != null) {
                gl.glBindTexture(GL2.GL_TEXTURE_2D, texture.id);

                if (texture.hasAlpha) {
                    gl.glEnable(GL2.GL_BLEND);
                    gl.glBlendFunc(GL2.GL_SRC_ALPHA, GL2.GL_ONE_MINUS_SRC_ALPHA);
                }
                if (defaultTextureSize) {
                    gl.glEnable(GL2.GL_TEXTURE_GEN_S);
                    gl.glEnable(GL2.GL_TEXTURE_GEN_T);

                    gl.glTexGeni(GL2.GL_S, GL2.GL_TEXTURE_GEN_MODE, GL2.GL_OBJECT_LINEAR);
                    gl.glTexGenfv(GL2.GL_S, GL2.GL_OBJECT_PLANE, FloatBuffer.wrap(new float[]{1.f, 0.f, 0.f, 0.f}));

                    gl.glTexGeni(GL2.GL_T, GL2.GL_TEXTURE_GEN_MODE, GL2.GL_OBJECT_LINEAR);
                    gl.glTexGenfv(GL2.GL_T, GL2.GL_OBJECT_PLANE, FloatBuffer.wrap(new float[]{0.f, 1.f, 0.f, 0.f}));
                }
            } else if (diffuseColor[3] < 1.0f) {
                gl.glEnable(GL2.GL_BLEND);
                gl.glBlendFunc(GL2.GL_SRC_ALPHA, GL2.GL_ONE_MINUS_SRC_ALPHA);
            }
        }

        public void unset(final GL2 gl) {
            unset(gl, true);
        }

        public void unset(final GL2 gl, final boolean defaultTextureSize) {
            if (texture != null) {
                if (defaultTextureSize) {
                    gl.glDisable(GL2.GL_TEXTURE_GEN_S);
                    gl.glDisable(GL2.GL_TEXTURE_GEN_T);
                }
                gl.glBindTexture(GL2.GL_TEXTURE_2D, 0);
            }
            if (diffuseColor[3] < 1.0f || (texture != null && texture.hasAlpha)) {
                gl.glDisable(GL2.GL_BLEND);
            }
        }

    }

    /**
     * Parses the given ros2 file and returns its scene element. In case no
     * scene element exists, the &lt;Simulation&gt; root element is returned.
     *
     * @param filename path to the file to parse
     * @return Element representing the scene
     * @throws teamcomm.gui.drawings.RoSi2Element.RoSi2ParseException if the
     * file could not be parsed as a ros2 file
     * @throws javax.xml.stream.XMLStreamException if the file could not be
     * parsed as a XML file
     * @throws java.io.FileNotFoundException if the file could not be found
     * @throws java.io.IOException on other IO errors
     */
    public static RoSi2Element parseFile(final String filename) throws RoSi2ParseException, XMLStreamException, FileNotFoundException, IOException {
        // XML parser factory
        final XMLInputFactory factory = XMLInputFactory.newFactory();
        factory.setProperty(XMLInputFactory.IS_COALESCING, true);
        factory.setProperty(XMLInputFactory.IS_NAMESPACE_AWARE, false);

        // Stack containing the opened files
        final Deque<InputFileState> inputFileStack = new LinkedList<>();

        // Stack containing the current element hierarchy
        final Deque<RoSi2Element> parentStack = new LinkedList<>();

        // Map with mappings of all named elements
        final Map<String, RoSi2Element> namedElements = new HashMap<>();

        // Flag indicating whether the parser is within the scene element
        boolean withinSceneElement = false;

        // Open the given file
        inputFileStack.addFirst(new InputFileState(factory, new File(filename)));

        // Create the root element
        parentStack.addFirst(new RoSi2Element(inputFileStack.getFirst().path, "Simulation", namedElements));

        // Parse the file(s)
        while (!inputFileStack.isEmpty()) {
            while (inputFileStack.getFirst().reader.hasNext()) {

                final XMLEvent ev = inputFileStack.getFirst().reader.nextEvent();

                if (ev.isStartElement()) {
                    final StartElement e = ev.asStartElement();
                    final String tag = e.getName().getLocalPart();

                    if (tag.equals("Simulation")) {
                        // Start actual parsing after passing Simulation element
                        inputFileStack.getFirst().simulationTagPassed = true;
                    } else if (tag.equals("Include")) {
                        // Open the included file
                        inputFileStack.addFirst(new InputFileState(factory, new File(inputFileStack.getFirst().path.getParentFile(), getXmlAttribute(e, "href", true))));
                    } else if (inputFileStack.getFirst().simulationTagPassed) {
                        if (tag.equals("Set")) {
                            // Set variable binding
                            final String name = getXmlAttribute(e, "name", true);
                            if (!parentStack.getFirst().vars.containsKey(name)) {
                                parentStack.getFirst().vars.put(name, getXmlAttribute(e, "value", true));
                            }
                        } else {
                            // Create and add element
                            final String name = getXmlAttribute(e, "name", false);
                            @SuppressWarnings("unchecked")
                            final RoSi2Element elem = new RoSi2Element(inputFileStack.getFirst().path, tag, name, (Iterator<Attribute>) e.getAttributes(), namedElements);
                            if (name != null && !withinSceneElement) {
                                namedElements.put(tag + '#' + name, elem);
                            }
                            if (!withinSceneElement && tag.equals("Scene")) {
                                withinSceneElement = true;
                            }

                            parentStack.getFirst().children.add(elem);
                            parentStack.addFirst(elem);
                        }
                    }
                } else if (ev.isAttribute()) {
                    parentStack.getFirst().attributes.put(((Attribute) ev).getName().getLocalPart(), ((Attribute) ev).getValue());
                } else if (ev.isCharacters()) {
                    final Characters e = ev.asCharacters();
                    if (!e.isWhiteSpace()) {
                        parentStack.getFirst().content.append(e.getData());
                    }
                } else if (ev.isEndElement()) {
                    final String tag = ev.asEndElement().getName().getLocalPart();
                    if (tag.equals(parentStack.getFirst().tag) && !tag.equals("Simulation")) {
                        if (tag.equals("Scene")) {
                            withinSceneElement = false;
                        }
                        parentStack.removeFirst();
                    }
                }
            }

            // Close the current file
            inputFileStack.pollFirst().close();
        }

        if (!(parentStack.size() == 1 && parentStack.getFirst().tag.equals("Simulation"))) {
            throw new RoSi2ParseException("File ended before parsing was complete");
        }

        // Find the Scene element
        for (final RoSi2Element cur : parentStack.getFirst().children) {
            if (cur.tag.equals("Scene")) {
                return cur;
            }
        }

        // If no Scene element exists, return the root instead
        return parentStack.getFirst();
    }

    private static String getXmlAttribute(final StartElement e, final String name, boolean required) throws RoSi2ParseException {
        final Attribute attr = e.getAttributeByName(new QName(name));
        if (attr == null) {
            if (required) {
                throw new RoSi2ParseException("Missing attribute " + name + " on " + e.getName().getLocalPart() + " tag.");
            }
            return null;
        }

        return attr.getValue();

    }

    /**
     * Exception thrown if a ros2 file could not be parsed.
     */
    public static class RoSi2ParseException extends Exception {

        private static final long serialVersionUID = 439895799292899819L;

        private RoSi2ParseException() {
        }

        private RoSi2ParseException(final String message) {
            super(message);
        }

    }

    private static class InputFileState {

        public final XMLEventReader reader;
        public final File path;
        public final FileInputStream stream;
        public boolean simulationTagPassed;

        public InputFileState(final XMLInputFactory factory, final File path) throws XMLStreamException, FileNotFoundException {
            File p;
            try {
                p = path.getCanonicalFile();
            } catch (IOException e) {
                p = path.getAbsoluteFile();
            }
            this.path = p;
            stream = new FileInputStream(path);
            reader = factory.createXMLEventReader(stream);
            simulationTagPassed = false;
        }

        public void close() throws XMLStreamException, IOException {
            reader.close();
            stream.close();
        }
    }
}
