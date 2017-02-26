"""
Brunswick.py
Northern Bites Behavior File
"""

class Brunswick(Player):

	readyStates = ['localizing', 'goingToKickoffPos', 'inKickoffPos']
	setStates = ['lookingForBall', 'foundBall']
	playingStates = ['searchBehavior', 'walkingTowardBall', 'aiming', 'kicking',
		'followingKick', 'dribbling', 'followingDribble']

	transitions = [
		['shouldGoToKickoffPos', 'localizing',        'goingToKickoffPos'],
		['atKickoffPos',         'goingToKickoffPos', 'inKickoffPos'],
		['lostKickoffPos',       'inKickoffPos',      'goingToKickoffPos'],

		['canSeeBall', 'lookingForBall', 'foundBall'],
		['lostBall',   'foundBall',      'lookingForBall'],

		['canSeeBall', 'searchBehavior', 'walkingTowardBall'],
		['lostBall', playingStates, 'searchBehavior']
		['shouldAim', 'walkingTowardBall', 'aiming'],
		['shouldKick', 'aiming', 'kicking'],
		['shouldDribble', 'aiming', 'dribbling'],
		['shouldAimAgain', ['kicking', 'dribbling'], 'aiming']
	]

	def __init__(self):
		

	def run():
