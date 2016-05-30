package nbtool.util;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.net.URL;
import java.net.URLClassLoader;
import java.nio.file.FileSystems;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.LinkedList;
import java.util.List;

import nbtool.gui.logviews.misc.ViewParent;

public class ClassFinder {
	
	private static final Debug.DebugSettings debug =
			Debug.createSettings(true, true, true, Debug.INFO, null);
	
	interface ClassCallback {
		public boolean callback(Class<?> arg) throws Exception;
	}
	
	private static void nbitesClassCaller(final ClassCallback impl) {
		final Path baseLoadDir = FileSystems.getDefault().getPath(ToolSettings.NBITES_DIR,
				"/build/nbtool/");
		final Path baseDir = baseLoadDir.resolve("nbtool/");
		final int baseCount = baseDir.getNameCount() - 1;
		
		try {
			final URLClassLoader loader = new URLClassLoader(new URL[]{baseLoadDir.toUri().toURL()});

			Files.walkFileTree(baseDir, new SimpleFileVisitor<Path>(){
				@Override
				public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
				
					Path subPath = file.subpath(baseCount, file.getNameCount());
					String className = subPath.toString().replace('/', '.');
					
					if (className.endsWith(".class")) {
						className = className.substring(0, className.length() - ".class".length());
						
						try {
							Class<?> fileClass = loader.loadClass(className);
							boolean ok = impl.callback(fileClass);
							if (!ok) return FileVisitResult.TERMINATE;
						} catch (Exception e) {
							debug.error("nbitesClassCaller ending because %s: %s",
									e.getClass().getName(), e.getMessage());
							e.printStackTrace();
							return FileVisitResult.TERMINATE;
						}
						
					}
										
					return FileVisitResult.CONTINUE;
				}
			});
			
			loader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void callAllInstancesOfStaticMethod(final String methodName) {
		
		ClassFinder.nbitesClassCaller(new ClassCallback(){

			@Override
			public boolean callback(Class<?> arg) throws Exception {
				for (Method m : arg.getDeclaredMethods()) {
					if (Modifier.isStatic(m.getModifiers()) &&
							m.getName().equals(methodName)) {
						Debug.print("ClassFinder: invoking %s on class %s",
								methodName, arg.getName());
						m.invoke(null);
					}
				}
				
				return true;
			}
			
		});
	}
	
	public static List<Class<?>> findAllSubclasses(final Class<?> parent) {
		final LinkedList<Class<?>> found = new LinkedList<>();
		
		nbitesClassCaller(new ClassCallback(){
			@Override
			public boolean callback(Class<?> arg) throws Exception {
				
				if (parent.isAssignableFrom(arg) &&
						!parent.equals(arg) &&
						!Modifier.isAbstract(arg.getModifiers())) {
					
					found.add(arg);
				}
				
				return true;
			}
		});
		
		return found;
	}

	public static void main(String[] args) {
		Debug.print("searching...");
		
		ClassFinder.findAllSubclasses(ViewParent.class);
	}
}
