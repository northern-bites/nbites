package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JSlider;

public class HardlyWorking extends UtilityParent {
	
	public HardlyWorking() {
		super();
		
		setSize(800, 800);
	}

	public Object getCurrentValue() {
		return new Integer(42);
	}
}
