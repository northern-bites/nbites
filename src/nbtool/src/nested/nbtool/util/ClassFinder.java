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

public class ClassFinder {

	public static void callAllInstancesOfStaticMethod(final String methodName) {
		
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
							
							for (Method m : fileClass.getDeclaredMethods()) {
								if (Modifier.isStatic(m.getModifiers()) &&
										m.getName().equals(methodName)) {
									Debug.print("ClassFinder: invoking %s on class %s",
											methodName, fileClass.getName());
									m.invoke(null);
								}
							}
							
						} catch (ClassNotFoundException e) {
							e.printStackTrace();
							return FileVisitResult.TERMINATE;
						} catch (IllegalAccessException e) {
							e.printStackTrace();
						} catch (IllegalArgumentException e) {
							e.printStackTrace();
						} catch (InvocationTargetException e) {
							e.printStackTrace();
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

	public static void main(String[] args) {
		Debug.print("searching...");
		
		callAllInstancesOfStaticMethod("_NBL_ADD_TESTS_");
	}
}
