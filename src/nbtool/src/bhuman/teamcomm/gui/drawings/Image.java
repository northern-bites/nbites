package teamcomm.gui.drawings;

import com.jogamp.opengl.GL2;
import java.nio.FloatBuffer;

/**
 * Helper class for drawing images.
 *
 * @author Felix Thielke
 */
public class Image {

    /**
     * Draw the given texture at the given position.
     *
     * @param gl OpenGL context
     * @param texture texture to draw
     * @param centerX center X coordinate at which the text is drawn
     * @param centerY center Y coordinate at which the text is drawn
     * @param size height of the image
     */
    public static void drawImage(final GL2 gl, final TextureLoader.Texture texture, final float centerX, final float centerY, final float size) {
        // Set material
        gl.glColorMaterial(GL2.GL_FRONT, GL2.GL_AMBIENT_AND_DIFFUSE);
        gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_SPECULAR, FloatBuffer.wrap(new float[]{0.0f, 0.0f, 0.0f, 1.0f}));
        gl.glMaterialf(GL2.GL_FRONT, GL2.GL_SHININESS, 0.0f);
        gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_EMISSION, FloatBuffer.wrap(new float[]{0.0f, 0.0f, 0.0f, 1.0f}));
        gl.glBindTexture(GL2.GL_TEXTURE_2D, texture.id);
        if (texture.hasAlpha) {
            gl.glEnable(GL2.GL_BLEND);
            gl.glBlendFunc(GL2.GL_SRC_ALPHA, GL2.GL_ONE_MINUS_SRC_ALPHA);
        }

        final float imageWidth = (float) ((double) size * (double) texture.width / (double) texture.heigth);
        final float[] offset = new float[]{centerX - imageWidth / 2, centerY + size / 2};

        gl.glNormal3f(0, 0, 1);
        gl.glColor3f(1, 1, 1);
        gl.glBegin(GL2.GL_QUADS);
        gl.glTexCoord2f(0, 0);
        gl.glVertex2f(offset[0], offset[1]);

        gl.glTexCoord2f(0, 1);
        gl.glVertex2f(offset[0], offset[1] - size);

        gl.glTexCoord2f(1, 1);
        gl.glVertex2f(offset[0] + imageWidth, offset[1] - size);

        gl.glTexCoord2f(1, 0);
        gl.glVertex2f(offset[0] + imageWidth, offset[1]);
        gl.glEnd();

        // Unset material
        gl.glBindTexture(GL2.GL_TEXTURE_2D, 0);
        if (texture.hasAlpha) {
            gl.glDisable(GL2.GL_BLEND);
        }
    }
}
