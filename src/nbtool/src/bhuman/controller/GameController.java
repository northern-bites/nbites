package controller;

import common.ApplicationLock;
import common.Log;
import controller.action.ActionBoard;
import controller.net.GameControlReturnDataReceiver;
import controller.net.SPLCoachMessageReceiver;
import controller.net.Sender;
import controller.ui.GUI;
import controller.ui.KeyboardListener;
import controller.ui.StartInput;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;
import data.Teams;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.Pattern;

import javax.swing.*;

/**
 * @author Michel Bartsch
 * 
 * The programm starts in this class.
 * The main components are initialised here.
 */
public class GameController
{
    /**
     * The version of the GameController.
     * Actually there are no dependencies, but this should be the first thing
     * to be written into the log file.
     */
    public static final String version = "GC2 1.3";
    
    /** Relative directory of where logs are stored */
    private final static String LOG_DIRECTORY = "logs";
    
    private static Pattern IPV4_PATTERN = Pattern.compile("^(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}$");

    private static final String HELP_TEMPLATE = "Usage: java -jar GameController.jar {options}"
            + "\n  (-h | --help)                   display help"
            + "\n  (-b | --broadcast) <address>    set broadcast ip (default is 255.255.255.255)"
            + "\n  (-l | --league) %s%sselect league (default is spl)"
            + "\n  (-w | --window)                 select window mode (default is fullscreen)"
            + "\n";
    private static final String DEFAULT_BROADCAST = "255.255.255.255";
    private static final String COMMAND_BROADCAST = "--broadcast";
    private static final String COMMAND_BROADCAST_SHORT = "-b";
    private static final String COMMAND_LEAGUE = "--league";
    private static final String COMMAND_LEAGUE_SHORT = "-l";
    private static final String COMMAND_WINDOW = "--window";
    private static final String COMMAND_WINDOW_SHORT = "-w";
    
    /**
     * The program starts here.
     * 
     * @param args  This is ignored.
     */
    public static void main(String[] args)
    {
    	System.out.println("OVERRIDE");
        // Do not just System.exit(0) on Macs when selecting GameController/Quit
        System.setProperty("apple.eawt.quitStrategy", "CLOSE_ALL_WINDOWS");

        //commands
        String outBroadcastAddress = DEFAULT_BROADCAST;
        boolean windowMode = false;
        
        parsing:
        for (int i=0; i<args.length; i++) {
            if ((args.length > i+1)
                    && ((args[i].equalsIgnoreCase(COMMAND_BROADCAST_SHORT))
                    || (args[i].equalsIgnoreCase(COMMAND_BROADCAST)))
                    && IPV4_PATTERN.matcher(args[++i]).matches()) {
                outBroadcastAddress = args[i];
                continue parsing;
            } else if ((args.length > i+1)
                    && ((args[i].equalsIgnoreCase(COMMAND_LEAGUE_SHORT))
                    || (args[i].equalsIgnoreCase(COMMAND_LEAGUE))) ) {
                i++;
                for (int j=0; j < Rules.LEAGUES.length; j++) {
                    if (Rules.LEAGUES[j].leagueDirectory.equals(args[i])) {
                        Rules.league = Rules.LEAGUES[j];
                        continue parsing;
                    }
                }
            } else if (args[i].equals(COMMAND_WINDOW_SHORT) || args[i].equals(COMMAND_WINDOW)) {
                windowMode = true;
                continue parsing;
            }
            String leagues = "";
            for (Rules rules : Rules.LEAGUES) {
                leagues += (leagues.equals("") ? "" : " | ") + rules.leagueDirectory;
            }
            if (leagues.contains("|")) {
                leagues = "(" + leagues + ")";
            }
            System.out.printf(HELP_TEMPLATE, leagues, leagues.length() < 17
                              ? "                ".substring(leagues.length())
                              : "\n                                  ");
            System.exit(0);
        }
        
        //application-lock
        final ApplicationLock applicationLock = new ApplicationLock("GameController");
        try {
            if (!applicationLock.acquire()) {
                JOptionPane.showMessageDialog(null,
                        "An instance of GameController already exists.",
                        "Multiple instances",
                        JOptionPane.WARNING_MESSAGE);
                System.exit(0);
            }
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null,
                    "Error while trying to acquire the application lock.",
                    "IOError",
                    JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }

        //collect the start parameters and put them into the first data.
        StartInput input = new StartInput(!windowMode);
        while (!input.finished) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                System.exit(0);
            }
        }

        AdvancedData data = new AdvancedData();
        for (int i=0; i<2; i++) {
            data.team[i].teamNumber = (byte)input.outTeam[i];
        }
        data.team[0].teamColor = input.outTeamColor[0];
        data.team[1].teamColor = input.outTeamColor[1];
        data.kickOffTeam = (byte)input.outTeam[0];
        data.colorChangeAuto = input.outAutoColorChange;
        data.gameType = Rules.league.dropInPlayerMode ? GameControlData.GAME_DROPIN
                : input.outFulltime ? GameControlData.GAME_PLAYOFF : GameControlData.GAME_ROUNDROBIN;

        try {
            //sender
            Sender.initialize(outBroadcastAddress);
            Sender sender = Sender.getInstance();
            sender.send(data);
            sender.start();

            //event-handler
            EventHandler.getInstance().data = data;

            //receiver
            GameControlReturnDataReceiver receiver = GameControlReturnDataReceiver.getInstance();
            receiver.start();

            if (Rules.league.isCoachAvailable) {
                SPLCoachMessageReceiver spl = SPLCoachMessageReceiver.getInstance();
                spl.start();
            }
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null,
                    "Error while setting up GameController on port: " + GameControlData.GAMECONTROLLER_RETURNDATA_PORT + ".",
                    "Error on configured port",
                    JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }

        //log
        SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss-S");
        
        final File logDir = new File(LOG_DIRECTORY);
        if (!logDir.exists() && !logDir.mkdirs()) {
            Log.init("log_"+df.format(new Date(System.currentTimeMillis()))+".txt");
        } else {
            final File logFile = new File(logDir, 
                "log_"+df.format(new Date(System.currentTimeMillis()))+".txt");
            Log.init(logFile.getPath());
        }
        Log.toFile("League = "+Rules.league.leagueName);
        Log.toFile("Game type = "+ (data.gameType == GameControlData.GAME_ROUNDROBIN ? "round robin"
                : data.gameType == GameControlData.GAME_PLAYOFF ?  "play-off" : "drop-in"));
        Log.toFile("Auto color change = "+data.colorChangeAuto);
        Log.toFile("Using broadcast address " + outBroadcastAddress);

        //ui
        ActionBoard.init();
        Log.state(data, Teams.getNames(false)[data.team[0].teamNumber]
                + " (" + Rules.league.teamColorName[data.team[0].teamColor]
                + ") vs " + Teams.getNames(false)[data.team[1].teamNumber]
                + " (" + Rules.league.teamColorName[data.team[1].teamColor] + ")");
        GUI gui = new GUI(input.outFullscreen, data);
        new KeyboardListener();
        EventHandler.getInstance().setGUI(gui);
        gui.update(data);

        //input dispose
        input.dispose();

        //clock runs until window is closed
        Clock.getInstance().start();

        // shutdown
        Log.toFile("Shutdown GameController");
        try {
            applicationLock.release();
        } catch (IOException e) {
            Log.error("Error while trying to release the application lock.");
        }
        Sender.getInstance().interrupt();
        GameControlReturnDataReceiver.getInstance().interrupt();
        SPLCoachMessageReceiver.getInstance().interrupt();
        Thread.interrupted(); // clean interrupted status
        try {
            Sender.getInstance().join();
            GameControlReturnDataReceiver.getInstance().join();
            SPLCoachMessageReceiver.getInstance().join();
        } catch (InterruptedException e) {
            Log.error("Waiting for threads to shutdown was interrupted.");
        }
        try {
            Log.close();
        } catch (IOException e) {
            Log.error("Error while trying to close the log.");
        }

        gui.dispose();
        System.exit(0);
    }
}