package nbtool.term;

import java.util.ArrayList;

import nbtool.data.Log;
import nbtool.io.NetIO;
import nbtool.io.NetIO.Boss;
import nbtool.util.U;

public class term implements NetIO.Boss{
	
	NetIO netio;
	ArrayList<Log> logs;
	
	public static void main(String[] args) throws InterruptedException {
		//System.out.println(args.length);
		term t = new term();
		t.netio = new NetIO(args[0], Integer.parseInt(args[1]), t);
		
		Thread thrd = new Thread(t.netio);
		
		thrd.start();
		
		while (true) {
			Thread.sleep(1000);
			if (!thrd.isAlive()) { U.w("THREAD DEAD!"); return;}
		}
	}

	
	public void takeDelivery(Log log) {
		System.out.println("" + ((Log) log).description);
	}


	@Override
	public void netThreadExiting() {
		U.w("netio exiting!");
		System.exit(1);
	}
}
