
// This file is part of TOOL, a robotics interaction and development
// package created by the Northern Bites RoboCup team of Bowdoin College
// in Brunswick, Maine.
//
// TOOL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TOOL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TOOL.  If not, see <http://www.gnu.org/licenses/>.

package edu.bowdoin.robocup.TOOL.Vision;

import java.awt.Color;

import edu.bowdoin.robocup.TOOL.Data.DataSet;
import edu.bowdoin.robocup.TOOL.Data.Frame;
import edu.bowdoin.robocup.TOOL.Data.RobotDef;

public interface Vision {

    public static Vision[] VISION_BINARIES =
            new Vision[RobotDef.ROBOT_DEFS.length];

    //current color encodings.
    public static final byte GREY = 0;
    public static final byte WHITE = 1;
    public static final byte GREEN = 2;
    public static final byte BLUE = 3;
    public static final byte YELLOW = 4;
    public static final byte ORANGE = 5;
    public static final byte YELLOWWHITE = 6;
    public static final byte BLUEGREEN = 7;
    public static final byte ORANGERED = 8;
    public static final byte ORANGEYELLOW = 9;
    public static final byte RED = 10;
    public static final byte NAVY = 11;
    public static final byte BLACK = 12;
    public static final byte PINK = 13;
    public static final byte SHADOW = 14;
    public static final byte CYAN = 15;
    public static final byte DARK_TURQUOISE = 16;
    public static final byte LAWN_GREEN = 17;
    public static final byte PALE_GREEN = 18;
    public static final byte BROWN = 19;
    public static final byte SEA_GREEN = 20;
    public static final byte ROYAL_BLUE = 21;
    public static final byte POWDER_BLUE = 22;
    public static final byte MEDIUM_PURPLE = 23;
    public static final byte MAROON = 24;
    public static final byte LIGHT_SKY_BLUE = 25;
    public static final byte MAGENTA = 26;
    public static final byte PURPLE = 27;

    public static final String[] COLOR_STRINGS = {
	"Undefined", "White", "Green", "Blue",
	"Yellow", "Orange", "Yellow/White", "Blue/Green",
	"Orange/Red", "Orange/Yellow", "Red", "Navy",
	"Black", "Pink","Shadow","Cyan","DarkTurquoise","LawnGreen","PaleGreen",
    "Brown","SeaGreen","RoyalBlue","PowderBlue","MediumPurple","Maroon",
    "LightSkyBlue","Magenta","Purple"};

    // Copied from CortexVision.h
    public static final Color GRAY_COLOR = new Color(100,100,100);
    public static final Color WHITE_COLOR = new Color(255,255,255);
    public static final Color GREEN_COLOR = new Color(0,125,50);
    public static final Color BLUE_COLOR = Color.BLUE;// new Color(51,102,255); // BLUE 3
    public static final Color YELLOW_COLOR = new Color(255,255,0); // YELLOW 4
    public static final Color ORANGE_COLOR = new Color(255, 140, 0);//new Color(255,80,0); // ORANGE 5
    public static final Color YELLOW_WHITE_COLOR = new Color(255,255,153);// YELLOWWHITE 6
    public static final Color BLUE_GREEN_COLOR = new Color(0,204,204); // BLUEGREEN 7
    public static final Color ORANGE_RED_COLOR = new Color(255,69,0);// ORANGERED 8
    public static final Color ORANGE_YELLOW_COLOR = new Color(255,204,0); // ORANGEYELLOW 9
    public static final Color RED_COLOR = Color.RED; // RED 10
    public static final Color NAVY_COLOR = Color.BLUE.darker(); // NAVY 11
    public static final Color BLACK_COLOR = new Color(0,0,0); // BLACK 12
    public static final Color PINK_COLOR = new Color(255,0,200); // BALL_PINK 13
    //public static final Color BOTHWHITE_COLOR = new Color(200,200,200); // BOTHWHITE 14
    public static final Color SHADOW_COLOR = Color.DARK_GRAY; // SHADOW 14
    public static final Color CYAN_COLOR = new Color(0,0,255); // CYAN 15
  // Generated from http://www.enetplace.com/color-picker.html
    public static final Color DARK_TURQUOISE_COLOR = new Color(0,206,209); // DARK TURQUOISE 16
    public static final Color LAWN_GREEN_COLOR = new Color(124,252,0); // LAWN GREEN 17
    public static final Color PALE_GREEN_COLOR = new Color(152,251,152); // PALE GREEN 18
    public static final Color BROWN_COLOR = new Color(139,69,19); // BROWN 19
    public static final Color SEA_GREEN_COLOR = new Color(46,139,87); // SEA GREEN 20
    public static final Color ROYAL_BLUE_COLOR = new Color(65,105,225); // ROYALBLUE 21
    public static final Color POWDER_BLUE_COLOR = new Color(176,224,230); //POWDERBLUE 22
    public static final Color MEDIUM_PURPLE_COLOR = new Color(147,112,219); //MEDIUMPURPLE 23
    public static final Color MAROON_COLOR = new Color(128,0,0); // MAROON 24
    public static final Color LIGHT_SKY_BLUE_COLOR = new Color(135,206,250); // LIGHT SKY BLUE 25
    public static final Color MAGENTA_COLOR = new Color(255,0,255); // MAGENTA 26
    public static final Color PURPLE_COLOR = new Color(128,0,128); // PURPLE 27



    /*
    public static final Color LWCOLOR = new Color(100, 100, 100);
    public static final Color OYCOLOR = new Color(180, 140, 30);
    public static final Color BGCOLOR = new Color(0, 204, 204);
    public static final Color YWCOLOR = new Color(255, 255, 153);
    public static final Color ORCOLOR = new Color(169, 69, 19);
    */

    public static final Color[] COLORS = {GRAY_COLOR,
                                          WHITE_COLOR,
                                          GREEN_COLOR,
                                          BLUE_COLOR,
                                          YELLOW_COLOR,
                                          ORANGE_COLOR,
                                          YELLOW_WHITE_COLOR,
                                          BLUE_GREEN_COLOR,
                                          ORANGE_RED_COLOR,
                                          ORANGE_YELLOW_COLOR,
                                          RED_COLOR,
                                          NAVY_COLOR,
                                          BLACK_COLOR,
                                          PINK_COLOR,
                                          SHADOW_COLOR,
                                          CYAN_COLOR,
                                          DARK_TURQUOISE_COLOR,
                                          LAWN_GREEN_COLOR,
                                          PALE_GREEN_COLOR,
                                          BROWN_COLOR,
                                          SEA_GREEN_COLOR,
                                          ROYAL_BLUE_COLOR,
                                          POWDER_BLUE_COLOR,
                                          MEDIUM_PURPLE_COLOR,
                                          MAROON_COLOR,
                                          LIGHT_SKY_BLUE_COLOR,
                                          MAGENTA_COLOR,
                                          PURPLE_COLOR};


    //current table sizes
    public static int YSHIFT = 1;
    public static int USHIFT = 1;
    public static int VSHIFT = 1;

    public static int YMAX = (int)(256>>YSHIFT);
    public static int UMAX = (int)(256>>USHIFT);
    public static int VMAX = (int)(256>>VSHIFT);


    //
    // Vision frame processing contract
    //

    public void processFrame(Frame f);
    public void processSequence(DataSet s);

}
