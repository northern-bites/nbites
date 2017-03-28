def updateGameState(player, interface):
	stateDict = {0: "GameInitial", 1: "GameReady", 2: "GameSet", 3: "GamePlaying", 4: "GameFinished"}
	if(stateDict[interface.gameState.state] != player.gsm.currentStateName):
		player.gsm.switchToState(stateDict[interface.gameState.state])