/**
 * Class defines a particle to represent a particle for use with displaying
 * Monte Carlo Localization logs.
 */

public class MCLParticle
{
    // Class variables
    private int x, y, h; // Position estimate variables
    private float w; // The current weight of the particle; ranged [0,1]

    // Constructors
    /**
     * @param x The x-value of the particle
     * @param y The y-value of the particle
     * @param h The heading-value of the particle
     * @param w The current weight of the particle
     */
    public MCLParticle(int x, int y, int h, float w)
    {
        this.x = x;
        this.y = y;
        this.h = h;
        this.w = w;
    }

    // Getters
    public int getX() {return x;}
    public int getY() {return y;}
    public int getH() {return h;}
}