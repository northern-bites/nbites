package edu.bowdoin.robocup.TOOL.WorldController;

/**
 * Class defines a particle to represent a particle for use with displaying
 * Monte Carlo Localization logs.
 */
public class MCLParticle
{
    // Class variables
    private float x, y, h; // Position estimate variables
    private float weight; // The current weight of the particle; ranged [0,1]

    // Constructors
    /**
     * @param x The x-value of the particle
     * @param y The y-value of the particle
     * @param h The heading-value of the particle
     * @param w The current weight of the particle
     */
    public MCLParticle(float x, float y, float h, float w)
    {
        this.x = x;
        this.y = y;
        this.h = h;
        this.weight = w;
    }

    // Getters
    public float getX() {return x;}
    public float getY() {return y;}
    public float getH() {return h;}
    public float getWeight() {return weight;}
}