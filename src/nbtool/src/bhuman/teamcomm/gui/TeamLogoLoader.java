package teamcomm.gui;

import data.Rules;
import data.Teams;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.util.HashMap;
import java.util.Map;
import javax.swing.ImageIcon;

/**
 * Singleton class managing the loading of team logos.
 *
 * @author Felix Thielke
 */
public class TeamLogoLoader {

    private static final int OPAQUENESS = (int) ((1 - 0.6) * 0xFF);

    private static final TeamLogoLoader instance = new TeamLogoLoader();
    private final Map<Integer, BufferedImage> logos = new HashMap<>();
    private final Map<Integer, BufferedImage> semiTransparentLogos = new HashMap<>();
    private final Map<Integer, ImageIcon> panelIcons = new HashMap<>();

    private TeamLogoLoader() {
    }

    /**
     * Returns the only instance of the TeamLogoLoader.
     *
     * @return instance
     */
    public static TeamLogoLoader getInstance() {
        return instance;
    }

    /**
     * Returns the logo of the given team.
     *
     * @param team team number
     * @return logo
     */
    public BufferedImage getTeamLogo(final int team) {
        BufferedImage image = logos.get(team);
        if (image == null) {
            try {
                if (team == 98 || team == 99) {
                    Rules.league = Rules.LEAGUES[1];
                } else {
                    Rules.league = Rules.LEAGUES[0];
                }
                image = Teams.getIcon(team);
            } catch (final NullPointerException | ArrayIndexOutOfBoundsException e) {
                return null;
            }

            if (image != null) {
                if (image.getType() != BufferedImage.TYPE_INT_ARGB) {
                    final BufferedImage temp = new BufferedImage(image.getWidth(), image.getHeight(), BufferedImage.TYPE_INT_ARGB);
                    final Graphics g = temp.createGraphics();
                    g.drawImage(image, 0, 0, null);
                    g.dispose();
                    image = temp;
                }
                logos.put(team, image);
            }
        }

        return image;
    }

    /**
     * Returns an icon containing the logo of the given team.
     *
     * @param team team number
     * @return icon
     */
    public ImageIcon getTeamLogoIcon(final int team) {
        final BufferedImage image = getTeamLogo(team);
        return image == null ? null : new ImageIcon(image);
    }

    /**
     * Returns a scaled icon containing the logo of the given team.
     *
     * @param team team number
     * @param width width to scale the logo to
     * @param height height to scale the logo to
     * @return icon
     */
    public ImageIcon getScaledTeamLogoIcon(final int team, final int width, final int height) {
        final BufferedImage image = getTeamLogo(team);
        if (image == null) {
            return null;
        } else {
            final double scaleFactor = Math.min((double) width / image.getWidth(null), (double) height / image.getHeight(null));;
            return new ImageIcon(image.getScaledInstance(
                    (int) (image.getWidth(null) * scaleFactor),
                    (int) (image.getHeight(null) * scaleFactor),
                    Image.SCALE_SMOOTH));
        }
    }

    /**
     * Returns a semi-transparent scaled icon containing the logo of the given
     * team.
     *
     * @param team team number
     * @param width width to scale the logo to
     * @param height height to scale the logo to
     * @return icon
     */
    public ImageIcon getScaledSemiTransparentTeamLogoIcon(final int team, final int width, final int height) {
        BufferedImage image = semiTransparentLogos.get(team);
        if (image == null) {
            image = getTeamLogo(team);
            if (image == null) {
                return null;
            }

            final int[] imagePixels = image.getRGB(0, 0, image.getWidth(), image.getHeight(), null, 0, image.getWidth());
            for (int i = 0; i < imagePixels.length; i++) {
                final int argb = imagePixels[i];
                imagePixels[i] = (argb & 0x00FFFFFF) | (Math.max(0, (argb >>> 24) - OPAQUENESS) << 24);
            }
            image.setRGB(0, 0, image.getWidth(), image.getHeight(), imagePixels, 0, image.getWidth());
            semiTransparentLogos.put(team, image);
        }

        final double scaleFactor = Math.min((double) width / image.getWidth(null), (double) height / image.getHeight(null));;
        return new ImageIcon(image.getScaledInstance(
                (int) (image.getWidth(null) * scaleFactor),
                (int) (image.getHeight(null) * scaleFactor),
                Image.SCALE_SMOOTH));
    }

    /**
     * Returns an icon containing the logo of the given team which is scaled to
     * the size of a robot panel.
     *
     * @param team team number
     * @return icon
     */
    public ImageIcon getTeamLogoPanelIcon(final int team) {
        ImageIcon icon = panelIcons.get(team);
        if (icon == null) {
            icon = getScaledTeamLogoIcon(team, RobotPanel.PANEL_WIDTH, RobotPanel.PANEL_HEIGHT);
            if (icon != null) {
                panelIcons.put(team, icon);
            }
        }
        return icon;
    }
}
