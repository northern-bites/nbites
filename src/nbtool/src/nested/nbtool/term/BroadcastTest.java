package nbtool.term;

import nbtool.io.BroadcastIO;
import nbtool.io.BroadcastIO.TeamBroadcastInstance;

public class BroadcastTest {

	public static void main(String[] args) {
		TeamBroadcastInstance tbl = new TeamBroadcastInstance(BroadcastIO.NBITES_TEAM_PORT);
		tbl.run();
	}



}
