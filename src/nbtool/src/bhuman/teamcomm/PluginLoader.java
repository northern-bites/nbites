package teamcomm;

import common.Log;
import data.SPLStandardMessage;
import java.io.File;
import java.io.FilenameFilter;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import teamcomm.data.AdvancedMessage;
import teamcomm.gui.drawings.Drawing;
import teamcomm.gui.drawings.PerPlayer;
import teamcomm.gui.drawings.Static;

/**
 * Singleton class for managing the loading of plugins.
 *
 * @author Felix Thielke
 */
public class PluginLoader {

    private static final String PLUGIN_PATH = "plugins/";
    private static final String COMMON_DRAWINGS_PLUGIN = "common.jar";

    /**
     * Teamnumber used for common drawings.
     */
    public static final int TEAMNUMBER_COMMON = -1;

    private static final PluginLoader instance = new PluginLoader();

    private final File pluginDir = new File(PLUGIN_PATH);
    private final Map<Integer, Class<? extends AdvancedMessage>> messageClasses = new HashMap<>();
    private final Map<Integer, Collection<Drawing>> drawings = new HashMap<>();

    private PluginLoader() {
        scanJar(new File(pluginDir, COMMON_DRAWINGS_PLUGIN), TEAMNUMBER_COMMON);
    }

    /**
     * Returns the only instance of the plugin loader.
     *
     * @return instance
     */
    public static PluginLoader getInstance() {
        return instance;
    }

    /**
     * Returns the class of messages from the given team.
     *
     * @param teamNumber number of the team
     * @return class for instantiating messages from the given team
     */
    public Class<? extends SPLStandardMessage> getMessageClass(final int teamNumber) {
        final Class<? extends AdvancedMessage> c = messageClasses.get(teamNumber);

        return c != null ? c : SPLStandardMessage.class;
    }

    /**
     * Returns drawings for all teams.
     *
     * @return drawings
     */
    public Collection<Drawing> getCommonDrawings() {
        return getDrawings(TEAMNUMBER_COMMON);
    }

    /**
     * Returns drawings for the given team.
     *
     * @param teamNumber number of the team
     * @return drawings
     */
    public Collection<Drawing> getDrawings(final int teamNumber) {
        final Collection<Drawing> ds = drawings.get(teamNumber);

        return ds != null ? ds : new ArrayList<Drawing>(0);
    }

    /**
     * Dynamically loads/reloads plugins of the given teams.
     *
     * @param teamNumbers numbers of the teams
     */
    public void update(final Integer... teamNumbers) {
        update(new HashSet<>(Arrays.asList(teamNumbers)));
    }

    /**
     * Dynamically loads/reloads plugins of the given teams.
     *
     * @param teamNumbers numbers of the teams
     */
    public void update(final Set<Integer> teamNumbers) {
        // Disallow reloading of plugins
        // (maybe use a ServiceLoader in the future to allow reloading)
        final Iterator<Integer> iter = teamNumbers.iterator();
        while (iter.hasNext()) {
            final int teamNumber = iter.next();
            if (messageClasses.get(teamNumber) == null) {
                final Collection<Drawing> ds = drawings.get(teamNumber);
                if (ds != null && !ds.isEmpty()) {
                    iter.remove();
                }
            } else {
                iter.remove();
            }
        }

        // Find dirs that correspond to team numbers
        final File[] pluginDirs = pluginDir.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(final File dir, final String name) {
                try {
                    return teamNumbers.contains(Integer.parseInt(name));
                } catch (NumberFormatException e) {
                    return false;
                }
            }
        });

        // Find jar files
        for (final File pDir : pluginDirs) {
            if (!pDir.isDirectory()) {
                continue;
            }

            final int teamNumber = Integer.parseInt(pDir.getName());
            final LinkedList<File> dirs = new LinkedList<>();
            dirs.add(pDir);
            final List<File> jars = new LinkedList<>();

            // Scan plugin directory
            while (!dirs.isEmpty()) {
                final File dir = dirs.pollFirst();
                for (final File file : dir.listFiles()) {
                    if (file.isDirectory()) {
                        dirs.addLast(file);
                    } else if (file.isFile() && file.getName().endsWith(".jar")) {
                        jars.add(file);
                    }
                }
            }

            // Load jars
            for (final File file : jars) {
                scanJar(file, teamNumber);
            }
        }
    }

    private void scanJar(final File file, final int teamNumber) {
        try {
            final Set<String> classNames;
            try (final JarFile jar = new JarFile(file)) {
                classNames = new HashSet<>();
                final Enumeration<JarEntry> entries = jar.entries();
                while (entries.hasMoreElements()) {
                    final JarEntry entry = entries.nextElement();
                    if (entry.getName().endsWith(".class")) {
                        classNames.add(entry.getName().substring(0, entry.getName().length() - 6).replaceAll("/", "\\."));
                    }
                }
            }

            // Load classes from jar
            try (final URLClassLoader loader = new URLClassLoader(new URL[]{file.toURI().toURL()})) {
                classLoop:
                for (final String className : classNames) {
                    final Class<?> cls = loader.loadClass(className);

                    if (AdvancedMessage.class.isAssignableFrom(cls)) {
                        // Class is a message class: set it as default if no
                        // other message class exists for the team
                        if (!messageClasses.containsKey(teamNumber)) {
                            try {
                                messageClasses.put(teamNumber, AdvancedMessage.class.cast(cls.newInstance()).getClass());
                            } catch (final Throwable e) {
                                Log.error(e.getClass().getSimpleName() + " was thrown while initializing custom message class " + cls.getName() + ": " + e.getMessage());
                            }
                        }
                    } else if (PerPlayer.class.isAssignableFrom(cls) || Static.class.isAssignableFrom(cls)) {
                        // Class is a drawing: add it to the team drawings
                        // if it does not yet exist
                        Collection<Drawing> drawingsForTeam = drawings.get(teamNumber);
                        if (drawingsForTeam == null) {
                            drawingsForTeam = new LinkedList<>();
                            drawings.put(teamNumber, drawingsForTeam);
                        }
                        for (final Drawing d : drawingsForTeam) {
                            if (cls.isInstance(d)) {
                                continue classLoop;
                            }
                        }
                        try {
                            final Drawing d = (Drawing) cls.newInstance();
                            d.setTeamNumber(teamNumber);
                            drawingsForTeam.add(d);
                        } catch (final Throwable e) {
                            Log.error(e.getClass().getSimpleName() + " was thrown while initializing custom drawing " + cls.getName() + ": " + e.getMessage());
                        }
                    }
                }
            }
        } catch (final Exception ex) {
            Log.error(ex.getClass().getSimpleName() + ": Could not open plugin " + file.getPath() + ": " + ex.getMessage());
        }
    }
}
