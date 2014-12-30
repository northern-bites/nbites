package nbclient.term;

import java.util.ArrayList;

import nbclient.data.Log;
import nbclient.io.NetIO;
import nbclient.io.NetIO.Boss;
import nbclient.util.U;

public class term implements NetIO.Boss{
	
	NetIO netio;
	ArrayList<Log> logs;
	
	public static void main(String[] args) throws InterruptedException {
		//System.out.println(args.length);
		term t = new term();
		t.netio = new NetIO();
		t.netio.server_address = args[0];
		t.netio.server_port = Integer.parseInt(args[1]);
		t.netio.running = true;
		
		t.netio.boss = t;
		
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
		// TODO Auto-generated method stub
		
	}
}
