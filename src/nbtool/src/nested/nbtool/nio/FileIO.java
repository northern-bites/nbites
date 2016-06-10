package nbtool.nio;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.nio.file.DirectoryStream;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.LinkedList;
import java.util.List;

import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.util.Debug;
import nbtool.util.Utility.Pair;

public class FileIO {
	
	private static final Debug.DebugSettings debug = Debug.createSettings(Debug.WARN);
	
	public static Path[] getContentsOf(Path directory) {
		List<Path> pathes = new LinkedList<>();
		try (DirectoryStream<Path> directoryStream = Files.newDirectoryStream(directory)) {
            for (Path path : directoryStream) {
                pathes.add(path);
            }
        } catch (IOException ex) {
        	Debug.error("could not list directory: ", directory);
        	ex.printStackTrace();
        	return null;
        }
		
		return pathes.toArray(new Path[0]);
	}
	
	public static Path[] getContentsOf(Path directory, int limit) {
		assert(Files.isDirectory(directory));
		List<Path> pathes = new LinkedList<>();
		try (DirectoryStream<Path> directoryStream = Files.newDirectoryStream(directory)) {
            for (Path path : directoryStream) {
                pathes.add(path);
                
                if (pathes.size() == limit) { 
                	debug.info("traversal of dir:{%s} stopping at %d files!", directory, limit);
                	break;
                }
            }
        } catch (IOException ex) {
        	Debug.error("could not list directory: ", directory);
        	ex.printStackTrace();
        	return null;
        }
		
		return pathes.toArray(new Path[0]);
	}
	
	public static Path getPath(String first, String ... parts) {
		return FileSystems.getDefault().getPath(first, parts);
	}
	
	public static void sizeCheck(Path containsLog) throws IOException {
		long fsize = Files.size(containsLog);
		if (fsize < Log.MINIMUM_LOG_SIZE) {
			Debug.error("asked to read Log file below minimum size: %d < %d @ %s",
					fsize, Log.MINIMUM_LOG_SIZE, containsLog.toString());
			throw new RuntimeException("Log below minimum size threshold");
		}
	}
	
	public static BufferedInputStream inputStreamFrom(Path path) throws IOException {
		return new BufferedInputStream(Files.newInputStream(path,
				StandardOpenOption.READ));
	}
	
	public static BufferedOutputStream outputStreamTo(Path path) throws IOException {
		return new BufferedOutputStream(Files.newOutputStream(path,
				StandardOpenOption.WRITE, StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING));
	}
	
	public static Log readLogFromPath(Path path) throws IOException {
		sizeCheck(path);
		BufferedInputStream is = inputStreamFrom(path);
		Log ret = Log.parseFromStream(is);
		is.close();
		return ret;
	}
	
	public static LogReference readRefFromPath(Path path) throws IOException {
		sizeCheck(path);
		return LogReference.referenceFromFile(path);
	}
	
	public static void writeLogToPath(Path path, Log log) throws IOException {
		BufferedOutputStream os = outputStreamTo(path);
		log.writeTo(os);
		os.close();
	}
	
	public static Log[] readAllLogsFromPath(Path path) throws IOException {
		if (!isValidLogFolder(path)) {
			Debug.error("cannot read multiple logs from invalid path: ", path.toString());
			return null;
		}
		
		LinkedList<Log> found = new LinkedList<>();
		DirectoryStream<Path> dirStream = Files.newDirectoryStream(path, "*.{nblog}");
		for (Path file : dirStream) {
			found.add(readLogFromPath(file));
		}
		
		return found.toArray(new Log[0]);
	}
	
	public static LogReference[] readAllRefsFromPath(Path path, boolean quick) throws IOException {
		if (!isValidLogFolder(path)) {
			Debug.error("cannot read multiple refs from invalid path: ", path.toString());
			return null;
		}
		
		LinkedList<LogReference> found = new LinkedList<>();
		DirectoryStream<Path> dirStream = Files.newDirectoryStream(path, "*.{nblog}");
		for (Path file : dirStream) {
			sizeCheck(file);
			if (quick) {
				found.add(LogReference.quickReferenceFromFile(file));
			} else {
				found.add(LogReference.referenceFromFile(file));
			}
		}
		
		return found.toArray(new LogReference[0]);
	}
	
	public static String logDescriptionAt(Path path) throws IOException {
		sizeCheck(path);
		
		DataInputStream dis = new DataInputStream(inputStreamFrom(path));
		
		int descSize = dis.readInt();
		byte[] desc = new byte[descSize];
		dis.readFully(desc);
		dis.close();
		
		String ret = new String(desc);
		ret = ret.replace("\0", "");
		return ret;
	}
	
	public static boolean isValidLogFolder(Path path) {
		if(path == null || path.getNameCount() < 1) {
			Debug.error( "path string null or empty!\n");
			return false;
		}
			
		if (!Files.exists(path)) {
			Debug.error( "file does not exist! %s\n", path.toString());
			return false;
		}
		if (!Files.isDirectory(path)) {
			Debug.error( "file is not a directory! %s\n", path.toString());
			return false;
		}
		
		if (!Files.isReadable(path) || !Files.isWritable(path)) {
			Debug.error( "permissions errors for file %s!\n", path.toString());
			return false;
		}
		
		return true;
	}
	
	/* asynchronous file writing thread */
	
	public static void queueWriteTask(Path path, byte[] data) {
		assert(path != null && data != null);
		assert(!Files.exists(path) || Files.isRegularFile(path));
		
		addTaskToQueue(new Task(path, data));
	}
	
	public static void addTaskToQueue(Task t) {
		synchronized(writer.queue) {
			writer.queue.addLast(t);
			writer.queue.notify();
		}
	}
	
	public static class Task {
		protected byte[] data;
		protected Path path;
		
		protected Task(Path p, byte[] d) {
			this.path = p; this.data = d;
		}

		public void executeWrite() throws IOException { 
			Files.write(path, data,
					StandardOpenOption.WRITE, StandardOpenOption.CREATE,
					StandardOpenOption.TRUNCATE_EXISTING);
		}
	}
	
	public static void _NBL_REQUIRED_START_() {
		Debug.warn("starting FileWriter thread...");
		fileWriteThread.start();
	}
	
	private static final FileWriter writer = new FileWriter();
	private static final Thread fileWriteThread = new Thread(writer);
	
	private static class FileWriter implements Runnable {
		
		protected final LinkedList<Task> queue = new LinkedList<>();
		
		private void loop() {
			for (;;) {
				Task task = null;
				synchronized(queue) {
					for(;;) {
						if (queue.isEmpty()) {
							try {
								queue.wait();
							} catch (InterruptedException e) {
								e.printStackTrace();
							}
						} else {
							task = queue.removeFirst();
							break;
						}
					}
				}
				
				try {
					task.executeWrite();
				} catch (IOException e) {
					Debug.error("error writing file to path %s", task.path.toString());
					e.printStackTrace();
				}
			}
		}

		@Override
		public void run() {
			Debug.warn("FileWriter thread running.");
			try {
				loop();
			} catch (Exception e) {
				Debug.error("FileWriter thread ending...");
				e.printStackTrace();
				throw e;
			}
		}
		
	}
	
}
