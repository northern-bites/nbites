package TOOL.WorldController;

public class RobotModel extends Robot {

	public RobotModel(int team, int color, int number){
		super(team, color, number, true);
	}

	public RobotModel(int team, int color, int number,
					  RobotData data){
		super(team, color, number, data, true);
	}
}