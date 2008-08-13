
from math import fabs

from . import PBDefs
from . import PBConstants
from . import Strategies

try:
    import potentialfield
except ImportError:
    import sys
    print >>sys.stderr, '****************************************************'
    print >>sys.stderr, '**** WARNING - no potentialfield module located ****'
    print >>sys.stderr, '****    GoTeam module will FAIL upon running!   ****'
    print >>sys.stderr, '****************************************************'


class GoTeam:
    """
    This is the class which controls all of our coordinated behavior system.  
    Should act as a replacement to the old PlayBook monolith approach
    """
    def __init__(self, brain):
        self.brain = brain
        self.useAttractor = False
        self.printStateChanges = True

        # Info about all of our states
        # Strategies
        self.currentStrategy = 'sInit'
        self.lastStrategy = 'sInit'
        self.lastDiffStrategy = 'sInit'
        self.strategyCounter = 0
        self.strategyStartTime = 0
        self.strategyTime = 0

        # Formations
        self.currentFormation = 'fInit'
        self.lastFormation = 'fInit'
        self.lastDiffFormation = 'fInit'
        self.formationCounter = 0
        self.formationStartTime = 0
        self.formationTime = 0

        # Roles
        self.currentRole = PBConstants.INIT_ROLE
	self.lastRole = PBConstants.INIT_ROLE
	self.lastDiffRole = PBConstants.INIT_ROLE
	self.roleCounter = 0
        self.roleStartTime = 0
        self.roleTime = 0

        # SubRoles
        self.currentSubRole = PBConstants.INIT_SUB_ROLE
	self.lastSubRole = PBConstants.INIT_SUB_ROLE
	self.lastDiffSubRole = PBConstants.INIT_SUB_ROLE
        self.subRoleCounter = 0
        self.subRoleStartTime = 0
        self.subRoleTime = 0

	self.subRoleOnDeck = PBConstants.INIT_SUB_ROLE
	self.lastSubRoleOnDeck = PBConstants.INIT_SUB_ROLE
	self.subRoleOnDeckCount = 0

        self.subRole = self.currentSubRole
        self.role = self.currentRole
        self.formation = self.currentFormation
        self.strategy = self.currentStrategy

        # Information about teammates
	self.teammates = []
        for i in xrange(PBConstants.NUM_TEAM_PLAYERS):
	    mate = PBDefs.Teammate(brain)
            mate.playerNumber = i + 1
            self.teammates.append(mate)
       
        self.position = []
        self.me = self.teammates[self.brain.my.playerNumber - 1]
        self.inactiveMates = []
        self.numInactiveMates = 0
        self.kickoffFormation = 0
	self.timeSinceCaptureChase = 0

	# potential field setup
	self.pf = potentialfield.PotentialField()
	self.pf.addFieldCharges()
	self.my_charge = None
	self.my_charge_type = None
	self.my_charge_pos = None


    def run(self):
        """
        We run this each frame to get the latest info
        """
        self.aPrioriTeammateUpdate()

        # We will always return a strategy
        self.currentStrategy, self.currentFormation, self.currentRole, \
            self.currentSubRole = self.strategize()

        # Update all of our new infos
        self.updateStateInfo()
        self.aPosterioriTeammateUpdate()

    def strategize(self):
        """
        Picks the strategy to run and returns all sorts of infos
        """
        # First we check for testing stuff
        if TEST_DEFENDER:
            return Strategies.sTestDefender(self)
        elif TEST_OFFENDER:
            return Strategies.sTestOffender(self)
        elif TEST_MIDDIE:
            return Strategies.sTestMiddie(self)
        elif TEST_CHASER:
            return Strategies.sTestChaser(self)

        # Now we look at shorthanded strategies
        elif self.numInactiveMates == 1:
            return Strategies.sOneDown(self)
        elif self.numInactiveMates == 2:
            return Strategies.sTwoDown(self)

        # Here we have the strategy stuff
        return Strategies.sSpread(self)

    def updateStateInfo(self):
        """
        Update information specific to the coordinated behaviors
        """
        # Update our counters!
        # Update Strategy Memory
	if self.lastStrategy != self.currentStrategy:
            if self.printStateChanges:
                self.printf("Strategy switched to "+
                            self.currentStrategy)
	    self.strategyCounter = 0
	    self.lastDiffStrategy = self.lastStrategy
	    self.strategyStartTime = self.getTime()
	    self.strategyTime = 0
	else:
	    self.strategyCounter += 1
            self.strategyTime = self.getTime() - self.strategyStartTime

        # Update Formations memory
	if self.lastFormation != self.currentFormation:
            if self.printStateChanges:
                self.printf("Formation switched to "+
                            self.currentFormation)
	    self.formationCounter = 0
	    self.lastDiffFormation = self.lastFormation
	    self.formationStartTime = self.getTime()
	    self.formationTime = 0
	else:
	    self.formationCounter += 1
            self.formationTime = self.getTime() - self.formationStartTime

        # Update memory of roles
	if self.lastRole != self.currentRole:
            if self.printStateChanges:
                self.printf("Role switched to "+
                            PBConstants.ROLES[self.currentRole])
	    self.roleCounter = 0
	    self.lastDiffRole = self.lastRole
	    self.roleStartTime = self.getTime()
	    self.roleTime = 0
            self.subRoleOnDeck = self.currentSubRole
            self.subRoleOnDeckCounter = 0
	else:
	    self.roleCounter += 1
            self.roleTime = self.getTime() - self.roleStartTime

        # We buffer the subRole switching by making sure that a subrole is 
        # returned for multiple frames in a row
        if (self.subRoleOnDeck and 
            self.subRoleOnDeck != self.currentSubRole):
            # if the sub role on deck is the same as the last, increment
            if self.subRoleOnDeck == self.lastSubRoleOnDeck:
                self.subRoleOnDeckCounter += 1
            # otherwise our idea of subRole has changed, null counter
            else:
                self.subRoleOnDeckCounter = 0

            # if we are sure that we want to switch into this sub role
            if (self.subRoleOnDeckCounter > 
                SUB_ROLE_SWITCH_BUFFER):
                #self.currentSubRole = self.subRoleOnDeck
                self.subRoleOnDeckCounter = 0

        # SubRoles
	if self.lastSubRole != self.currentSubRole:
            if self.printStateChanges:
                try:
                    self.printf("SubRole switched to "+
                                PBConstants.SUB_ROLES[self.currentSubRole])
                except:
                    raise ValueError("Incorrect currentSubRole:" + 
                                     str(self.currentSubRole))
	    self.roleCounter = 0
	    self.lastDiffSubRole = self.lastSubRole
	    self.subRoleStartTime = self.getTime()
	    self.subRoleTime = 0
	else:
	    self.subRoleCounter += 1
            self.subRoleTime = self.getTime() - self.subRoleStartTime

        # Get ready for the next frame
        self.lastStrategy = self.currentStrategy
        self.lastFormation = self.currentFormation
        self.lastRole = self.currentRole
        self.lastSubRole = self.currentSubRole
        self.lastSubRoleOnDeck = self.subRoleOnDeck

        # Robots are funnnn.... :(
        self.strategy = self.currentStrategy
        self.formation = self.currentFormation
        self.role = self.currentRole
        self.subRole = self.currentSubRole

    def getTime(self):
        return self.brain.getTime()

    def printf(self, string):
        print string

    def getAction(self):
	'''called by player behaviors. returns a list [x,y,h] for position and
	a role to play'''
        if self.isGoalie():
            self.aPrioriTeammateUpdate()
            return self.noCalledChaser()

        self.run()

	return list(self.position), self.currentStrategie, \
            self.currentFormation, self.currentRole, self.currentSubRole

    ######################################################
    ############       Role Switching Stuff     ##########
    ######################################################
    def determineChaser(self):
	'''return the player number of the chaser'''

	chaser_mate = self.teammates[self.me.playerNumber-1]

	# center field capture hack for defender
	if (self.currentSubRole == DEFENSIVE_MIDFIELD and
	    self.brain.getPosDist(self.position) < 125.0 and
	    0.0 < self.brain.ball.dist < 125.0 and
	    Constants.CENTER_FIELD_Y - 100.0 < self.brain.my.y < 
	    Constants.CENTER_FIELD_Y + 10.0 and
	    -90 < self.brain.my.h < 90):
	    self.timeSinceCaptureChase = self.brain.getTime()

	# center field capture hack for offender
	if (self.currentSubRole == CENTER_O_MIDFIELD and
	    self.brain.getPosDist(self.position) < 100.0 and
	    0.0 < self.brain.ball.dist < 125.0 and
	    Constants.CENTER_FIELD_Y - 10.0 < self.brain.my.y < 
	    Constants.CENTER_FIELD_Y + 100.0 and
	    (self.brain.my.h < -90 or self.brain.my.h > 90)):
	    self.timeSinceCaptureChase = self.brain.getTime()

	# keep chasing for center field hack
	if (self.brain.getTime() - self.timeSinceCaptureChase) < 2000.0:
	    return self.me

	if DEBUG_DET_CHASER:
	    print "chaser det: me == #", self.me.playerNumber
	# scroll through the teammates
        for i,mate in enumerate(self.teammates):
	    if DEBUG_DET_CHASER:
		print "\t mate #", i+1,
	    # TEAMMATE SANITY CHECKS
	    # don't check goalie out
	    # don't check out penalized or turned-off dogs
	    if (mate.playerNumber == GOALIE_NUMBER or  
		mate.inactive or 
                mate.playerNumber == self.brain.my.playerNumber or 
		fabs(mate.ballY - self.brain.ball.y) > 150.0):
# 		(0.0 < self.brain.ball.dist <= 125.0 and		 
# 		 fabs(self.brain.ball.dist - 
# 		      self.brain.getPosDist([mate.ballX, mate.ballY]) > 
# 		      125.0))): 
		if DEBUG_DET_CHASER:
		    print "\t inactive or goalie or me"
		continue

	    # if the player has got the ball, return his number
	    if (mate.grabbing or
		mate.dribbling or
		mate.kicking):
		if DEBUG_DET_CHASER:
		    print "\t grabbing"
		return mate

            # Calculate chase time locally
            mate.chaseTime = self.getChaseTime(mate)

	    # Tie break stuff
	    if self.me.chaseTime < mate.chaseTime:
		chaseTimeScale = self.me.chaseTime
	    else:
		chaseTimeScale = mate.chaseTime

	    if ((self.me.chaseTime - mate.chaseTime < 
                 CALL_OFF_THRESH + 
		 .15 *chaseTimeScale or
		(self.me.chaseTime - mate.chaseTime < 
                 STOP_CALLING_THRESH +
		 .35 * chaseTimeScale and
		 self.lastRole == CHASER)) and
		mate.playerNumber < self.me.playerNumber):
		if DEBUG_DET_CHASER:
		    print ("\t #%d @ %g >= #%d @ %g" % 
			   (mate.playerNumber, mate.chaseTime, 
			    chaser_mate.playerNumber, 
			    chaser_mate.chaseTime))

		continue
	    elif (mate.playerNumber > self.me.playerNumber and
		  mate.chaseTime - self.me.chaseTime < 
                  LISTEN_THRESH + 
		  .45 * chaseTimeScale and
		  mate.calledRole == CHASER):
		chaser_mate = mate

	    # else pick the lowest chaseTime
	    else:
		if mate.chaseTime < chaser_mate.chaseTime:
		    chaser_mate = mate

	    if DEBUG_DET_CHASER:
		print ("\t #%d @ %g >= #%d @ %g" % 
		       (mate.playerNumber, mate.chaseTime, 
			chaser_mate.playerNumber, 
			chaser_mate.chaseTime))

	if DEBUG_DET_CHASER:
	    print "\t ---- MATE %g WINS" % (chaser_mate.playerNumber)
	# returns teammate instance (could be mine)
	return chaser_mate
    
    def getChaseTime(self, mate):
	'''
        returns estimated time to get to the ball on a chase
        '''
	
	time = 0.0
	# add chase forward time
	time += ((mate.ballLocDist / CHASE_SPEED) * 
                 SEC_TO_MILLIS)
	# add bearing correction time
	time += ((fabs(mate.ballLocBearing) / CHASE_SPIN) * 
                 SEC_TO_MILLIS)

	# special KICKOFF_PLAY bonus
	if (self.currentFormation == KICKOFF_PLAY and 
	    self.brain.game.getTimeSincePlay() > 
            KICKOFF_PLAY_SWITCH_TIME and
	    self.me.playerNumber == 4):
	    time -= KICKOFF_PLAY_BONUS

	# velocity bonus
	if (VB_MIN_REL_VEL_Y < self.brain.ball.relVelY < 
	    VB_MAX_REL_VEL_Y and
	    0 < self.brain.ball.relY < VB_MAX_REL_Y and
	    fabs(self.brain.ball.relX + self.brain.ball.relVelX*
		 (self.brain.ball.relY / self.brain.ball.relVelY)) < 
	    VB_X_THRESH):
	    time -= VELOCITY_BONUS

	# ball bearing line to goal bonus
        if mate.y - self.brain.ball.y != 0:
	    xDiff = fabs( (mate.x - self.brain.ball.x) /
			  (mate.y - self.brain.ball.y) *
			  (GOAL_BACK_Y - mate.y) + mate.x -
			  GOAL_BACK_X)
	else:
	    xDiff = 0

	if ( 0. < xDiff < BEARING_SMOOTHNESS and 
             mate.y < self.brain.ball.y):
	    time -= (-BEARING_BONUS/
                      BEARING_SMOOTHNESS * 
		      xDiff + BEARING_BONUS)
	elif 0. < xDiff < BEARING_SMOOTHNESS:
	    time += (-BEARING_BONUS/
                      BEARING_SMOOTHNESS * 
		      xDiff + BEARING_BONUS)

	# frames off bonus
        if self.brain.ball.framesOff < 3:
	    time -= BALL_NOT_SUPER_OFF_BONUS
		
        return time

    def determineSupporter(self, otherMates, roleInfo):
	'''
        return a player object of the closest teammate in otherMates to 
        supportingPos
        '''
        supportRole = roleInfo[0]
        supportingPos = self.getMyChargePos()
	supporterMate = self.me
        self.me.supportTime = self.getSupportTime(self.me, supportingPos)

	# scroll through the teammates
        for i, mate in enumerate(otherMates):
            mate.supportTime = self.getSupportTime(mate, supportingPos)

	    # Tie break stuff
            supportTimeScale = min(self.me.supportTime, mate.supportTime)

	    if ((self.me.supportTime - mate.supportTime < 
                 CALL_OFF_THRESH + 
		 .15 *supportTimeScale or
		(self.me.supportTime - mate.supportTime < 
                 STOP_CALLING_THRESH +
		 .35 * supportTimeScale and
		 self.lastRole == supportRole)) and
		mate.playerNumber < self.me.playerNumber):
		if DEBUG_DET_SUPPORTER:
		    print ("\t #%d @ %g >= #%d @ %g" % 
			   (mate.playerNumber, mate.supportTime, 
			    supporterMate.playerNumber, 
			    supporterMate.supportTime))

		continue
	    elif (mate.playerNumber > self.me.playerNumber and
		  mate.supportTime - self.me.supportTime < 
                  LISTEN_THRESH + 
		  .45 * supportTimeScale and
		  mate.calledRole == CHASER):
		supporterMate = mate

	    # else pick the lowest supportTime
	    else:
		if mate.supportTime < supporterMate.supportTime:
		    supporterMate = mate

	    if DEBUG_DET_SUPPORTER:
		print ("\t #%d @ %g >= #%d @ %g" % 
		       (mate.playerNumber, mate.supportTime, 
			supporterMate.playerNumber, 
			supporterMate.supportTime))

	# returns teammate instance (could be mine)
	return supporterMate

    def getSupportTime(self, mate, supporterSpot):
	'''returns estimated time to get to the supporter spot'''
	
	time = 0.0
        
        positionDist = self.brain.getDist(mate.x, mate.y, supporterSpot[0],
                                          supporterSpot[1])
        positionBearing = fabs(self.brain.getOthersRelativeBearing(
                mate.x, mate.y, mate.h, supporterSpot[0], supporterSpot[1]))

        # Not totally correct, since we move ortho...
	# add forward time
	time += ((positionDist / SUPPORT_SPEED) * 
                 SEC_TO_MILLIS)

	# add bearing correction time
	time += ((positionBearing / SUPPORT_SPIN) * 
                 SEC_TO_MILLIS)

	return time

    def getLeastWeightPosition(self,positions, mates = None):
	"""
	Gets the position for the dog such that the distance all dogs have
	to move is the least possible
	"""
	# if there is only one position return the position
	if len(positions) == 1:
	    return positions[0]

	# if we have two positions only two possibilites of positions
	elif len(positions) == 2:
	    myDist1 = hypot(positions[0][0] - self.brain.my.x,
			    positions[0][1] - self.brain.my.y)
	    myDist2 = hypot(positions[1][0] - self.brain.my.x,
			    positions[1][1] - self.brain.my.y)
	    mateDist1 = hypot(positions[0][0] - mates.x,
			      positions[0][1] - mates.y)
	    mateDist2 = hypot(positions[1][0] - mates.x,
			      positions[1][1] - mates.y)

	    if myDist1 + mateDist2 < myDist2 + mateDist1:
		return positions[0]
	    else:
		return positions[1]

        # We have three positions, 6 possibilities
	elif len(positions) == 3:
	    # Pseudo 2d array to hold the 3 dogs dists to the 3 positions
	    dog2, dog3, dog4 = [], [], []
	    dog_distances = [dog2,dog3,dog4]
	    mates.append(self.teammates[self.me.playerNumber - 1])

	    # get distances from every dog every possible position
	    for i,dog in enumerate(dog_distances):
		dog_number = mates[i].playerNumber
		x, y = 0.0, 0.0
		# use either my estimate or teammates'
		if dog_number == self.me.playerNumber:
		    x = self.brain.my.x
		    y = self.brain.my.y
		else:
		    x = self.teammates[dog_number-1].x
		    y = self.teammates[dog_number-1].y

		for p in positions:
		    dog.append([hypot(p[0] - x, p[1] - y), p])

	    # 6 possible weights and positions
	    distances = [ (dog2[0][0] + dog3[1][0] + dog4[2][0],
			   dog2[0][1],  dog3[1][1],  dog4[2][1]),
			  (dog2[1][0] + dog3[2][0] + dog4[0][0],
			   dog2[1][1],  dog3[2][1],  dog4[0][1]),
			  (dog2[2][0] + dog3[0][0] + dog4[1][0],
			   dog2[2][1],  dog3[0][1],  dog4[1][1]),
			  (dog2[1][0] + dog3[0][0] + dog4[2][0],
			   dog2[1][1],  dog3[0][1],  dog4[2][1]),
			  (dog2[2][0] + dog3[1][0] + dog4[0][0],
			   dog2[2][1],  dog3[1][1],  dog4[0][1]),
			  (dog2[0][0] + dog3[2][0] + dog4[1][0],
			   dog2[0][1],  dog3[2][1],  dog4[1][1])]

	    # We must find the least weight choice from the 6 possibilities
	    min_dist = Constants.FIELD_HEIGHT*3
	    chosenPositions = None
	    for i,d in enumerate(distances):
		if d[0] < min_dist:
		    min_dist = d[0]
		    chosenPositions = d

	    return chosenPositions

	# This is going to work for four positions now
	else:
	    dog2, dog3, dog4, dog5 = [], [], [], []
	    dog_distances = [dog2,dog3,dog4,dog5]

	    # Determine all distances to all places
	    for i,dog in enumerate(dog_distances):
		dog_number = i+2
		x, y = 0.0, 0.0
		# use either my estimate or teammates'
		if dog_number == self.me.playerNumber:
		    x = self.brain.my.x
		    y = self.brain.my.y
		else:
		    x = self.teammates[dog_number-1].x
		    y = self.teammates[dog_number-1].y
		for p in positions:
		    dog.append([hypot(p[0] - x, p[1] - y), p])

	    # Here are the 24 possible combinations of distances
	    distances = [(dog2[0][0] + dog3[1][0] + dog4[2][0] + dog5[3][0],
			  dog2[0][1],  dog3[1][1],  dog4[2][1],  dog5[3][1]),
			 (dog2[0][0] + dog3[1][0] + dog4[3][0] + dog5[2][0],
			  dog2[0][1],  dog3[1][1],  dog4[3][1],  dog5[2][1]),
			 (dog2[0][0] + dog3[2][0] + dog4[1][0] + dog5[3][0],
			  dog2[0][1],  dog3[2][1],  dog4[1][1],  dog5[3][1]),
			 (dog2[0][0] + dog3[2][0] + dog4[3][0] + dog5[1][0],
			  dog2[0][1],  dog3[2][1],  dog4[3][1],  dog5[1][1]),
			 (dog2[0][0] + dog3[3][0] + dog4[1][0] + dog5[2][0],
			  dog2[0][1],  dog3[3][1],  dog4[1][1],  dog5[2][1]),
			 (dog2[0][0] + dog3[3][0] + dog4[2][0] + dog5[1][0],
			  dog2[0][1],  dog3[3][1],  dog4[2][1],  dog5[1][1]),

			 (dog2[1][0] + dog3[0][0] + dog4[2][0] + dog5[3][0],
			  dog2[1][1],  dog3[0][1],  dog4[2][1],  dog5[3][1]),
			 (dog2[1][0] + dog3[0][0] + dog4[3][0] + dog5[2][0],
			  dog2[1][1],  dog3[0][1],  dog4[3][1],  dog5[2][1]),
			 (dog2[1][0] + dog3[2][0] + dog4[0][0] + dog5[3][0],
			  dog2[1][1],  dog3[2][1],  dog4[0][1],  dog5[3][1]),
			 (dog2[1][0] + dog3[2][0] + dog4[3][0] + dog5[0][0],
			  dog2[1][1],  dog3[2][1],  dog4[3][1],  dog5[0][1]),
			 (dog2[1][0] + dog3[3][0] + dog4[0][0] + dog5[2][0],
			  dog2[1][1],  dog3[3][1],  dog4[0][1],  dog5[2][1]),
			 (dog2[1][0] + dog3[3][0] + dog4[2][0] + dog5[0][0],
			  dog2[1][1],  dog3[3][1],  dog4[2][1],  dog5[0][1]),

			 (dog2[2][0] + dog3[0][0] + dog4[1][0] + dog5[3][0],
			  dog2[2][1],  dog3[0][1],  dog4[1][1],  dog5[3][1]),
			 (dog2[2][0] + dog3[0][0] + dog4[3][0] + dog5[1][0],
			  dog2[2][1],  dog3[0][1],  dog4[3][1],  dog5[1][1]),
			 (dog2[2][0] + dog3[1][0] + dog4[0][0] + dog5[3][0],
			  dog2[2][1],  dog3[1][1],  dog4[0][1],  dog5[3][1]),
			 (dog2[2][0] + dog3[1][0] + dog4[3][0] + dog5[0][0],
			  dog2[2][1],  dog3[1][1],  dog4[3][1],  dog5[0][1]),
			 (dog2[2][0] + dog3[3][0] + dog4[0][0] + dog5[1][0],
			  dog2[2][1],  dog3[3][1],  dog4[0][1],  dog5[1][1]),
			 (dog2[2][0] + dog3[3][0] + dog4[1][0] + dog5[0][0],
			  dog2[2][1],  dog3[3][1],  dog4[1][1],  dog5[0][1]),

			 (dog2[3][0] + dog3[0][0] + dog4[1][0] + dog5[2][0],
			  dog2[3][1],  dog3[0][1],  dog4[1][1],  dog5[2][1]),
			 (dog2[3][0] + dog3[0][0] + dog4[2][0] + dog5[1][0],
			  dog2[3][1],  dog3[0][1],  dog4[2][1],  dog5[1][1]),
			 (dog2[3][0] + dog3[1][0] + dog4[0][0] + dog5[2][0],
			  dog2[3][1],  dog3[1][1],  dog4[0][1],  dog5[2][1]),
			 (dog2[3][0] + dog3[1][0] + dog4[2][0] + dog5[0][0],
			  dog2[3][1],  dog3[1][1],  dog4[2][1],  dog5[0][1]),
			 (dog2[3][0] + dog3[2][0] + dog4[0][0] + dog5[1][0],
			  dog2[3][1],  dog3[2][1],  dog4[0][1],  dog5[1][1]),
			 (dog2[3][0] + dog3[2][0] + dog4[1][0] + dog5[0][0],
			  dog2[3][1],  dog3[2][1],  dog4[1][1],  dog5[0][1])]

	    # We must find the least weight choice from the 24 possibilities
	    min_dist = Constants.FIELD_HEIGHT*3
	    chosenPositions = None
	    for i,d in enumerate(distances):
		if d[0] < min_dist:
		    min_dist = d[0]
		    chosenPositions = d

	    return chosenPositions

    ######################################################
    ############       Teammate Stuff     ################
    ######################################################
        
    def aPrioriTeammateUpdate(self):
	'''
        Here we update information about teammates before running a new frame
	'''
	# Change which wing is forward based on the opponents score
	self.kickoffFormation = (self.brain.game.theirTeam.teamScore) % 2

	# update my own information for role switching
	self.me.updateMe()
	self.me.chaseTime = self.getChaseTime(self.me)

	# loop through teammates    
	for mate in self.teammates:
	    if (self.isTeammateDead(mate) or 
		self.isTeammatePenalized(mate)):
		mate.inactive = True

        self.inactiveMates = self.getInactiveFieldPlayers()
        self.numInactiveMates = len(self.inactiveMates)
        self.useAttractor = False

    def updateTeammateCharges(self):
	'''updates teammate field charges. bigger repulsor for chaser.'''

	for i,mate in enumerate(self.teammates):
	    # don't update self
	    if self.me.playerNumber == mate.playerNumber:
		continue

	    ## update potential field repulsor at teammate (x,y) ##
	    # if teammate is inactive but charge is still there
	    # (ie he just became inactive), remove repulsor
	    if (mate.inactive or 
		self.currentFormation == KICKOFF or
		(self.currentRole == DEFENDER and 
		 self.brain.my.y < Constants.FIELD_HEIGHT * 1./4.)):
		if mate.charge:
		    #print "removing mate %d repulsor" % (i)
		    self.removeTeammateRepulsor(mate)	    
	    elif mate.role == CHASER and mate.y < self.brain.my.y:
		#if ((mate.grabbing or mate.dribbling or mate.kicking) and
		#    ):
		#self.updateTeammateCharge(mate,self.CHASER_TO_GOAL_CHARGE)
		#else:
		self.updateTeammateCharge(mate,
                                          CHASER_CHARGE)	    
            elif self.brain.game.state == Constants.GAME_READY:
                self.updateTeammateCharge(mate, READY_CHARGE)
	    else:
		self.updateTeammateCharge(mate, NORMAL_CHARGE)

    def  aPosterioriTeammateUpdate(self):
        """
        Here are updates to teammates which occur after running before 
        exiting the frame
        """
	# update teammate pfield charges based on strategy
	self.updateTeammateCharges()

        # Set our position via potentialFields if required
        if self.useAttractor:
            self.position = self.getAttractorPosition()

    def isTeammatePenalized(self,teammate):
	'''
	this checks GameController to see if a player is penalized.
	this check is more redundant than anything, because our players stop
	sending packets when they are penalized, so they will most likely
	fall under the isTeammateDead() check anyways.
	'''
	return (self.brain.game.myTeam.players[teammate.playerNumber-1].penalty)

    def isTeammateDead(self,teammate):
	'''
	returns True if teammates' last timestamp is sufficiently behind ours.
	however, the dog could still be on but sending really laggy packets.
	'''
	return (teammate.playerNumber != self.brain.my.playerNumber and
                teammate.lastPacketTime == 0 or 
                teammate.lastPacketTime < (self.brain.getTime() -
					   PACKET_DEAD_PERIOD))

    def getInactiveFieldPlayers(self):
	'''cycles through teammate objects and returns number of teammates
	that are 'dead'. ignores myself'''
	inactive_teammates = []
	for i,mate in enumerate(self.teammates):
	    if (mate.inactive and mate.playerNumber != 
                self.brain.my.playerNumber and 
		mate.playerNumber != GOALIE_NUMBER):
		inactive_teammates.append(mate)
	return inactive_teammates

    def highestActivePlayerNumber(self):
	'''returns true if the player is the highest active player number'''
	activeMates = self.getOtherActiveTeammates()
	
	for mate in activeMates:
	    if mate.playerNumber > self.me.playerNumber:
		return False

	return True


    def getNumPenalizedOpponents(self):
	'''returns number of penalized robots on opponents team'''
	return self.brain.game.theirTeam.numPenalized

    def teammateHasBall(self):
	'''returns True if any mate has the ball'''
	for i,mate in enumerate(self.teammates):
	    if (mate.inactive or 
		mate.playerNumber == self.me.playerNumber):
		continue
	    elif (mate.grabbing or 
		  mate.dribbling or 
		  mate.kicking):
		return True
	return False

    def isGoalieInactive(self):
	'''
	Determines if the goalie is dead or penalized
	'''
	return (self.isTeammateDead(self.teammates[0]) or
		self.isTeammatePenalized(self.teammates[0]) or 
		self.teammates[0].inactive)

    def isGoalie(self):
        return self.me.playernumber == 1
    ######################################################
    ############   Strategy Decision Stuff     ###########
    ######################################################
    def shouldUseDubD(self):
	return ((self.brain.ball.x > Constants.MY_GOALBOX_LEFT_X + 5. and
		 self.brain.ball.x < Constants.MY_GOALBOX_RIGHT_X -5.  and
		 self.brain.ball.y < Constants.MY_GOALBOX_TOP_Y - 5.) or
		(self.brain.ball.x > Constants.MY_GOALBOX_LEFT_X -5. and
		 self.brain.ball.x < Constants.MY_GOALBOX_RIGHT_X + 5. and
		 self.brain.ball.y < Constants.MY_GOALBOX_TOP_Y + 5. and
		 self.teammates[0].calledRole == CHASER))

    def ballInMyGoalBox(self):
	'''
	returns True if estimate of ball (x,y) lies in my goal box
	-includes all y values below top of goalbox 
	(so inside the goal is included)
	'''
	return (self.brain.ball.x > Constants.MY_GOALBOX_LEFT_X and
		self.brain.ball.x < Constants.MY_GOALBOX_RIGHT_X and
		self.brain.ball.y < Constants.MY_GOALBOX_TOP_Y)
    
    def ballNearSideline(self):
	'''
	returns True if ball (x,y) is near any sideline. 
	avoids either goal box
	'''
	return (
	    # if ball is nearing left sideline
	    (self.brain.ball.x < Constants.FIELD_WHITE_LEFT_SIDELINE_X + 
	     NEAR_LINE_THRESH) or 
	    # if ball is nearing right sideline
	    (self.brain.ball.x > Constants.FIELD_WHITE_RIGHT_SIDELINE_X -
	     NEAR_LINE_THRESH) or
	    # if ball is left of goalbox AND
	    (self.brain.ball.x < Constants.GOALBOX_LEFT_X and 
	     # ball is nearing top sideline OR
	     ((self.brain.ball.y > Constants.FIELD_WHITE_TOP_SIDELINE_Y - 
	       NEAR_LINE_THRESH) or 
	      # ball is nearing bottom sideline
	      (self.brain.ball.y < Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
	       NEAR_LINE_THRESH))) or
	    # if ball is right of goalbox AND
	    (self.brain.ball.x > Constants.GOALBOX_RIGHT_X and 
	     # ball is nearing top sideline OR
	     ((self.brain.ball.y > Constants.FIELD_WHITE_TOP_SIDELINE_Y - 
	       NEAR_LINE_THRESH) or 
	      # ball is nearing bottom sideline
	      (self.brain.ball.y < Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
	       NEAR_LINE_THRESH)))
	    )

    def getPointBetweenBallAndGoal(self,dist_from_ball):
	'''returns defensive position between ball (x,y) and goal (x,y)
	at <dist_from_ball> centimeters away from ball'''
	delta_x = self.brain.ball.x - Constants.LANDMARK_MY_GOAL_X
	delta_y = self.brain.ball.y - Constants.LANDMARK_MY_GOAL_Y
	
	pos_x = self.brain.ball.x - (dist_from_ball/
				     hypot(delta_x,delta_y))*delta_x
	pos_y = self.brain.ball.y - (dist_from_ball/
				     hypot(delta_x,delta_y))*delta_y
	if pos_y > DEFENSIVE_MIDFIELD_Y:
	    pos_y = DEFENSIVE_MIDFIELD_Y
	    pos_x = (Constants.LANDMARK_MY_GOAL_X + delta_x / delta_y *
		     (DEFENSIVE_MIDFIELD_Y - 
                      Constants.LANDMARK_MY_GOAL_Y))

	return pos_x,pos_y

    def getNonChaserTeammates(self,chaser_mate):
	'''returns non-chaser teammate with you (assuming three dogs)'''
	nonChasers = []

	for elem in self.teammates:
	    if ( elem.playerNumber != GOALIE_NUMBER and 
		 elem != chaser_mate and 
		 elem.playerNumber != self.me.playerNumber):
		nonChasers.append(elem)

	return nonChasers

    def getNonChaserPlayers(self, chaser_mate):
	'''
	figure out who the other active teammates are
	'''
	mates = []
	for mate in self.teammates:
	    if (not mate.inactive and mate.playerNumber != 
                GOALIE_NUMBER 
                and mate != chaser_mate):
		mates.append(mate)
	return mates

    def getOtherActiveTeammate(self):
	'''this returns the teammate instance of an active teammate that isn't 
	you. THIS ASSUMES THAT THERE IS ALREADY ONE FIELD PLAYER DEAD'''
	# Figure out who isn't penalized with you
	others = [mate for mate in self.teammates if
                  mate.playerNumber != GOALIE_NUMBER  and
                  mate.playerNumber != self.me.playerNumber and
                  not mate.inactive]
        if others:
            return others[0]
        else:
            return None

    def getOtherActiveTeammates(self):
	'''
	figure out who the other active teammates are
	'''
	mates = []
	for mate in self.teammates:
	    if (not mate.inactive and mate.playerNumber != 
                GOALIE_NUMBER
		and mate.playerNumber != self.me.playerNumber):
		mates.append(mate)
	return mates

    def noCalledChaser(self):
	"""
	Returns true if no one is chasing and they are not searching
	"""
	# If everyone else is out, let's not go for the ball
	if len(self.getInactiveFieldPlayers()) == \
                NUM_TEAM_PLAYERS - 1.:
	    return False

	for mate in self.teammates:
	    if (not mate.inactive and (mate.calledRole == CHASER
				       or mate.calledRole == 
                                       SEARCHER)):
		return False
	return True	

    def getGoalDifferential(self):
	'''
	Returns the current goal differential
    	'''
	return (self.brain.game.myTeam.teamScore - 
		self.brain.game.theirTeam.teamScore)

    ######################################################
    ############   Potential Fields Stuff     ############
    ######################################################

    def getAttractorPosition(self):
	'''returns position to move to (via pf) + x_eq, y_eq'''
	x_move, y_move, x_eq, y_eq = \
	    self.pf.movementVectorAt(self.brain.my.x,self.brain.my.y)	
	return (self.brain.my.x+x_move,self.brain.my.y+y_move,x_eq,y_eq)

    def updateMyCharge(self,position):
	'''updates my charge (adds if we don't have any, moves otherwise)'''
	if self.my_charge:
	    self.moveMyCharge(position)
        else:
	    self.addMyCharge(position)

    def addMyCharge(self,pos):
	'''adds an attractor point at (x,y) for me to move to'''
	self.my_charge = self.pf.addPointCharge(Constants.VERY_STRONG * 
						Constants.ATTRACTION,
						Constants.HALF_FIELD,
						pos[0],pos[1])
	self.my_charge_pos = pos

    def moveMyCharge(self,pos):
	'''moves my attractor point to (x,y) position'''
        self.useAttractor = True
	if not self.my_charge:
	    self.addMyCharge(pos)
	elif self.my_charge == pos:
	    return
	else:
	    self.pf.movePointCharge(self.my_charge,pos[0],pos[1])
	    self.my_charge_pos = pos  

    def removeMyCharge(self):
	'''removes own attractor point'''
	if self.my_charge:
	    self.pf.removeCharge(self.my_charge)
	    self.my_charge = None
	    self.my_charge_pos = None

    def updateTeammateCharge(self,mate,charge_type):
	'''updates teammate's charge with proper charge type handling'''
	# mate has no charge, add charge
	if not mate.charge:
	    self.addTeammateRepulsor(mate,charge_type)
	# mate has charge but not the desired type, remove and add
	elif mate.charge_type != charge_type:
	    self.removeTeammateRepulsor(mate)
	    self.addTeammateRepulsor(mate,charge_type)
	# mate has a charge, is right type, so just move charge
	else:
	    self.moveTeammateRepulsor(mate)
	    
    def addTeammateRepulsor(self,mate,type):
	'''adds teammate (x,y) as repulsor point according to type of charge'''
	if type == NORMAL_CHARGE or type == READY_CHARGE:	    
	    mate.charge = self.pf.addPointCharge(Constants.VERY_WEAK * 
						 Constants.REPULSION, 
						 Constants.VERY_SMALL_REGION, 
						 mate.x, 
						 mate.y)
	    mate.charge_type = type
        elif type == CHASER_CHARGE:
	    mate.charge = self.pf.addPointCharge(Constants.STRONG * 
						 Constants.REPULSION, 
						 Constants.SMALL_REGION, 
						 mate.x, 
						 mate.y)
	    mate.charge_type = type
	elif type == CHASER_TO_GOAL_CHARGE:
	    mate.charge = \
		self.pf.addSegmentCharge(Constants.VERY_STRONG * 
					 Constants.REPULSION,
					 Constants.LINE_SPREAD,
					 mate.x, mate.y,
					 Constants.LANDMARK_OPP_GOAL_X,
					 Constants.LANDMARK_OPP_GOAL_Y)
	    mate.charge_type = type
        elif type == READY_CHARGE and self.closeToReadySpot(mate):
            mate.charge = self.pf.addPointCharge(Constants.VERY_WEAK *
                                                 Constants.REPULSION,
                                                 Constants.UTTERLY_SMALL_REGION,
                                                 mate.x,
                                                 mate.y)
            mate.charge_type = type
	else:
	    mate.charge = None
	    mate.charge_type = None
	    print "addTeammateRepuslor: unknown charge type"
			

    def moveTeammateRepulsor(self,mate):
	'''moves teammate's repulsor to his new (x,y)'''
	if (mate.charge_type == NORMAL_CHARGE or
	    mate.charge_type == CHASER_CHARGE or
            mate.charge_type == READY_CHARGE or
	    mate.charge_type == GOALIE_CHARGE):
	    self.pf.movePointCharge(mate.charge,
				    mate.x, 
				    mate.y)
	elif mate.charge_type == CHASER_TO_GOAL_CHARGE:
	    self.pf.moveSegmentCharge(mate.charge,
				      mate.x, mate.y,
				      Constants.LANDMARK_OPP_GOAL_X,
				      Constants.LANDMARK_OPP_GOAL_Y)
    
    def removeTeammateRepulsor(self,mate):
	'''removes teammate repulsor (if he gets penalized/dies)'''
	if mate.charge:
	    self.pf.removeCharge(mate.charge)
	    mate.charge = None
	    mate.charge_type = None

    def addBallRepulsor(self):
        """
        Adds a charge to avoid the ball 
        """
        if not self.brain.ball.charge:
            self.brain.ball.charge = self.pf.addPointCharge(Constants.WEAK*
                                   Constants.REPULSION,
                                   Constants.VERY_SMALL_REGION,
                                   self.brain.ball.x,
                                   self.brain.ball.y)
                                           
    def moveBallRepulsor(self):
        """
        Move the charge of the ball to a new location
        """
        if self.brain.ball.charge:
            self.pf.movePointCharge(self.brain.ball.charge,
                                    self.brain.ball.x,
                                    self.brain.ball.y)
        else:
            self.addBallRepulsor()

    def removeBallRepulsor(self):
        """
        Removes the repulsor for the ball
        """
        if self.brain.ball.charge:
            self.pf.removeCharge(self.brain.ball.charge)
            self.brain.ball.charge = None

    def closeToReadySpot(self, mate):
        """
        Determine if a mate is close to the correct ready spot
        """
        return False

    def getMyChargePos(self):
	'''returns attraction position of the pf'''
	if self.my_charge_pos:
	    return self.my_charge_pos
        else:
	    return [self.brain.my.x, self.brain.my.y]

    def reset(self):
	'''resets all information stored from teammates'''
        #print "playbook:: reset!"
	self.removeMyCharge()
	for i,mate in enumerate(self.teammates):
	    mate.reset()

    def update(self,packet):
	'''public method called by Brain.py to update a teammates' info
	with a new packet'''
	self.teammates[packet.playerNumber-1].update(packet)


