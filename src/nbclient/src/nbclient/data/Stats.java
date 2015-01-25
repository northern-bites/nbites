package nbclient.data;

public class Stats {
	public static final Stats INST = new Stats();
	private Stats() {
		l_found = s_found = 0;
		db_found = db_cur = db_dropped = 0;
	}	
	
	public int l_found, s_found;
	public long db_found, db_cur, db_dropped;
}
