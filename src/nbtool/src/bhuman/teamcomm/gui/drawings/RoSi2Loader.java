package teamcomm.gui.drawings;

import com.jogamp.opengl.GL2;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import javax.swing.JOptionPane;
import javax.xml.stream.XMLStreamException;

/**
 * Singleton class for managing the loading of models from ros2 files.
 *
 * @author Felix Thielke
 */
public class RoSi2Loader {

    /**
     * Path to the file containing models for the default drawings.
     */
    public static final String COMMON_SCENE_FILE = "scene/TeamComm.ros2";

    private static final RoSi2Loader instance = new RoSi2Loader();
    private final Map<GL2, Map<String, Map<String, Integer>>> models = new HashMap<>();
    private final Map<GL2, Map<String, Set<String>>> modelsToLoad = new HashMap<>();

    private RoSi2Loader() {
    }

    /**
     * Returns the only instance of the RoSi2Loader.
     *
     * @return instance
     */
    public static RoSi2Loader getInstance() {
        return instance;
    }

    /**
     * Hints that the given models should be loaded from the common scene file.
     * This way the file has to be parsed only once when loading the models.
     *
     * @param gl OpenGL context
     * @param modelnames names of the models to load
     */
    public void cacheModels(final GL2 gl, final String[] modelnames) {
        cacheModels(gl, COMMON_SCENE_FILE, modelnames);
    }

    /**
     * Hints that the given models should be loaded from the given scene file.
     * This way the file has to be parsed only once when loading the models.
     * Calling this method is advised in the init()-method of drawings.
     *
     * @param gl OpenGL context
     * @param filename path to the scene file
     * @param modelnames names of the models to load
     */
    public void cacheModels(final GL2 gl, final String filename, final String... modelnames) {
        final Set<String> modelNameSet = new HashSet<>(Arrays.asList(modelnames));

        // Check if some of the models have already been loaded
        final Map<String, Map<String, Integer>> modelFileMap = models.get(gl);
        if (modelFileMap != null) {
            final Map<String, Integer> modelMap = modelFileMap.get(filename);
            if (modelMap != null) {
                final Iterator<String> iter = modelNameSet.iterator();
                while (iter.hasNext()) {
                    if (modelMap.containsKey(iter.next())) {
                        iter.remove();
                    }
                }
            }
        }

        // Add models to the set of models to load
        if (!modelNameSet.isEmpty()) {
            Map<String, Set<String>> fileMap = modelsToLoad.get(gl);
            if (fileMap == null) {
                fileMap = new HashMap<>();
                modelsToLoad.put(gl, fileMap);
            }
            Set<String> nameSet = fileMap.get(filename);
            if (nameSet == null) {
                fileMap.put(filename, modelNameSet);
            } else {
                nameSet.addAll(modelNameSet);
            }
        }
    }

    /**
     * Loads the given model from the common scene file into a display list of
     * the given OpenGL context.
     *
     * @param gl OpenGL context
     * @param modelname name of the model
     * @return id of the display list
     */
    public int loadModel(final GL2 gl, final String modelname) {
        return loadModel(gl, COMMON_SCENE_FILE, modelname);
    }

    /**
     * Loads the given model from the given scene file into a display list of
     * the given OpenGL context.
     *
     * @param gl OpenGL context
     * @param filename path to the scene file
     * @param modelname name of the model
     * @return id of the display list
     */
    public int loadModel(final GL2 gl, final String filename, final String modelname) {
        // Check if the model has already been loaded
        Map<String, Map<String, Integer>> fileMap = models.get(gl);
        if (fileMap == null) {
            fileMap = new HashMap<>();
            models.put(gl, fileMap);
        }
        Map<String, Integer> modelMap = fileMap.get(filename);
        if (modelMap == null) {
            modelMap = new HashMap<>();
            fileMap.put(filename, modelMap);
        } else {
            final Integer id = modelMap.get(modelname);
            if (id != null) {
                return id;
            }
        }

        // Determine models to load from the same file
        Map<String, Set<String>> loadFileMap = modelsToLoad.get(gl);
        if (loadFileMap == null) {
            loadFileMap = new HashMap<>();
            modelsToLoad.put(gl, loadFileMap);
        }
        Set<String> nameSet = loadFileMap.get(filename);
        if (nameSet == null) {
            nameSet = new HashSet<>();
            loadFileMap.put(filename, nameSet);
        }
        nameSet.add(modelname);

        // Load models
        try {
            for (final RoSi2Element e : RoSi2Element.parseFile(filename).findElements(nameSet)) {
                modelMap.put(e.getName(), e.instantiate(gl).createDisplayList());
            }
        } catch (RoSi2Element.RoSi2ParseException | XMLStreamException | IOException ex) {
            JOptionPane.showMessageDialog(null,
                    ex.getMessage(),
                    "Error loading scene",
                    JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }
        nameSet.clear();

        return modelMap.get(modelname);
    }
}
