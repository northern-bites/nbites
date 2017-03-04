def updateGameState(player, interface):
	stateDict = {0: "initial", 1: "ready", 2: "set", 3: "playing", 4: "finished"}
	print stateDict[interface.gameState.state]
	if(stateDict[interface.gameState.state] != player.gsmMachine.state):
		print "SWITCHING"
		getattr(player.gsmMachine, 'to_' + stateDict[interface.gameState.state])()