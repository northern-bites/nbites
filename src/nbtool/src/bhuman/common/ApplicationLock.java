package common;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;

/**
 * @author Marcel Steinbeck
 *
 * This class is used to ensure, that only one instance of an application exists.
 */
public class ApplicationLock {
    /** The lockFile. */
    private File lockFile = null;

    /** The acquire. */
    private FileLock lock = null;

    /** The lockChannel. */
    private FileChannel lockChannel = null;

    /** The lockStream. */
    private FileOutputStream lockStream = null;

    /**
     * Creates a new ApplicationLock instance.
     * Every application instance gets it own key.
     *
     * @param key the key of the lock
     */
    public ApplicationLock(String key) {
        // ensure the path ends with system dependent file-separator
        String tmp_dir = System.getProperty("java.io.tmpdir");
        if (!tmp_dir.endsWith(System.getProperty("file.separator"))) {
            tmp_dir += System.getProperty("file.separator");
        }

        // create lock-file in tmp-dir
        lockFile = new File(tmp_dir + key + ".app_lock");
    }

    /**
     * Acquires a the lock.
     *
     * @return true if no other application acquired a lock before, false otherwise
     * @throws IOException if an error occurred while trying to lock
     */
    public boolean acquire() throws IOException {
        lockStream = new FileOutputStream(lockFile);
        lockChannel = lockStream.getChannel();
        lock = lockChannel.tryLock();
        return null != lock;

    }

    /**
     * Releases the lock
     *
     * @throws IOException if an error occurred while trying to unlock
     */
    public void release() throws IOException {
        if (lock.isValid()) {
            lock.release();
        }
        if (lockStream != null) {
            lockStream.close();
        }
        if (lockChannel.isOpen()) {
            lockChannel.close();
        }
    }
}