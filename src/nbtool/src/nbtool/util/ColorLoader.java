package nbtool.util;

public class ColorLoader {
    float darkU;
    float darkV;
    float lightU;
    float lightV;
    float fuzzyU;
    float fuzzyV;

    public ColorLoader(float dU, float dV, float lU, float lV, float fU, float fV) {
        darkU = dU;
        darkV = dV;
        lightU = lU;
        lightV = lV;
        fuzzyU = fU;
        fuzzyV = fV;
    }

    public float getDarkU() { return darkU; }
    public void setDarkU(float u) { darkU = u; }

    public float getDarkV() { return darkV; }
    public void setDarkV(float v) { darkV = v; }

    public float getLightU() { return lightU; }
    public void setLightU(float u) { lightU = u; }

    public float getLightV() { return lightV; }
    public void setLightV(float v) { lightV = v; }

    public float getFuzzyU() { return fuzzyU; }
    public void setFuzzyU(float u) { fuzzyU = u; }

    public float getFuzzyV() { return fuzzyV; }
    public void setFuzzyV(float v) { fuzzyV = v; }

}