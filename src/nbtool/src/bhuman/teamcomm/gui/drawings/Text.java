package teamcomm.gui.drawings;

import com.jogamp.opengl.GL2;
import java.io.File;
import java.io.IOException;
import java.nio.FloatBuffer;
import java.nio.charset.Charset;
import javax.swing.JOptionPane;
import teamcomm.gui.drawings.TextureLoader.Texture;

/**
 * Helper class for drawing text.
 *
 * @author Felix Thielke
 */
public class Text {

    private static final int CHAR_WIDTH_PX = 42;
    private static final int CHAR_HEIGHT_PX = 82;
    private static final double CHAR_PX_RATIO = (double) CHAR_WIDTH_PX / (double) CHAR_HEIGHT_PX;

    /**
     * Draw the given text at the given position. Only characters in the ASCII
     * charset are allowed, others are ignored.
     *
     * @param gl OpenGL context
     * @param text text to draw
     * @param centerX center X coordinate at which the text is drawn
     * @param centerY center Y coordinate at which the text is drawn
     * @param size height of the font
     */
    public static void drawText(final GL2 gl, final String text, final float centerX, final float centerY, final float size) {
        drawText(gl, text, centerX, centerY, size, new float[]{1, 1, 1, 1});
    }

    /**
     * Draw the given text at the given position. Only characters in the ASCII
     * charset are allowed, others are ignored.
     *
     * @param gl OpenGL context
     * @param text text to draw
     * @param centerX center X coordinate at which the text is drawn
     * @param centerY center Y coordinate at which the text is drawn
     * @param size height of the font
     * @param color array with rgb or rgba values describing the color of the
     * text (color values are in the range [0.0f,1.0f])
     */
    public static void drawText(final GL2 gl, final String text, final float centerX, final float centerY, final float size, final float[] color) {
        final FloatBuffer colorBuffer;
        if (color.length > 4) {
            return;
        } else if (color.length < 4) {
            colorBuffer = FloatBuffer.allocate(4);
            for (final float c : color) {
                colorBuffer.put(c);
            }
            while (colorBuffer.position() < 4) {
                colorBuffer.put(1);
            }
        } else {
            colorBuffer = FloatBuffer.wrap(color);
        }

        // Load texture
        final Texture tex;
        try {
            tex = TextureLoader.getInstance().loadTexture(gl, new File("scene/Textures/ascii.png"));
        } catch (IOException ex) {
            // Handle error
            JOptionPane.showMessageDialog(null,
                    "Error loading texture",
                    "IOException",
                    JOptionPane.ERROR_MESSAGE);
            return;
        }

        // Set material
        gl.glColorMaterial(GL2.GL_FRONT, GL2.GL_AMBIENT_AND_DIFFUSE);
        gl.glColor4fv(colorBuffer);
        gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_SPECULAR, FloatBuffer.wrap(new float[]{0.0f, 0.0f, 0.0f, 1.0f}));
        gl.glMaterialf(GL2.GL_FRONT, GL2.GL_SHININESS, 0.0f);
        gl.glMaterialfv(GL2.GL_FRONT, GL2.GL_EMISSION, FloatBuffer.wrap(new float[]{0.0f, 0.0f, 0.0f, 1.0f}));
        gl.glBindTexture(GL2.GL_TEXTURE_2D, tex.id);
        if (tex.hasAlpha) {
            gl.glEnable(GL2.GL_BLEND);
            gl.glBlendFunc(GL2.GL_SRC_ALPHA, GL2.GL_ONE_MINUS_SRC_ALPHA);
        }

        final float charWidth = (float) ((double) size * CHAR_PX_RATIO);
        final float[] startOffset = new float[]{centerX - (text.length() * charWidth) / 2, centerY + size / 2};

        int n = 0;
        gl.glBegin(GL2.GL_QUADS);
        for (final byte b : text.getBytes(Charset.forName("ASCII"))) {
            if (b < 0x20 || b >= (0x20 + 95)) {
                continue;
            }
            final float[] texCoordsX = new float[]{(float) (b - 0x20) / 95.0f, (float) (b - 0x1F) / 95.0f};

            gl.glTexCoord2f(texCoordsX[0], 0);
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(startOffset[0] + (float) n * charWidth, startOffset[1], 0);

            gl.glTexCoord2f(texCoordsX[0], 1);
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(startOffset[0] + (float) n * charWidth, startOffset[1] - size, 0);

            gl.glTexCoord2f(texCoordsX[1], 1);
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(startOffset[0] + (float) (n + 1) * charWidth, startOffset[1] - size, 0);

            gl.glTexCoord2f(texCoordsX[1], 0);
            gl.glNormal3f(0, 0, 1);
            gl.glVertex3f(startOffset[0] + (float) (n + 1) * charWidth, startOffset[1], 0);

            n++;
        }
        gl.glEnd();

        // Unset material
        gl.glBindTexture(GL2.GL_TEXTURE_2D, 0);
        if (tex.hasAlpha) {
            gl.glDisable(GL2.GL_BLEND);
        }
    }
}
