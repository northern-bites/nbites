package teamcomm.gui.drawings;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.HashMap;
import java.util.Map;
import javax.imageio.ImageIO;

/**
 * Singleton class managing the loading of textures from image files.
 *
 * @author Felix Thielke
 */
public class TextureLoader {

    private static final TextureLoader instance = new TextureLoader();
    private final Map<GL, Map<String, Texture>> textures = new HashMap<>();

    /**
     * Class for storing info about a texture.
     */
    public static class Texture {

        /**
         * OpenGL texture id of the texture.
         */
        public final int id;

        /**
         * Flag indicating whether the texture has an alpha channel.
         */
        public final boolean hasAlpha;

        /**
         * Width of the texture in pixels.
         */
        public final int width;

        /**
         * Height of the texture in pixels.
         */
        public final int heigth;

        /**
         * Constructor.
         *
         * @param id OpenGL texture id of the texture
         * @param hasAlpha flag indicating whether the texture has an alpha
         * channel
         * @param width width of the texture in pixels
         * @param height height of the texture in pixels
         */
        public Texture(final int id, final boolean hasAlpha, final int width, final int height) {
            this.id = id;
            this.hasAlpha = hasAlpha;
            this.width = width;
            this.heigth = height;
        }

    }

    private TextureLoader() {
    }

    /**
     * Returns the only instance of the TextureLoader.
     *
     * @return instance
     */
    public static TextureLoader getInstance() {
        return instance;
    }

    /**
     * Loads a texture from the given file to the given OpenGL context.
     *
     * @param gl GL context
     * @param filename path of the image file
     * @return texture info
     * @throws IOException if the file could not be read as an image
     */
    public Texture loadTexture(final GL gl, final File filename) throws IOException {
        // Get the map for the given gl context
        Map<String, Texture> map = textures.get(gl);
        if (map == null) {
            map = new HashMap<>();
            textures.put(gl, map);
        }

        // Check if the texture has already been loaded
        Texture tex = map.get(filename.getAbsolutePath());
        if (tex != null) {
            return tex;
        }

        // Load the image
        final BufferedImage img = ImageIO.read(filename);
        final int[] imageData = img.getRGB(0, 0, img.getWidth(), img.getHeight(), null, 0, img.getWidth());
        final boolean hasAlpha = img.getColorModel().hasAlpha();
        final ByteBuffer buffer = ByteBuffer.allocate(imageData.length * (hasAlpha ? 4 : 3));
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        for (final int c : imageData) {
            if (hasAlpha) {
                buffer.putInt(c);
            } else {
                buffer.put((byte) (c & 0xFF));
                buffer.put((byte) ((c >> 8) & 0xFF));
                buffer.put((byte) ((c >> 16) & 0xFF));
            }
        }
        buffer.rewind();

        // Allocate texture
        final IntBuffer texIds = IntBuffer.allocate(1);
        gl.glGenTextures(1, texIds);
        final int textureId = texIds.get(0);

        // Load texture into GL
        gl.glBindTexture(GL.GL_TEXTURE_2D, textureId);
        gl.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_NEAREST);
        gl.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_NEAREST);
        gl.glTexImage2D(GL.GL_TEXTURE_2D, 0, hasAlpha ? 4 : 3, img.getWidth(), img.getHeight(), 0, hasAlpha ? GL.GL_BGRA : GL2.GL_BGR, GL.GL_UNSIGNED_BYTE, buffer);
        gl.glBindTexture(GL.GL_TEXTURE_2D, 0);
        tex = new Texture(textureId, hasAlpha, img.getWidth(), img.getHeight());
        map.put(filename.getAbsolutePath(), tex);

        return tex;
    }
}
