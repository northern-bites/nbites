##
## MODULE IMPORTS ##
##
# our own
from TypeDefs import *
try:
    import potentialfield
except ImportError:
    import PotentialField as potentialfield
import Constants

# python
from math import hypot, fabs

# See PlayBook under Behaviors on the Wiki for extended documentation

# TODO: Fix getLeastWeight() for 4 and fNormal() tieBreaking
# Really build up pMiddie()

# PlayBook extended to operate for a team of 5 robots
# The implementation as it is now is quite a rudimentary switch
# The fifth robot is added via creating a 'MIDDIE' role
# The middie has not been fully integrated into the formations, instead it is
# simply added to a place on the field where it can be marginally usefull,
# Ideally the first revision would be in taking any time the defender or 
# offender is at midfield, substitute that player with the middie 
# and have the defender play back of the offender play up, 
# depending on the situation

OFFLINE = False

# Test switches to force one role to always be given out
TEST_DEFENDER = False
TEST_OFFENDER = False
TEST_CHASER = False
TEST_MIDDIE = False

# Print information as to how the chaser is determined
DEBUG_DET_CHASER = False

DEFENDER_CHASER = False # Has a single robot act as a hybrid defender-chaser
DEEP_STOPPER = False # Fallback to a deep defensive position
KICKOFF_PLAY = False # Forces the more intelligent and restrictive kickoff play

class PlayBook:

############## CONSTANTS ##################

    # Potential strategies?
    # Power Play Punch Down - rush forward to take advantage on power play
    # Man Out Keep Stout - get rid of striker when we have a player out
    # Bunker Down - keep two robots back
    # 5 robots surround goalbox, divide by 0

    #### STRATEGY CONSTANTS ####

    NUM_STRATEGIES = 1
    STRATEGIES = dict(zip(range(NUM_STRATEGIES), ("WIN")))
    (WIN) = range(NUM_STRATEGIES)
    
    #### FORMATION CONSTANTS ####
    
    # number of formations
    NUM_FORMATIONS = 6
    # dictionary of formations
    FORMATIONS = dict(zip(range(NUM_FORMATIONS), ("NORMAL",
						  "DUB_D",
						  "FINDER",
						  "KICKOFF",
						  "KICKOFF_PLAY",
						  "READY")))
    # tuple of formations
    (NORMAL,
     DUB_D,
     FINDER,
     KICKOFF,
     KICKOFF_PLAY,
     READY) = range(NUM_FORMATIONS)
    
    # random circumstance constants
    # ball uncert thresh where we switch to FINDER
    FINDER_TIME_THRESH = 5000
    NEAR_LINE_THRESH = 15 # near out of bounds threshold
    
    KICKOFF_FORMATION_TIME = 5000

    #### ROLE CONSTANTS ####

    # number of roles
    NUM_ROLES = 6
    # dictionary of roles
    ROLES = dict(zip(range(NUM_ROLES), ("CHASER",
					"OFFENDER",
					"MIDDIE",
					"DEFENDER",
					"SEARCHER",
					"GOALIE")))
    # tuple of roles
    (CHASER,
     OFFENDER,
     MIDDIE,
     DEFENDER,
     SEARCHER,
     GOALIE) = range(NUM_ROLES)
  
    #### SUB_ROLE CONSTANTS ####

    # dictionary of subRoles
    NUM_SUB_ROLES = 31
    SUB_ROLES = dict(zip(range(NUM_SUB_ROLES), (#OFFENDER SUB ROLES
		                                "LEFT_WING",
						"RIGHT_WING",
						"LEFT_STRIKER",
						"RIGHT_STRIKER",
						"FORWARD_MIDFIELD",
						"ATTACKING_MIDFIELD",
						"CENTER_O_MIDFIELD",
						"RIGHT_SIDELINE_O",
						"LEFT_SIDELINE_O",
						
						# MIDDIE SUB ROLES
						"LEFT_O_MIDDIE",
						"RIGHT_O_MIDDIE",
						"CENTER_O_MIDDIE",
						"LEFT_D_MIDDIE",
						"RIGHT_D_MIDDIE",
						"CENTER_D_MIDDIE",

						# DEFENDER SUB ROLES
						"STOPPER",
						"DEEP_STOPPER",
						"SWEEPER",
						"DEFENSIVE_MIDFIELD",
						"LEFT_DEEP_BACK",
						"RIGHT_DEEP_BACK",
						
						# CHASER SUB ROLES
						"CHASE_NORMAL",
						"CHASE_AROUND_BOX"
						
						# FINDER SUB ROLES
						"FRONT_FINDER",
						"LEFT_FINDER",
						"RIGHT_FINDER",
						"OTHER_FINDER",

						# GOALIE SUB ROLE
						"GOALIE_SUB_ROLE",

						# KICKOFF SUB ROLES
						"KICKOFF_SWEEPER",
						"KICKOFF_STRIKER",
						"KICKOFF_MIDDIE")))
    # tuple of subRoles
    (LEFT_WING,
     RIGHT_WING,
     LEFT_STRIKER,
     RIGHT_STRIKER,
     FORWARD_MIDFIELD,
     ATTACKING_MIDFIELD,
     CENTER_O_MIDFIELD,
     RIGHT_SIDELINE_O,
     LEFT_SIDELINE_O,
     LEFT_O_MIDDIE,
     RIGHT_O_MIDDIE,
     CENTER_O_MIDDIE,
     LEFT_D_MIDDIE,
     RIGHT_D_MIDDIE,
     CENTER_D_MIDDIE,
     STOPPER,
     DEEP_STOPPER,
     SWEEPER,
     DEFENSIVE_MIDFIELD,
     LEFT_DEEP_BACK,
     RIGHT_DEEP_BACK,
     CHASE_NORMAL,
     CHASE_AROUND_BOX,
     FRONT_FINDER,
     LEFT_FINDER,
     RIGHT_FINDER,
     OTHER_FINDER,
     GOALIE_SUB_ROLE,
     KICKOFF_SWEEPER,
     KICKOFF_STRIKER,
     KICKOFF_MIDDIE) = range(NUM_SUB_ROLES)

    # penalty is: (ball_dist*heading)/scale
    PLAYER_HEADING_PENALTY_SCALE = 300.0 # max 60% of distance
    # penalty is: (ball_dist*ball_bearing)/scale
    BALL_BEARING_PENALTY_SCALE = 200.0 # max 90% of distance
    NO_VISUAL_BALL_PENALTY = 2 # centimeter penalty for not seeing the ball
    TEAMMATE_CHASING_PENALTY = 3 # adds standard penalty for teammates (buffer)
    TEAMMATE_CHASER_USE_NUMBERS_BUFFER = 20.0 # cm
    TEAMMATE_POSITIONING_USE_NUMBERS_BUFFER = 25.0 # cm
    
    ## POSITION CONSTANTS ##
    
    # READY_KICKOFF: two back, one forward
    READY_KICKOFF_DEFENDER_0 = [Constants.CENTER_FIELD_X * 1./2.,
			      Constants.FIELD_HEIGHT * 1./5.,
			      Constants.OPP_GOAL_HEADING] # left

    READY_KICKOFF_DEFENDER_1 = [Constants.CENTER_FIELD_X * 3./2.,
			      Constants.FIELD_HEIGHT * 1./5.,
			      Constants.OPP_GOAL_HEADING] # right


    READY_KICKOFF_CHASER = [Constants.CENTER_FIELD_X - 20,
			    Constants.CENTER_FIELD_Y - 40.,
			    -30.0] # center

    READY_KICKOFF_NORMAL_CHASER = [Constants.CENTER_FIELD_X,
				   Constants.CENTER_FIELD_Y - 50.0,
				   Constants.OPP_GOAL_HEADING]

    READY_KICKOFF_OFFENDER_0 = [Constants.CENTER_FIELD_X * 3./2.,
			      Constants.CENTER_FIELD_Y - 30.0,
			      Constants.OPP_GOAL_HEADING] # right

    READY_KICKOFF_OFFENDER_1 = [Constants.CENTER_FIELD_X * 1./2.,
			      Constants.CENTER_FIELD_Y - 30.0,
			      Constants.OPP_GOAL_HEADING] # left

    # Starting style just be a wing on the other side of the field
    READY_KICKOFF_MIDDIE_0 = READY_KICKOFF_OFFENDER_1
    READY_KICKOFF_MIDDIE_1 = READY_KICKOFF_OFFENDER_0

    
    READY_KICKOFF_STOPPER = [Constants.FIELD_WIDTH * 2./5.,
			     Constants.FIELD_HEIGHT * 1./4.,
			     Constants.OPP_GOAL_HEADING]  


    
    # READY_NON_KICKOFF
    # non kickoff positions: three in a row, one back in corner
    # this was a really quick make, should be rethought out
    # Stricker should maintain straight line with the ball
    NON_KICKOFF_Y = 130.0 # y to aim for
    READY_NON_KICKOFF_DEFENDER = [Constants.FIELD_WIDTH * 1./4.,
				  NON_KICKOFF_Y - 50.,
				  Constants.OPP_GOAL_HEADING] # left back
    READY_NON_KICKOFF_MIDDIE = [Constants.FIELD_WIDTH * 1./3.,
				NON_KICKOFF_Y,
				Constants.OPP_GOAL_HEADING] # left
    READY_NON_KICKOFF_CHASER = [Constants.CENTER_FIELD_X,
				NON_KICKOFF_Y,
				Constants.OPP_GOAL_HEADING] # center 
    READY_NON_KICKOFF_OFFENDER = [Constants.FIELD_WIDTH * 2./3.,
				  NON_KICKOFF_Y,
				  Constants.OPP_GOAL_HEADING] # right

    READY_NON_KICKOFF_LEFT_POSITION = [Constants.FIELD_WIDTH * 2./5.,
				       NON_KICKOFF_Y,
				       Constants.OPP_GOAL_HEADING]
    
    READY_NON_KICKOFF_RIGHT_POSITION = [Constants.FIELD_WIDTH * 3./5.,
					NON_KICKOFF_Y,
					Constants.OPP_GOAL_HEADING]

    # KICK OFF POSITIONS (right after kickoff, rather)
    KICKOFF_OFFENDER_0 = [Constants.FIELD_WIDTH * 3./4.,
			  Constants.FIELD_HEIGHT * 2./3.]
    KICKOFF_OFFENDER_1 = [Constants.FIELD_WIDTH * 1./4.,
			  Constants.FIELD_HEIGHT * 2./3.]

    KICKOFF_MIDDIE_0 = KICKOFF_OFFENDER_1
    KICKOFF_MIDDIE_1 = KICKOFF_OFFENDER_0

    KICKOFF_DEFENDER_0 = [Constants.FIELD_WIDTH * 1./2.,
			  Constants.FIELD_HEIGHT * 1./4.]
    KICKOFF_DEFENDER_1 = [Constants.FIELD_WIDTH * 1./2.,
			  Constants.FIELD_HEIGHT * 1./4.]

    # KICK OFF POSITIONS (right after kickoff, rather)
    KICKOFF_PLAY_OFFENDER = [Constants.FIELD_WIDTH * 4./5.,
			Constants.FIELD_HEIGHT * 2./3.]
    KICKOFF_PLAY_DEFENDER = [Constants.FIELD_WIDTH * 1./2.,
			Constants.FIELD_HEIGHT * 1./4.]
    KICKOFF_PLAY_MIDDIE = [Constants.FIELD_WIDTH * 1./5.,
			   Constants.FIELD_HEIGHT / 2.]
    
    # Defender
    DEFENDER_BALL_DIST = 100
    SWEEPER_X = Constants.CENTER_FIELD_X
    SWEEPER_Y = Constants.MY_GOALBOX_TOP_Y + 50.
    DEEP_BACK_Y = SWEEPER_Y
    LEFT_DEEP_BACK_X = Constants.MY_GOALBOX_LEFT_X - 40.
    RIGHT_DEEP_BACK_X = Constants.MY_GOALBOX_RIGHT_X + 40.
    
    # Midfield
    # OLD-MIDFIELD for DEFENDER and OFFENDER, not setup for MIDDIE!!!
    DEFENSIVE_CENTER_MIDFIELD_Y = Constants.CENTER_FIELD_Y - 60.
    DEEP_STOPPER_Y = Constants.CENTER_FIELD_Y * 1./2.
    OFFENSIVE_CENTER_MIDFIELD_Y = Constants.CENTER_FIELD_Y + 60.
    FORWARD_MIDFIELD_Y = Constants.CENTER_FIELD_Y + 125.
    FORWARD_MID_LEFT_LIMIT = Constants.CENTER_FIELD_X - 60.
    FORWARD_MID_RIGHT_LIMIT = Constants.CENTER_FIELD_X + 60.
    DEFENSIVE_MIDFIELD_Y = Constants.CENTER_FIELD_Y - 100.

    # Sub Role Boundries for offender
    OPP_CORNERS_Y = Constants.OPP_GOALBOX_BOTTOM_Y
    RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X = 325.
    LEFT_SIDELINE_OFFENDER_BALL_THRESH_X = 75.
    FORWARD_MIDFIELD_BALL_THRESH_Y = 375.
    CENTER_MID_DEFENDER_LIMIT = Constants.CENTER_FIELD_Y + 70.
    CENTER_MID_OFFENDER_LIMIT = Constants.CENTER_FIELD_Y - 40.
    CENTER_MID_LEFT_LIMIT = Constants.CENTER_FIELD_X - 100.
    CENTER_MID_RIGHT_LIMIT = Constants.CENTER_FIELD_X + 100.
    OFFENDER_SUB_ROLE_BUFFER_SIZE = 25.
    SIDELINE_BUFFER_SIZE = 35.

    # Offender
    WING_X_OFFSET = 50.
    WING_Y_OFFSET = 75.
    WING_MIN_Y = 325.
    WING_MAX_Y = 450.
    LEFT_WING_MIN_X = 75.
    LEFT_WING_MAX_X = 175.
    RIGHT_WING_MIN_X = 225.
    RIGHT_WING_MAX_X = 325.
    STRIKER_LEFT_X = Constants.CENTER_FIELD_X - 50.
    STRIKER_RIGHT_X = Constants.CENTER_FIELD_X + 50.
    STRIKER_Y = Constants.OPP_GOALBOX_BOTTOM_Y - 30.
    SIDELINE_O_Y_OFFSET = 50.
    SIDELINE_O_MIN_Y = 325
    SIDELINE_O_MAX_Y = 400.
    RIGHT_SIDELINE_O_X = Constants.CENTER_FIELD_X + 50.
    LEFT_SIDELINE_O_X = Constants.CENTER_FIELD_X - 50.

    # Finder
    FOUR_DOG_FINDER_POSITIONS = (
	(Constants.FIELD_WHITE_LEFT_SIDELINE_X + 75.0, 
	 Constants.FIELD_HEIGHT * 1/2),
	(Constants.FIELD_WHITE_RIGHT_SIDELINE_X - 75.0, 
	 Constants.FIELD_HEIGHT * 1/2),
	(Constants.MIDFIELD_X, 
	 Constants.FIELD_HEIGHT * 1./4.),
	(Constants.MIDFIELD_X,
	 Constants.FIELD_HEIGHT * 3./4.))
    THREE_DOG_FINDER_POSITIONS = (
	(Constants.FIELD_WHITE_LEFT_SIDELINE_X + 75.0, 
	 Constants.FIELD_HEIGHT * 1/2),
	(Constants.FIELD_WHITE_RIGHT_SIDELINE_X - 75.0, 
	 Constants.FIELD_HEIGHT * 1/2),
	(Constants.MIDFIELD_X, 
	 Constants.FIELD_HEIGHT * 1./4.))
    TWO_DOG_FINDER_POSITIONS = (
	(Constants.CENTER_FIELD_X, 
	 Constants.CENTER_FIELD_Y / 2.0),
	(Constants.FIELD_WIDTH * 1./3.,
	 Constants.FIELD_HEIGHT * 1./2.))

    # Dub_d
    CENTER_MIDFIELD_OFFENDER_POS = (Constants.CENTER_FIELD_X, 
				    OFFENSIVE_CENTER_MIDFIELD_Y)
    LEFT_DEEP_BACK_POS =  (LEFT_DEEP_BACK_X, DEEP_BACK_Y)
    RIGHT_DEEP_BACK_POS = (RIGHT_DEEP_BACK_X, DEEP_BACK_Y)

    # Sub roles
    SUB_ROLE_SWITCH_BUFFER = 10.

        ## OTHER CONSTANTS ##
    NUM_TEAM_PLAYERS = 5
    PACKET_DEAD_PERIOD = 5000
    GOALIE_NUMBER = 1

    # role switching constants
    CALL_OFF_THRESH = 125.
    LISTEN_THRESH = 250.
    STOP_CALLING_THRESH = 275.
    BEARING_BONUS = 300.
    BALL_NOT_SUPER_OFF_BONUS = 300.
    BALL_CAPTURE_BONUS = 1000.
    BEARING_SMOOTHNESS = 500.

    # Velocity bonus constants
    VB_MIN_REL_VEL_Y = -100.
    VB_MAX_REL_VEL_Y = -20.
    VB_MAX_REL_Y = 200.
    VB_X_THRESH = 30.
    VELOCITY_BONUS = 250.
    
    GOAL_BACK_X = Constants.CENTER_FIELD_X
    GOAL_BACK_Y = Constants.FIELD_GREEN_HEIGHT

    NUM_TEAMMATE_CHARGE_TYPES = 4
    TEAMMATE_CHARGE_TYPES = dict(zip(range(NUM_TEAMMATE_CHARGE_TYPES), 
				     ("NORMAL_CHARGE",
				      "READY_CHARGE",
				      "CHASER_CHARGE",
				      "GOALIE_CHARGE",
				      "CHASER_TO_GOAL_CHARGE")))
    # tuple of charge types
    (NORMAL_CHARGE,
     CHASER_CHARGE,
     GOALIE_CHARGE,
     CHASER_TO_GOAL_CHARGE) = range(NUM_TEAMMATE_CHARGE_TYPES)

    '''The PlayBook'''
    def __init__(self, the_brain=None):
	'''initializes varirables in the class'''

	if OFFLINE:
	    self.playerNumber = 0
	    return

	############# VARIABLES ##################
	
	# brain object
	self.brain = the_brain
	self.ball = self.brain.ball
	self.my = self.brain.my

	self.strategy = None
	self.formation = None
	self.role = None
	self.position = None
	self.subRole = None

	# Memory objects
	self.lastRole = None
	self.lastSubRole = None
	self.lastDiffRole = None	
	self.lastDiffSubRole = None
	self.roleCounter = 0
	self.subRoleCounter = 0
	self.subRoleOnDeck = None
	self.lastSubRoleOnDeck = None
	self.subRoleOnDeckCount = 0

	self.playerNumber = self.brain.my.playerNumber
	#print "Initializing my player number to", self.playerNumber
	self.teammates = []
        for i in xrange(self.NUM_TEAM_PLAYERS):
	    mate = Teammate(the_brain)
            mate.playerNumber = i + 1
            self.teammates.append(mate)

	# stores 'my mate' object
	#self.me_mate = self.teammates[self.playerNumber-1] 
	
	# potential field setup
	self.pf = potentialfield.PotentialField()
	self.pf.addFieldCharges()
	self.my_charge = None
	self.my_charge_type = None
	self.my_charge_pos = None

	self.kickoff_formation = 0
	self.timeSinceCaptureChase = 0
	    
    def get_action(self):
	'''called by player behaviors. returns a list [x,y,h] for position and
	a role to play'''

	# do routine teammate updating 
	self.updateTeammateStuff()
	# update my own information for role switching
	self.teammates[self.playerNumber-1].updateMe()
	self.teammates[self.playerNumber-1].chaseTime = \
	    self.getChaseTime(self.teammates[self.playerNumber-1])
	# Change which wing is forward based on the opponents score
	self.kickoff_formation = (self.brain.game.theirTeam.teamScore) % 2

	# pick strategy, go from there
	self.strategize()
	
	# update teammate pfield charges based on strategy
	self.updateTeammateCharges()

	# Update memory of roles
	if self.lastRole != self.role:
	    self.roleCounter = 0
	    self.lastDiffRole = self.lastRole
	else:
	    self.roleCounter += 1

	# Update memory of sub roles
	if self.lastSubRole != self.subRole:
	    self.subRoleCounter = 0
	    self.lastDiffSubRole = self.lastSubRole
	else:
	    self.subRoleCounter += 1
	    
	self.lastRole = self.role
	self.lastSubRole = self.subRole
	self.lastSubRoleOnDeck = self.subRoleOnDeck

	return tuple(self.position), self.role, self.formation, self.strategy,\
	    self.subRole

    def get_goalie_action(self):
	"""
	Gets everything we need for the goalie
	"""
	# Figure out if teammates are dead or penalized
	self.updateTeammateStuff()
	# Update my information
	self.teammates[0].updateMe()

	return self.noCalledChaser()

    def strategize(self):
	'''
	Returns Strategy based on score, game situation, opponent, etc.

	Currently we have one strategy: win.
	'''	
	self.strategy = self.WIN

	if self.strategy == self.WIN:
	    self.sWin()
	else:
	    print "PlayBook::strategize(): unknown strategy"

    ### STRATEGIES ###

    def sWin(self):
	'''
	Returns best 'formation' on variety of factors:
	-number of teammates/opponents penalized.
	-global ball uncertainty
	-position of ball on the field

	'''
        ## GAME READY ##
	if self.brain.game.state == Constants.GAME_READY:
	    # team is kicking off
	    self.formation = self.READY
    
        ## GAME PLAYING ##
	elif self.brain.game.state == Constants.GAME_PLAYING:
	    if (self.brain.game.getTimeSincePlay() < 
		self.KICKOFF_FORMATION_TIME and 
		not TEST_DEFENDER and
		not TEST_OFFENDER and
		not TEST_CHASER):
		if KICKOFF_PLAY and self.getNumInactiveFieldPlayers() == 0:   
		    self.formation = self.KICKOFF_PLAY
		else:
		    self.formation = self.KICKOFF
	    elif self.shouldUseDubD():#self.ballInMyGoalBox():
	     	self.formation = self.DUB_D
	    # ball hasn't been seen by me or teammates in a while
	    elif (self.brain.timeSinceBallSeen() > self.FINDER_TIME_THRESH and
		  self.brain.game.getTimeSinceUnpenalized() >
		  self.FINDER_TIME_THRESH):
		self.formation = self.FINDER
	    else:
		self.formation = self.NORMAL	
	
        # formation allocation
	if self.formation == self.NORMAL:
	    self.fNormal()
	elif self.formation == self.DUB_D:
	    self.fDubD()
	elif self.formation == self.FINDER:
	    self.fFinder()
	elif self.formation == self.KICKOFF:
	    self.fKickoff()
	elif self.formation == self.KICKOFF_PLAY:
	    self.fKickoffPlay()
	elif self.formation == self.READY:
	    self.fReady()
	else:
	    print "PlayBook::sWin(): formation unknown"

    ### FORMATIONS ###

    def fNormal(self):
	# testing switches -- look up top
	if TEST_DEFENDER:
	    self.role = self.DEFENDER
	    self.rDefender()
	    return
	elif TEST_OFFENDER:
	    self.role = self.OFFENDER
	    self.rOffender()
	    return
	elif TEST_CHASER:
	    self.role = self.CHASER
	    self.rChaser()
	    return
	elif TEST_MIDDIE:
	    self.role = self.MIDDIE
	    self.rMiddie()
	    return
	elif (self.playerNumber == 2 and
	      self.getNumInactiveFieldPlayers() == 3):	    
	    if self.brain.ball.y > Constants.MIDFIELD_Y + 30.0:
		if (self.role == self.CHASER and 
		    self.brain.ball.y < Constants.MIDFIELD_Y + 60.0):
		    pass
		else:
		    self.role = self.DEFENDER
		    self.rDefender()
	    else:
		if (self.role == self.DEFENDER and 
		    self.brain.ball.y > Constants.MIDFIELD_Y):
		    pass
		else:
		    self.role = self.CHASER
		    self.rChaser()
	    return

	# gets teammate that is chaser (could be me)
	chaser_mate = self.determineChaser()
	chaser_mate.role = self.CHASER
	
	# if i am chaser
	if chaser_mate.playerNumber == self.playerNumber:
	    self.role = self.CHASER
	# We now figure out the moving of everyone else
	else:
	    other_teammates = self.getNonChaserTeammates(chaser_mate)

	    # If two robots are inactive, we become defender
	    if self.getNumInactiveFieldPlayers() == 2:
		self.role = self.DEFENDER
		other_teammates[0].role = self.OFFENDER
		other_teammates[1].role = self.MIDDIE

	    # If one robot is inactive, no middie
	    elif self.getNumInactiveFieldPlayers() == 1: 

		# get fellow teammate who isn't chaser
		for mate in other_teammates:
		    if not mate.inactive:
			other_teammate = mate
		    else:
			mate.role = self.MIDDIE
   
		# Farther back player is defender
		if self.brain.my.playerNumber > other_teammate.playerNumber:
		    if ((self.brain.my.y - other_teammate.y < 
			self.TEAMMATE_POSITIONING_USE_NUMBERS_BUFFER)
			or (self.brain.my.y - other_teammate.y < 
			    2*self.TEAMMATE_POSITIONING_USE_NUMBERS_BUFFER and 
			    self.role == self.DEFENDER)):
			self.role = self.DEFENDER
			other_teammate.role = self.OFFENDER
		    else:
			self.role = self.OFFENDER
			other_teammate.role = self.DEFENDER
		else:
		    if (self.brain.my.y < other_teammate.y + 50.0 and 
			other_teammate.calledRole != self.DEFENDER):
			self.role = self.DEFENDER
			self.other_teammate = self.OFFENDER
		    else:
			self.role = self.OFFENDER
			self.other_teammate = self.DEFENDER

		    
	    else:
		# Prioritizing positioning by deciding defender first
		# Determine order of Y values, lowest to highest
		# Winner is Defender, second is middie, third is offender
		minY = self.brain.my.y
		maxY = self.brain.my.y
		minPlayer = self.playerNumber
		maxPlayer = self.playerNumber

		# Assign the appropriate roles
		for mate in other_teammates:
		    if mate.y < minY:
			minY = mate.y
			minPlayer = mate.playerNumber
		    if mate.y > maxY:
			maxY = mate.y
			maxPlayer = mate.playerNumber

		if self.playerNumber == maxPlayer:
		    self.role = self.OFFENDER

		elif self.playerNumber == minPlayer:
		    self.role = self.DEFENDER

		else:
		    self.role = self.MIDDIE

		# Tie-break, close calls go to the higher number
		for mate in other_teammates:
		    if self.playerNumber > mate.playerNumber:
			pass
        # Perform our assigned role
        self.performRole()
		
    def fDubD(self):
	# If we're down a player, use different positions
	if self.getNumInactiveFieldPlayers() == 2:

	    # Figure out who isn't penalized with you
	    other_teammate = self.getOtherActiveTeammate()
	    
	    # Determine if we should have two defenders or a defender 
	    # and a middie dependent on score differential
	    if self.getGoalDifferential() >= 2:
		pos1 = (self.SWEEPER_X, self.SWEEPER_Y)
		currentCenterMidX = self.brain.clip(self.brain.ball.x, 
						    self.CENTER_MID_LEFT_LIMIT, 
						    self.CENTER_MID_RIGHT_LIMIT)
		pos2 = (currentCenterMidX, 
			self.OFFENSIVE_CENTER_MIDFIELD_Y)
	    else:
		pos1 = self.LEFT_DEEP_BACK_POS
		pos2 = self.RIGHT_DEEP_BACK_POS
		self.role = self.DEFENDER

	    # Figure out who should go to which position
	    pos = self.getLeastWeightPosition((pos1,pos2), other_teammate)
	    if pos == self.LEFT_DEEP_BACK_POS:
		self.role = self.DEFENDER
		self.subRole = self.LEFT_DEEP_BACK

	    elif pos == self.RIGHT_DEEP_BACK_POS:
		self.role = self.DEFENDER
		self.subRole = self.RIGHT_DEEP_BACK

	    else:
		self.role = self.OFFENDER
		self.subRole = self.CENTER_O_MIDFIELD

	# If we are the only player, become the sweeper
	elif self.getNumInactiveFieldPlayers() == 3:
	    pos = (self.SWEEPER_X, self.SWEEPER_Y)
	    self.role = self.DEFENDER
	    self.subRole = self.SWEEPER

	# We have three dogs, we can have a middie and two deep backs
	elif self.getNumInactiveFieldPlayers() == 1:
	    # Assign role based on closest position
	    currentCenterMidX = self.brain.clip(self.brain.ball.x, 
						self.CENTER_MID_LEFT_LIMIT, 
						self.CENTER_MID_RIGHT_LIMIT)
	    currentCenterMidPos = (currentCenterMidX, 
				   self.OFFENSIVE_CENTER_MIDFIELD_Y)
	    playerPos = self.getLeastWeightPosition((self.LEFT_DEEP_BACK_POS,
						     self.RIGHT_DEEP_BACK_POS,
						     currentCenterMidPos),
					       self.getOtherActiveTeammates())

	    pos = playerPos[self.playerNumber - 1]

	    if pos == self.LEFT_DEEP_BACK_POS:
		self.role = self.DEFENDER
		self.subRole = self.LEFT_DEEP_BACK

	    elif pos == self.RIGHT_DEEP_BACK_POS:
		self.role = self.DEFENDER
		self.subRole = self.RIGHT_DEEP_BACK

	    else:
		self.role = self.OFFENDER
		self.subRole = self.CENTER_O_MIDFIELD

	    # Tie breaking
	    if not self.highestActivePlayerNumber():
		for i in xrange(self.brain.my.playerNumber+1, 5):
		    if (self.teammates[i-1].calledSubRole == self.subRole
			and not self.teammates[i-1].inactive):
			pos = playerPos[i - 1]
			if pos == self.LEFT_DEEP_BACK_POS:
			    self.role = self.DEFENDER
			    self.subRole = self.LEFT_DEEP_BACK

			elif pos == self.RIGHT_DEEP_BACK_POS:
			    self.role = self.DEFENDER
			    self.subRole = self.RIGHT_DEEP_BACK

			else:
			    self.role = self.OFFENDER
			    self.subRole = self.CENTER_O_MIDFIELD
			break

	# We have four dogs, two defenders, two middies?
	else:
	    # Assign role based on closest position
	    leftMiddiePos = (self.CENTER_MID_LEFT_LIMIT,
			     self.DEFENSIVE_CENTER_MIDFIELD_Y)
	    rightMiddiePos = (self.CENTER_MID_RIGHT_LIMIT,
			      self.OFFENSIVE_CENTER_MIDFIELD_Y)

	    playerPos = self.getLeastWeightPosition((self.LEFT_DEEP_BACK_POS,
						     self.RIGHT_DEEP_BACK_POS,
						     leftMiddiePos,
						     rightMiddiePos))

	    pos = playerPos[self.playerNumber - 1]

	    if pos == self.LEFT_DEEP_BACK_POS:
		self.role = self.DEFENDER
		self.subRole = self.LEFT_DEEP_BACK

	    elif pos == self.RIGHT_DEEP_BACK_POS:
		self.role = self.DEFENDER
		self.subRole = self.RIGHT_DEEP_BACK

	    elif pos == leftMiddiePos:
		self.role = self.MIDDIE
		self.subRole = self.RIGHT_O_MIDDIE

	    else:
		self.role = self.OFFENDER
		self.subRole = self.CENTER_O_MIDFIELD

	    # add some tiebreaking here
	    if not self.highestActivePlayerNumber():
		for i in xrange(self.brain.my.playerNumber+1, 5):
		    if self.teammates[i-1].calledSubRole == self.subRole:
			pos = playerPos[i - 1]
			if pos == self.LEFT_DEEP_BACK_POS:
			    self.role = self.DEFENDER
			    self.subRole = self.LEFT_DEEP_BACK

			elif pos == self.RIGHT_DEEP_BACK_POS:
			    self.role = self.DEFENDER
			    self.subRole = self.RIGHT_DEEP_BACK

			elif pos == leftMiddiePos:
			    self.role = self.MIDDIE
			    self.subRole = self.RIGHT_O_MIDDIE

			else:
			    self.role = self.OFFENDER
			    self.subRole = self.CENTER_O_MIDFIELD

			break

	# position setting
	self.moveMyCharge(pos)
	self.position = self.getAttractorPosition()

    def fFinder(self):
	'''no one knows where the ball is'''
	self.role = self.SEARCHER
	self.rSearcher()

    def fKickoff(self):
	'''time immediately after kickoff'''
	num_inactive_teammates = self.getNumInactiveFieldPlayers()
	if num_inactive_teammates == 0:
	    if self.playerNumber == 2:
		self.role = self.DEFENDER
	    elif self.playerNumber == 3:
		self.role = self.CHASER
	    elif self.playerNumber == 4:
		self.role = self.OFFENDER
	    elif self.playerNumber == 5:
		self.role = self.MIDDIE
	elif num_inactive_teammates == 2:
	    other_teammate = self.getOtherActiveTeammate()
	    if self.playerNumber > other_teammate.playerNumber:
		self.role = self.CHASER
	    else:
		self.role = self.DEFENDER
	else:
	    self.role = self.CHASER

	if self.role == self.CHASER:
	    self.subRole = self.CHASE_NORMAL
	    self.rChaser()
	elif self.role == self.OFFENDER:
	    self.subRole = self.KICKOFF_STRIKER
	    self.pKickoffStriker()
	elif self.role == self.DEFENDER:
	    self.subRole = self.KICKOFF_SWEEPER
	    self.pKickoffSweeper()
	elif self.role == self.MIDDIE:
	    self.subRole = self.KICKOFF_MIDDIE
	    self.pKickoffMiddie()
	else:
	    print "PlayBook::fNormal(): role not assigned"

	self.position = self.getAttractorPosition()

    def fKickoffPlay(self):
	'''time immediately after kickoff'''
	if self.playerNumber == 2:
	    self.role = self.DEFENDER
	    self.subRole = self.KICKOFF_SWEEPER
	    self.pKickoffPlaySweeper()
	elif self.playerNumber == 3:
	    self.role = self.CHASER
	    self.subRole = self.CHASER
	    self.rChaser()
	elif self.playerNumber == 4:
	    self.role = self.OFFENDER
	    self.subRole = self.KICKOFF_STRIKER
	    self.pKickoffPlayStriker()
	elif self.playerNumber == 5:
	    self.role = self.MIDDIE
	    self.subRole = self.KICKOFF_MIDDIE
	    self.pKickoffPlayMiddie()

	self.position = self.getAttractorPosition()

    def fReady(self):
	'''kickoff positions'''

	self.position = []
	# ready state depends on number of players alive
	num_inactive_teammates = self.getNumInactiveFieldPlayers()
	kickOff = self.brain.game.myTeam.kickOff

	# if three dogs alive, position normally
	if num_inactive_teammates == 0:
	    if self.playerNumber == 2:
		self.role = self.DEFENDER
		if kickOff:
		    if self.kickoff_formation == 0:
			self.position = self.READY_KICKOFF_DEFENDER_0
		    else:
			self.position = self.READY_KICKOFF_DEFENDER_1
		else:
		    self.position = self.READY_NON_KICKOFF_DEFENDER
	    elif self.playerNumber == 3:
		self.role = self.CHASER
		if kickOff:
		    self.position = self.READY_KICKOFF_NORMAL_CHASER
		else:
		    self.position = self.READY_NON_KICKOFF_CHASER
	    elif self.playerNumber == 4:
		self.role = self.OFFENDER
		if kickOff:
		    if self.kickoff_formation == 0:
			self.position = self.READY_KICKOFF_OFFENDER_0
		    else:
			self.position = self.READY_KICKOFF_OFFENDER_1
		else:
		    self.position = self.READY_NON_KICKOFF_OFFENDER
	    elif self.playerNumber == 5:
		self.role = self.MIDDIE
		if kickOff:
		    if self.kickoff_formation == 0:
			self.position = self.READY_KICKOFF_MIDDIE_0
		    else:
			self.position = self.READY_KICKOFF_MIDDIE_1
		else:
		    self.position = self.READY_NON_KICKOFF_MIDDIE
	    else:
		pass

	# two dogs alive, alter positions a bit
	elif num_inactive_teammates == 1:
	    other_teammate = self.getOtherActiveTeammate()

	    if self.playerNumber > other_teammate.playerNumber:
		self.role = self.CHASER
		if kickOff:
		    self.position = self.READY_KICKOFF_NORMAL_CHASER
		else:
		    self.position = self.READY_NON_KICKOFF_LEFT_POSITION
	    else:
		self.role = self.DEFENDER
		if kickOff:
		    self.position = self.READY_KICKOFF_STOPPER
		else:
		    self.position = self.READY_NON_KICKOFF_RIGHT_POSITION

	# just you
	else:
	    self.role = self.CHASER
	    if kickOff:
		self.position = self.READY_KICKOFF_NORMAL_CHASER
	    else:
		self.position = self.READY_NON_KICKOFF_CHASER

    ### ROLES ###
    
    def rDefender(self):
	'''gets positioning for defender'''

	# If the ball is deep in our side, we become a sweeper
	if self.brain.ball.y < self.SWEEPER_Y:
	    self.subRoleOnDeck = self.SWEEPER

	# Ball is deeper into the opponent side, move up to be center mid	
	elif (self.brain.ball.y > self.CENTER_MID_DEFENDER_LIMIT and 
	      not DEFENDER_CHASER and
	      not DEEP_STOPPER):
	    self.subRoleOnDeck = self.DEFENSIVE_MIDFIELD

	# Stand between the ball and the back of the goal if it is on our side	
	elif DEEP_STOPPER:
	    self.subRoleOnDeck = self.DEEP_STOPPER
	else:
	    self.subRoleOnDeck = self.STOPPER
	    
	# if we've just switched into a new role, switch sub roles immediately
	if self.role != self.lastRole:
	    self.subRole = self.subRoleOnDeck
	    self.subRoleOnDeckCounter = 0
	else:
	    # if the subRole on deck != current sub role
	    if (self.subRoleOnDeck is not None and 
		self.subRoleOnDeck != self.subRole):
		# if the sub role on deck is the same as the last, increment
		if self.subRoleOnDeck == self.lastSubRoleOnDeck:
		    self.subRoleOnDeckCounter += 1
		# otherwise our idea of subRole has changed, null counter
		else:
		    self.subRoleOnDeckCounter = 0

		# if we are sure that we want to switch into this sub role
		if self.subRoleOnDeckCounter > self.SUB_ROLE_SWITCH_BUFFER:
		    self.subRole = self.subRoleOnDeck
		    self.subRoleOnDeckCounter = 0

	if self.subRole == self.SWEEPER:
	    self.pSweeper()
	elif self.subRole == self.DEFENSIVE_MIDFIELD:
	    self.pDefensiveMidfield()
	elif self.subRole == self.STOPPER:
	    self.pStopper()
	elif self.subRole == self.DEEP_STOPPER:
	    self.pDeepStopper()
	else:
	    pass

	self.position = self.getAttractorPosition()


    def rOffender(self):
	'''in backfield: stay on ball x, but stay on upfield.
	in forefield: pick opposite left or right side'''

	# CENTER_O_MIDFIELD if it ball goes deep in our territory
	if self.brain.ball.y <= self.CENTER_MID_OFFENDER_LIMIT:
	    if (self.subRole == self.FORWARD_MIDFIELD and
		self.brain.ball.y > self.CENTER_MID_OFFENDER_LIMIT -
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.CENTER_O_MIDFIELD

	# ATTACKING_MIDFIELD if ball is in the opponent goalbox
	elif (self.brain.ball.y > Constants.OPP_GOALBOX_BOTTOM_Y and 
	      Constants.OPP_GOALBOX_LEFT_X < self.brain.ball.x <
	      Constants.OPP_GOALBOX_RIGHT_X):
	    # Add buffers for wings
	    if (self.subRole == self.LEFT_WING and 
		self.brain.ball.y < Constants.OPP_GOALBOX_BOTTOM_Y + 
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE and
		self.brain.ball.x > Constants.CENTER_FIELD_X - 
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    elif (self.subRole == self.RIGHT_WING and
		  self.brain.ball.y < Constants.OPP_GOALBOX_BOTTOM_Y + 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE and
		  self.brain.ball.x < Constants.CENTER_FIELD_X + 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Buffer moving from strikers
	    elif (self.subRole == self.LEFT_STRIKER and
		  self.brain.ball.x > Constants.OPP_GOALBOX_RIGHT_X - 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    elif (self.subRole == self.RIGHT_STRIKER and
		  self.brain.ball.x < Constants.OPP_GOALBOX_LEFT_X + 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.ATTACKING_MIDFIELD

	# LEFT STRIKER if the ball is in the right corner
	elif (self.brain.ball.y > self.OPP_CORNERS_Y and 
	      self.brain.ball.x < Constants.OPP_GOALBOX_LEFT_X):
	    # if in left wing, have a bigger buffer to switch to right striker
	    if (self.subRole == self.LEFT_WING and
		self.brain.ball.y < self.OPP_CORNERS_Y + 
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # have a larget buffer for the sideline sub roles
	    elif (self.subRole == self.RIGHT_SIDELINE_O and
		  self.brain.ball.y < self.OPP_CORNERS_Y +
		  self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer moving from attacking midfield
	    elif (self.subRole == self.ATTACKING_MIDFIELD and
		  self.brain.ball.x > Constants.OPP_GOALBOX_LEFT_X - 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.RIGHT_STRIKER

	# RIGHT STRIKER if the ball is in the right corner		
	elif (self.brain.ball.y > self.OPP_CORNERS_Y and
	      self.brain.ball.x > Constants.OPP_GOALBOX_RIGHT_X):
	    # Buffer moving from the wing and sideline
	    if (self.subRole == self.RIGHT_WING and
		self.brain.ball.y < self.OPP_CORNERS_Y + 
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # have a larget buffer for the sideline sub roles
	    elif (self.subRole == self.LEFT_SIDELINE_O and
		  self.brain.ball.y < self.OPP_CORNERS_Y +
		  self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer moving from attacking midfield
	    elif (self.subRole == self.ATTACKING_MIDFIELD and
		  self.brain.ball.x < Constants.OPP_GOALBOX_RIGHT_X + 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.LEFT_STRIKER

	# RIGHT_SIDELINE_OFFENDER if ball is near right sideline
	elif (self.brain.ball.y > Constants.CENTER_FIELD_Y and
	      self.brain.ball.x >= self.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X):
	    # Buffer with striker
	    if (self.subRole == self.LEFT_STRIKER and
		self.brain.ball.y > self.OPP_CORNERS_Y - 
		self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with wing
	    elif (self.subRole == self.LEFT_WING and self.brain.ball.x < 
		  self.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X +
		  self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with forward middie
	    elif (self.subRole == self.FORWARD_MIDFIELD and
		  (self.brain.ball.y < Constants.CENTER_FIELD_Y + 
		   self.SIDELINE_BUFFER_SIZE or
		   self.brain.ball.x <self.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X
		   + self.SIDELINE_BUFFER_SIZE)):
		pass
	    else:
		self.subRoleOnDeck = self.RIGHT_SIDELINE_O

	# LEFT_SIDELINE_OFFENDER
	elif (self.brain.ball.y > Constants.CENTER_FIELD_Y and
	      self.brain.ball.x < self.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X):
	    # Buffer with striker
	    if (self.subRole == self.RIGHT_STRIKER and
		self.brain.ball.y > self.OPP_CORNERS_Y - 
		self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with wing
	    elif (self.subRole == self.RIGHT_WING and self.brain.ball.x > 
		  self.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X -
		  self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with forward middie
	    elif (self.subRole == self.FORWARD_MIDFIELD and
		  (self.brain.ball.y < Constants.CENTER_FIELD_Y + 
		   self.SIDELINE_BUFFER_SIZE or
		   self.brain.ball.x > self.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X
		   - self.SIDELINE_BUFFER_SIZE)):
		pass
	    else:
		self.subRoleOnDeck = self.LEFT_SIDELINE_O

 	# FORWARD_MIDFIELD if ball is on our half but not deep
 	elif self.brain.ball.y <= self.FORWARD_MIDFIELD_BALL_THRESH_Y:
	    # Buffer with center middie
	    if (self.subRole == self.CENTER_O_MIDFIELD and
		self.brain.ball.y < self.CENTER_MID_OFFENDER_LIMIT +
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Buffer with sideline offenders
	    elif ((self.subRole == self.RIGHT_SIDELINE_O or
		   self.subRole == self.LEFT_SIDELINE_O) and
		  self.brain.ball.y > Constants.CENTER_FIELD_Y -
		  self.SIDELINE_BUFFER_SIZE):
		pass
	    elif (self.subRole == self.RIGHT_SIDELINE_O and 
		  self.brain.ball.x < self.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X
		  + self.SIDELINE_BUFFER_SIZE):
		pass
	    elif (self.subRole == self.LEFT_SIDELINE_O and 
		  self.brain.ball.x > self.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X
		  - self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with wings
	    elif ((self.subRole == self.RIGHT_WING or
		   self.subRole == self.LEFT_WING) and
		  self.brain.ball.y > self.FORWARD_MIDFIELD_BALL_THRESH_Y -
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.FORWARD_MIDFIELD	    	     
 
	# RIGHT_WING  if ball is in opp half but not in a corner
	elif self.brain.ball.x < Constants.CENTER_FIELD_X:

	    # Don't switch from left wing if the ball is close to center field
	    if (self.subRole == self.LEFT_WING and 
		self.brain.ball.x > Constants.CENTER_FIELD_X - 
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Don't switch out of striker if ball just left the corner
	    elif (self.subRole == self.RIGHT_STRIKER and
		  self.brain.ball.y > self.OPP_CORNERS_Y - 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Buffer with attacking midfield
	    elif (self.subRole == self.ATTACKING_MIDFIELD and
		  self.brain.ball.y > Constants.OPP_GOALBOX_BOTTOM_Y - 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Buffer with left sideline
	    elif (self.subRole == self.LEFT_SIDELINE_O and
		  self.brain.ball.x > self.LEFT_SIDELINE_OFFENDER_BALL_THRESH_X
		  - self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with forward middie
	    elif (self.subRole == self.FORWARD_MIDFIELD and
		  self.brain.ball.y < self.FORWARD_MIDFIELD_BALL_THRESH_Y + 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.RIGHT_WING

	# LEFT_WING otherwise
	else:
	    # Don't switch from right wing if the ball is close to center field
	    if (self.subRole == self.RIGHT_WING and 
		self.brain.ball.x < Constants.CENTER_FIELD_X + 
		self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass

	    # Don't switch out of striker if ball just left the corner
	    elif (self.subRole == self.LEFT_STRIKER and
		  self.brain.ball.y > self.OPP_CORNERS_Y - 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Buffer with attacking midfield
	    elif (self.subRole == self.ATTACKING_MIDFIELD and
		  self.brain.ball.y > Constants.OPP_GOALBOX_BOTTOM_Y - 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    # Buffer with right sideline
	    elif (self.subRole == self.RIGHT_SIDELINE_O and
		  self.brain.ball.x < self.RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X
		  + self.SIDELINE_BUFFER_SIZE):
		pass
	    # Buffer with forward middie
	    elif (self.subRole == self.FORWARD_MIDFIELD and
		  self.brain.ball.y < self.FORWARD_MIDFIELD_BALL_THRESH_Y + 
		  self.OFFENDER_SUB_ROLE_BUFFER_SIZE):
		pass
	    else:
		self.subRoleOnDeck = self.LEFT_WING

	# if we've just switched into a new role, switch sub roles immediately
	if self.role != self.lastRole:
	    self.subRole = self.subRoleOnDeck
	    self.subRoleOnDeckCounter = 0
	else:
	    # if the subRole on deck != current sub role
	    if (self.subRoleOnDeck is not None and 
		self.subRoleOnDeck != self.subRole):
		# if the sub role on deck is the same as the last, increment
		if self.subRoleOnDeck == self.lastSubRoleOnDeck:
		    self.subRoleOnDeckCounter += 1
		# otherwise our idea of subRole has changed, null counter
		else:
		    self.subRoleOnDeckCounter = 0

		# if we are sure that we want to switch into this sub role
		if self.subRoleOnDeckCounter > self.SUB_ROLE_SWITCH_BUFFER:
		    self.subRole = self.subRoleOnDeck
		    self.subRoleOnDeckCounter = 0
		    print "New Subrole", self.SUB_ROLES[self.subRole]

	# subRole assignment
	if self.subRole == self.CENTER_O_MIDFIELD:
	    self.pCenterOMidfield()
	elif self.subRole == self.FORWARD_MIDFIELD:
	    self.pForwardMidfield()
	elif self.subRole == self.ATTACKING_MIDFIELD:
	    self.pAttackingMidfield()
	elif self.subRole == self.RIGHT_STRIKER:
	    self.pRightStriker()
	elif self.subRole == self.LEFT_STRIKER:
	    self.pLeftStriker()
	elif self.subRole == self.RIGHT_WING:
	    self.pRightWing()
	elif self.subRole == self.LEFT_WING:
	    self.pLeftWing()
	elif self.subRole == self.RIGHT_SIDELINE_O:
	    self.pRightSidelineO()
	elif self.subRole == self.LEFT_SIDELINE_O:
	    self.pLeftSidelineO()
	else:
	    pass

	self.position = self.getAttractorPosition()
    
    def rMiddie(self):
	'''gets position for the middie'''
	
	# Simple start, we move to a get out of the way place
	# Middfielder pretty much avoids play, really needs to be changed
	# Buffering needs to be added
	if self.ball.y < Constants.MIDFIELD_Y:
	    if self.ball.x < Constants.MIDFIELD_X:
		self.subRoleOnDeck = self.RIGHT_O_MIDDIE
	    else:
		self.subRoleOnDeck = self.LEFT_O_MIDDIE
	else:
	    if self.ball.x < Constants.MIDFIELD_X:
		self.subRoleOnDeck = self.RIGHT_D_MIDDIE
	    else:
		self.subRoleOnDeck = self.LEFT_D_MIDDIE

	# Sub-role timing control from pOffender and pDefender
	if self.role != self.lastRole:
	    self.subRole = self.subRoleOnDeck
	    self.subRoleOnDeckCounter = 0
	else:
	    # if the subRole on deck != current sub role
	    if (self.subRoleOnDeck is not None and 
		self.subRoleOnDeck != self.subRole):
		# if the sub role on deck is the same as the last, increment
		if self.subRoleOnDeck == self.lastSubRoleOnDeck:
		    self.subRoleOnDeckCounter += 1
		# otherwise our idea of subRole has changed, null counter
		else:
		    self.subRoleOnDeckCounter = 0

		# if we are sure that we want to switch into this sub role
		if self.subRoleOnDeckCounter > self.SUB_ROLE_SWITCH_BUFFER:
		    self.subRole = self.subRoleOnDeck
		    self.subRoleOnDeckCounter = 0
		    print "New Subrole", self.SUB_ROLES[self.subRole]

	if self.subRole == self.RIGHT_O_MIDDIE:
	    self.pRightOMiddie()
	elif self.subRole == self.LEFT_O_MIDDIE:
	    self.pLeftOMiddie()
	elif self.subRole == self.RIGHT_D_MIDDIE:
	    self.pRightDMiddie()
	elif self.subRole == self.LEFT_D_MIDDIE:
	    self.pLeftDMiddie()
	else:
	    pass

	self.position = self.getAttractorPosition()

    def rChaser(self):

# 	if (self.formation == self.KICKOFF and 
# 	    self.brain.game.getTimeSincePlay() < 2000.0 and
# 	    self.brain.ball.framesOff > 5):
	    
	# If we are in the corners or the ball is in a corner, we avoid the box
	if ((self.ball.x < Constants.MY_GOALBOX_LEFT_X and 
	     self.ball.y < Constants.MY_GOALBOX_TOP_Y and 
	     self.my.x > Constants.MY_GOALBOX_LEFT_X) or 
	    (self.ball.x > Constants.MY_GOALBOX_RIGHT_X and
	     self.ball.y < Constants.MY_GOALBOX_TOP_Y and
	     self.my.x < Constants.MY_GOALBOX_RIGHT_X) or 
	    (self.my.x < Constants.MY_GOALBOX_LEFT_X and
	     self.my.y < Constants.MY_GOALBOX_TOP_Y and
	     self.ball.x > Constants.MY_GOALBOX_LEFT_X) or 
	    (self.my.x > Constants.MY_GOALBOX_RIGHT_X and
	     self.my.y < Constants.MY_GOALBOX_TOP_Y and
	     self.ball.x < Constants.MY_GOALBOX_RIGHT_X)):	    
	    self.moveMyCharge((self.ball.x,self.ball.y))
	    self.subRole = self.CHASE_AROUND_BOX
	    self.position = self.getAttractorPosition()
	# Almost always chase normal, i.e. without potential fields
	else:	    
	    self.subRole = self.CHASE_NORMAL
	    self.position = (self.brain.my.x,self.brain.my.y,False,False)

    def rSearcher(self):
	'''
	Determines positioning for robots while using the finder formation
	'''
	if self.getNumInactiveFieldPlayers() == 3:
	    pos = self.READY_NON_KICKOFF_DEFENDER
	elif self.getNumInactiveFieldPlayers() == 2:
	    pos = self.getLeastWeightPosition(self.TWO_DOG_FINDER_POSITIONS,
					      self.getOtherActiveTeammate())
	elif self.getNumInactiveFieldPlayers() == 1:
	    playerPos = self.getLeastWeightPosition(
		self.THREE_DOG_FINDER_POSITIONS,self.getOtherActiveTeammates())
	    print "playerPos: ", playerPos
	    pos = playerPos[self.playerNumber - 1]
	    print "initial pos: ", pos
	    self.role = self.SEARCHER
	    if pos == self.THREE_DOG_FINDER_POSITIONS[0]:
		self.subRole = self.LEFT_FINDER

	    elif pos == self.THREE_DOG_FINDER_POSITIONS[1]:
		self.subRole = self.RIGHT_FINDER

	    else:
		self.subRole = self.FRONT_FINDER

	    if not self.highestActivePlayerNumber():
		for i in xrange(self.playerNumber+1, 5):
		    if (self.teammates[i-1].calledSubRole == self.subRole
			and not self.teammates[i-1].inactive):
			pos = playerPos[i - 1]
			if pos == self.THREE_DOG_FINDER_POSITIONS[0]:
			    self.subRole = self.LEFT_FINDER

			elif pos == self.THREE_DOG_FINDER_POSITIONS[1]:
			    self.subRole = self.RIGHT_FINDER

			else:
			    self.subRole = self.FRONT_FINDER
			break

	    print "three dogs pSearcher:",pos

	else:
	    playerPos = self.getLeastWeightPosition(
		self.FOUR_DOG_FINDER_POSITIONS)
	    pos = playerPos[self.playerNumber - 1]
	    self.role = self.SEARCHER
	    if pos == self.FOUR_DOG_FINDER_POSITIONS[0]:
		self.subRole = self.LEFT_FINDER

	    elif pos == self.FOUR_DOG_FINDER_POSITIONS[1]:
		self.subRole = self.RIGHT_FINDER

	    elif pos == self.FOUR_DOG_FINDER_POSITIONS[2]:
		self.subRole = self.FRONT_FINDER

	    else:
		self.subRole = self.OTHER_FINDER

	    if not self.highestActivePlayerNumber():
		for i in xrange(self.playerNumber+1, 5):
		    if self.teammates[i-1].calledSubRole == self.subRole:
			pos = playerPos[i - 1]
			if pos == self.FOUR_DOG_FINDER_POSITIONS[0]:
			    self.subRole = self.LEFT_FINDER

			elif pos == self.FOUR_DOG_FINDER_POSITIONS[1]:
			    self.subRole = self.RIGHT_FINDER

			elif pos == self.FOUR_DOG_FINDER_POSITIONS[2]:
			    self.subRole = self.FRONT_FINDER

			else:
			    self.subRole = self.OTHER_FINDER
			break


	self.moveMyCharge(pos)
	self.position = self.getAttractorPosition()

    #### SUB_ROLES ####

    def pLeftWing(self):
	'''position left winger'''
	x = self.brain.clip(self.brain.ball.x - self.WING_X_OFFSET,
			    self.LEFT_WING_MIN_X, self.LEFT_WING_MAX_X)
	y = self.brain.clip(self.brain.ball.y - self.WING_Y_OFFSET,
			    self.WING_MIN_Y, self.WING_MAX_Y)
	self.moveMyCharge((x,y))

    def pRightWing(self):
	'''position right winger'''
	x = self.brain.clip(self.brain.ball.x + self.WING_X_OFFSET,
			    self.RIGHT_WING_MIN_X, self.RIGHT_WING_MAX_X)
	y = self.brain.clip(self.brain.ball.y - self.WING_Y_OFFSET,
			    self.WING_MIN_Y, self.WING_MAX_Y)
	self.moveMyCharge((x,y))

    def pLeftStriker(self):
	'''position left striker'''
	x = self.STRIKER_LEFT_X
	y = self.STRIKER_Y
	self.moveMyCharge((x,y))

    def pRightStriker(self):
	'''position right striker'''
	x = self.STRIKER_RIGHT_X
	y = self.STRIKER_Y
	self.moveMyCharge((x,y))	

    def pForwardMidfield(self):
	'''position forward midfield'''
	# Make our x equal with the ball, unless it is too far to the side
	x = self.brain.clip(self.brain.ball.x, self.FORWARD_MID_LEFT_LIMIT, 
			    self.FORWARD_MID_RIGHT_LIMIT)
	y = self.FORWARD_MIDFIELD_Y
	self.moveMyCharge((x,y))

    def pAttackingMidfield(self):
	'''position attacking midfield'''
	# Make our x equal with the ball, unless it is too far to the 
	x = self.brain.clip(self.brain.ball.x, self.STRIKER_LEFT_X,  
			    self.STRIKER_RIGHT_X)
	
	y = self.STRIKER_Y - 30. 
	self.moveMyCharge((x,y))

    def pCenterOMidfield(self):
	'''position center middy'''
	# Make our x equal with the ball, unless it is too far to the side
	x = self.brain.clip(self.brain.ball.x, self.CENTER_MID_LEFT_LIMIT, 
			    self.CENTER_MID_RIGHT_LIMIT)
	y = self.OFFENSIVE_CENTER_MIDFIELD_Y
	self.moveMyCharge((x,y))

    def pRightSidelineO(self):
	'''position right sideline offender '''
	x = self.RIGHT_SIDELINE_O_X
	y = self.brain.clip(self.brain.ball.y - self.SIDELINE_O_Y_OFFSET, 
			    self.SIDELINE_O_MIN_Y, self.SIDELINE_O_MAX_Y)
	self.moveMyCharge((x,y))

    def pLeftSidelineO(self):
	'''position left sideline offender '''
	x = self.LEFT_SIDELINE_O_X
	y = self.brain.clip(self.brain.ball.y - self.SIDELINE_O_Y_OFFSET, 
			    self.SIDELINE_O_MIN_Y, self.SIDELINE_O_MAX_Y)
	self.moveMyCharge((x,y))

    def pStopper(self):
	'''position stopper'''
	x,y = self.getPointBetweenBallAndGoal(self.DEFENDER_BALL_DIST)
	y = self.brain.clip(y, self.SWEEPER_Y,
			    self.DEFENSIVE_CENTER_MIDFIELD_Y)
	self.moveMyCharge((x,y))

    def pDeepStopper(self):
	'''position stopper'''
	x,y = self.getPointBetweenBallAndGoal(self.DEFENDER_BALL_DIST)
	y = self.brain.clip(y, self.SWEEPER_Y,self.DEEP_STOPPER_Y)
	self.moveMyCharge((x,y))

    def pSweeper(self):
	'''position sweeper'''
	x = self.SWEEPER_X
	y = self.SWEEPER_Y
	self.moveMyCharge((x,y))

    def pDefensiveMidfield(self):
	'''position defensive midfield'''
	x = self.brain.clip(self.brain.ball.x, self.CENTER_MID_LEFT_LIMIT, 
			    self.CENTER_MID_RIGHT_LIMIT)
	y = self.DEFENSIVE_CENTER_MIDFIELD_Y
	self.moveMyCharge((x,y))

    def pLeftDeepBack(self):
	'''position deep left back'''
	pass

    def pRightDeepBack(self):
	'''position deep right back'''
	pass

    def pKickoffSweeper(self):
	'''position kickoff sweeper'''
	if self.kickoff_formation == 0:
	    self.moveMyCharge(self.KICKOFF_DEFENDER_0)
	else:
	    self.moveMyCharge(self.KICKOFF_DEFENDER_1)

    def pKickoffStriker(self):
	'''position kickoff striker'''
	if self.kickoff_formation == 0:
	    self.moveMyCharge(self.KICKOFF_OFFENDER_0)
	else:
	    self.moveMyCharge(self.KICKOFF_OFFENDER_1)

    def pKickoffMiddie(self):
	'''position kickoff middie'''
	if self.kickoff_formation == 0:
	    self.moveMyCharge(self.KICKOFF_MIDDIE_0)
	else:
	    self.moveMyCharge(self.KICKOFF_MIDDIE_1)

    def pKickoffPlaySweeper(self):
	'''position kickoff sweeper'''
	self.moveMyCharge(self.KICKOFF_PLAY_DEFENDER)

    def pKickoffPlayStriker(self):
	'''position kickoff striker'''
	self.moveMyCharge(self.KICKOFF_PLAY_OFFENDER)

    def pKickoffPlayMiddie(self):
	'''position kickoff middie'''
	self.moveMyCharge(self.KICKOFF_PLAY_MIDDIE)

    # Crap shit subRoles for mock middie
    def pRightOMiddie(self):
	x = self.CENTER_MID_RIGHT_LIMIT + 30.
	y = self.OFFENSIVE_CENTER_MIDFIELD_Y + 50.
	self.moveMyCharge((x,y))

    def pLeftOMiddie(self):
	x = self.CENTER_MID_LEFT_LIMIT - 30.
	y = self.OFFENSIVE_CENTER_MIDFIELD_Y + 50.
	self.moveMyCharge((x,y))

    def pRightDMiddie(self):
	x = self.CENTER_MID_RIGHT_LIMIT + 30.
	y = self.DEFENSIVE_CENTER_MIDFIELD_Y - 50.
	self.moveMyCharge((x,y))

    def pLeftDMiddie(self):
	x = self.CENTER_MID_LEFT_LIMIT - 30.
	y = self.DEFENSIVE_CENTER_MIDFIELD_Y - 50.
	self.moveMyCharge((x,y))

    #### OPERATIONAL METHODS FOR BEHAVIOR TREE ####

    def performRole(self):
        """
        Method to simply perform the allocated role
        """
	# position allocation
	if self.role == self.CHASER:
	    self.rChaser()
	elif self.role == self.OFFENDER:
	    self.rOffender()
	elif self.role == self.DEFENDER:
	    self.rDefender()
	elif self.role == self.MIDDIE:
	    self.rMiddie()
	else:
	    print "PlayBook::fNormal(): role not assigned"


    ################################################################
    #                USEFUL BEHAVIOR FUNCTIONS                     #
    ################################################################


    def determineChaser(self):
	'''return the player number of the chaser'''

	chaser_mate = self.teammates[self.playerNumber-1]
	me = self.teammates[self.playerNumber-1]

	# center field capture hack for defender
	if (self.role == self.DEFENDER and
	    self.subRole == self.DEFENSIVE_MIDFIELD and
	    self.brain.getPosDist(self.position) < 125.0 and
	    0.0 < self.brain.ball.dist < 125.0 and
	    Constants.CENTER_FIELD_Y - 100.0 < self.brain.my.y < 
	    Constants.CENTER_FIELD_Y + 10.0 and
	    -90 < self.brain.my.h < 90):
	    self.timeSinceCaptureChase = self.brain.getTime()

	# center field capture hack for offender
	if (self.role == self.OFFENDER and
	    self.subRole == self.CENTER_O_MIDFIELD and
	    self.brain.getPosDist(self.position) < 100.0 and
	    0.0 < self.brain.ball.dist < 125.0 and
	    Constants.CENTER_FIELD_Y - 10.0 < self.brain.my.y < 
	    Constants.CENTER_FIELD_Y + 100.0 and
	    (self.brain.my.h < -90 or self.brain.my.h > 90)):
	    self.timeSinceCaptureChase = self.brain.getTime()

	# center field capture hack for middie
# 	if (self.role == self.MIDDIE and
# 	    self.subRole == self.CENTER_O_MIDFIELD and
# 	    self.brain.getPosDist(self.position) < 100.0 and
# 	    0.0 < self.brain.ball.dist < 125.0 and
# 	    Constants.CENTER_FIELD_Y - 10.0 < self.brain.my.y < 
# 	    Constants.CENTER_FIELD_Y + 100.0 and
# 	    (self.brain.my.h < -90 or self.brain.my.h > 90)):
# 	    self.timeSinceCaptureChase = self.brain.getTime()

	# keep chasing for center field hack
	if (self.brain.getTime() - self.timeSinceCaptureChase) < 2000.0:
	    return me

	if DEBUG_DET_CHASER:
	    print "chaser det: me == #", self.playerNumber
	# scroll through the teammates
        for i,mate in enumerate(self.teammates):
	    if DEBUG_DET_CHASER:
		print "\t mate #", i+1,
	    # TEAMMATE SANITY CHECKS
	    # don't check goalie out
	    # don't check out penalized or turned-off dogs
	    if (mate.playerNumber == self.GOALIE_NUMBER or  
		mate.inactive or
		mate.playerNumber == self.playerNumber or 
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

#             player_number_buffer = self.brain.clip(chaser_mate.chaseTime * 0.3,
# 						   1000.0,
# 						   10000.0)	

# 	    if ((fabs(mate.chaseTime-chaser_mate.chaseTime) < 
# 		 chaser_mate.chaseTime * 0.3 or
# 		 (fabs(mate.chaseTime-chaser_mate.chaseTime) < 250.))
# 		and mate.calledRole == self.CHASER):
# 		# then default to the higher player number
# 		if mate.playerNumber > chaser_mate.playerNumber:
# 		    if DEBUG_DET_CHASER:
# 			print ("\t chaser buff: #%d no. > #%d no." % 
# 			       (mate.playerNumber, chaser_mate.playerNumber))
# 		    chaser_mate = mate
# 		# else chaser_mate doesn't change
# 		else: 
# 		    if DEBUG_DET_CHASER:
# 			print ("\t chaser buff: #%d no. > #%d no." % 
# 			       (chaser_mate.playerNumber, mate.playerNumber))
# 		    pass

	    # Tie break stuff
	    if me.chaseTime < mate.chaseTime:
		chaseTimeScale = me.chaseTime
	    else:
		chaseTimeScale = mate.chaseTime

	    if ((me.chaseTime - mate.chaseTime < self.CALL_OFF_THRESH + 
		 .15 *chaseTimeScale or
		(me.chaseTime - mate.chaseTime < self.STOP_CALLING_THRESH +
		 .35 * chaseTimeScale and
		 self.lastRole == self.CHASER)) and
		mate.playerNumber < me.playerNumber):
		if DEBUG_DET_CHASER:
		    print ("\t #%d @ %g >= #%d @ %g" % 
			   (mate.playerNumber, mate.chaseTime, 
			    chaser_mate.playerNumber, 
			    chaser_mate.chaseTime))

		continue
	    elif (mate.playerNumber > me.playerNumber and
		  mate.chaseTime - me.chaseTime < self.LISTEN_THRESH + 
		  .45 * chaseTimeScale and
		  mate.calledRole == self.CHASER):
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
	'''returns estimated time to get to the ball on a chase'''
	
	time = 0.0
	# add chase forward time
	time += (mate.ballLocDist / 37.905) * 1000.0
	# add bearing correction time
	time += (fabs(mate.ballLocBearing) / 100.0) * 1000.0 

	# special KICKOFF_PLAY bonus
	if (self.formation == self.KICKOFF_PLAY and 
	    self.brain.game.getTimeSincePlay() > 2000.0 and
	    self.playerNumber == 4):
	    time -= 1000.0

	# velocity bonus
	if (self.VB_MIN_REL_VEL_Y < self.brain.ball.relVelY < 
	    self.VB_MAX_REL_VEL_Y and
	    0 < self.brain.ball.relY < self.VB_MAX_REL_Y and
	    fabs(self.brain.ball.relX + self.brain.ball.relVelX*
		 (self.brain.ball.relY / self.brain.ball.relVelY)) < 
	    self.VB_X_THRESH):
	    time -= self.VELOCITY_BONUS

	# ball bearing line to goal bonus
        if mate.y - self.brain.ball.y != 0:
	    xDiff = fabs( (mate.x - self.brain.ball.x) /
			  (mate.y - self.brain.ball.y) *
			  (self.GOAL_BACK_Y - mate.y) + mate.x -
			  self.GOAL_BACK_X)
	else:
	    xDiff = 0

	if 0. < xDiff < self.BEARING_SMOOTHNESS and mate.y < self.brain.ball.y:
	    time -= (-self.BEARING_BONUS/self.BEARING_SMOOTHNESS * 
		      xDiff + self.BEARING_BONUS)
	elif 0. < xDiff < self.BEARING_SMOOTHNESS:
	    time += (-self.BEARING_BONUS/self.BEARING_SMOOTHNESS * 
		      xDiff + self.BEARING_BONUS)

	# frames off bonus
	if self.brain.ball.framesOff < 3:
	    time -= self.BALL_NOT_SUPER_OFF_BONUS
		
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
	    mates.extend(self.teammates[self.playerNumber - 1])

	    # get distances from every dog every possible position
	    for i,dog in enumerate(dog_distances):
		dog_number = mates[i].playerNumber
		x, y = 0.0, 0.0
		# use either my estimate or teammates'
		if dog_number == self.playerNumber:
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
		if dog_number == self.playerNumber:
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
	    
    def shouldUseDubD(self):
	return ((self.brain.ball.x > Constants.MY_GOALBOX_LEFT_X + 5. and
		 self.brain.ball.x < Constants.MY_GOALBOX_RIGHT_X -5.  and
		 self.brain.ball.y < Constants.MY_GOALBOX_TOP_Y - 5.) or
		(self.brain.ball.x > Constants.MY_GOALBOX_LEFT_X -5. and
		 self.brain.ball.x < Constants.MY_GOALBOX_RIGHT_X + 5. and
		 self.brain.ball.y < Constants.MY_GOALBOX_TOP_Y + 5. and
		 self.teammates[0].calledRole == self.CHASER))

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
	     self.NEAR_LINE_THRESH) or 
	    # if ball is nearing right sideline
	    (self.brain.ball.x > Constants.FIELD_WHITE_RIGHT_SIDELINE_X -
	     self.NEAR_LINE_THRESH) or
	    # if ball is left of goalbox AND
	    (self.brain.ball.x < Constants.GOALBOX_LEFT_X and 
	     # ball is nearing top sideline OR
	     ((self.brain.ball.y > Constants.FIELD_WHITE_TOP_SIDELINE_Y - 
	       self.NEAR_LINE_THRESH) or 
	      # ball is nearing bottom sideline
	      (self.brain.ball.y < Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
	       self.NEAR_LINE_THRESH))) or
	    # if ball is right of goalbox AND
	    (self.brain.ball.x > Constants.GOALBOX_RIGHT_X and 
	     # ball is nearing top sideline OR
	     ((self.brain.ball.y > Constants.FIELD_WHITE_TOP_SIDELINE_Y - 
	       self.NEAR_LINE_THRESH) or 
	      # ball is nearing bottom sideline
	      (self.brain.ball.y < Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
	       self.NEAR_LINE_THRESH)))
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
	if pos_y > self.DEFENSIVE_MIDFIELD_Y:
	    pos_y = self.DEFENSIVE_MIDFIELD_Y
	    pos_x = (Constants.LANDMARK_MY_GOAL_X + delta_x / delta_y *
		     (self.DEFENSIVE_MIDFIELD_Y - Constants.LANDMARK_MY_GOAL_Y))

	return pos_x,pos_y

    def getNonChaserTeammates(self,chaser_mate):
	'''returns non-chaser teammate with you (assuming three dogs)'''
	nonChasers = []

	for elem in self.teammates:
	    if ( elem.playerNumber != self.GOALIE_NUMBER and 
		 elem != chaser_mate and 
		 elem.playerNumber != self.playerNumber):
		nonChasers.append(elem)

	return nonChasers

    def getOtherActiveTeammate(self):
	'''this returns the teammate instance of an active teammate that isn't 
	you. THIS ASSUMES THAT THERE IS ALREADY ONE FIELD PLAYER DEAD'''
	# Figure out who isn't penalized with you
	return [mate for mate in self.teammates if
		mate.playerNumber != self.GOALIE_NUMBER and
		mate.playerNumber != self.playerNumber and
		not mate.inactive][0]

    def getOtherActiveTeammates(self):
	'''
	figure out who the other active teammates are
	'''
	mates = []
	for mate in self.teammates:
	    if (not mate.inactive and mate.playerNumber != self.GOALIE_NUMBER
		and mate.playerNumber != self.playerNumber):
		mates.extend(mate)
	return mates

    def noCalledChaser(self):
	"""
	Returns true if no one is chasing and they are not searching
	"""
	# If everyone else is out, let's not go for the ball
	if self.getNumInactiveFieldPlayers() == self.NUM_TEAM_PLAYERS - 1.:
	    return False

	for mate in self.teammates:
	    if (not mate.inactive and (mate.calledRole == self.CHASER
				       or mate.calledRole == self.SEARCHER)):
		return False
	return True
    
    ##
    ## Potential Field Stuff
    ##
	
    def getAttractorPosition(self):
	'''returns position to move to (via pf) + x_eq, y_eq'''
	x_move, y_move, x_eq, y_eq = \
	    self.pf.movementVectorAt(self.brain.my.x,self.brain.my.y)	
	return (self.brain.my.x+x_move,self.brain.my.y+y_move,x_eq,y_eq)

    def updateMyCharge(self,position):
	'''updates my charge (adds if we don't have any, moves otherwise)'''
	if self.my_charge is None:
	    self.addMyCharge(position)
	else:
	    self.moveMyCharge(position)

    def addMyCharge(self,pos):
	'''adds an attractor point at (x,y) for me to move to'''
	self.my_charge = self.pf.addPointCharge(Constants.VERY_STRONG * 
						Constants.ATTRACTION,
						Constants.HALF_FIELD,
						pos[0],pos[1])
	self.my_charge_pos = pos

    def moveMyCharge(self,pos):
	'''moves my attractor point to (x,y) position'''
	if self.my_charge is None:
	    self.addMyCharge(pos)
	elif self.my_charge == pos:
	    return
	else:
	    self.pf.movePointCharge(self.my_charge,pos[0],pos[1])
	    self.my_charge_pos = pos  

    def removeMyCharge(self):
	'''removes own attractor point'''
	if self.my_charge is not None:
	    self.pf.removeCharge(self.my_charge)
	    self.my_charge = None
	    self.my_charge_pos = None

    def updateTeammateCharge(self,mate,charge_type):
	'''updates teammate's charge with proper charge type handling'''
	# mate has no charge, add charge
	if mate.charge is None:
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
	if type == self.NORMAL_CHARGE:	    
	    mate.charge = self.pf.addPointCharge(Constants.VERY_WEAK * 
						 Constants.REPULSION, 
						 Constants.VERY_SMALL_REGION, 
						 mate.x, 
						 mate.y)
	    mate.charge_type = type
        elif type == self.CHASER_CHARGE:
	    mate.charge = self.pf.addPointCharge(Constants.STRONG * 
						 Constants.REPULSION, 
						 Constants.SMALL_REGION, 
						 mate.x, 
						 mate.y)
	    mate.charge_type = type
	elif type == self.CHASER_TO_GOAL_CHARGE:
	    mate.charge = \
		self.pf.addSegmentCharge(Constants.VERY_STRONG * 
					 Constants.REPULSION,
					 Constants.LINE_SPREAD,
					 mate.x, mate.y,
					 Constants.LANDMARK_OPP_GOAL_X,
					 Constants.LANDMARK_OPP_GOAL_Y)
	    mate.charge_type = type
	else:
	    mate.charge = None
	    mate.charge_type = None
	    print "addTeammateRepuslor: unknown charge type"
			    
    def moveTeammateRepulsor(self,mate):
	'''moves teammate's repulsor to his new (x,y)'''
	if (mate.charge_type == self.NORMAL_CHARGE or
	    mate.charge_type == self.CHASER_CHARGE or
	    mate.charge_type == self.GOALIE_CHARGE):
	    self.pf.movePointCharge(mate.charge,
				    mate.x, 
				    mate.y)
	elif mate.charge_type == self.CHASER_TO_GOAL_CHARGE:
	    self.pf.moveSegmentCharge(mate.charge,
				      mate.x, mate.y,
				      Constants.LANDMARK_OPP_GOAL_X,
				      Constants.LANDMARK_OPP_GOAL_Y)
    
    def removeTeammateRepulsor(self,mate):
	'''removes teammate repulsor (if he gets penalized/dies)'''
	if mate.charge is not None:
	    self.pf.removeCharge(mate.charge)
	    mate.charge = None
	    mate.charge_type = None

    def getGoalDifferential(self):
	'''
	Returns the current goal differential
    	'''
	return (self.brain.game.myTeam.teamScore - 
		self.brain.game.theirTeam.teamScore)

    ######################################################
    ############       Teammate Stuff     ################
    ######################################################


    def updateTeammateStuff(self):
	'''
	this gets called every get_action(). updates teammate information
	necessary every frame (even when we don't receive packets from them).
	doesn't bother updating own packet
	'''
	# loop through teammates    
	for i,mate in enumerate(self.teammates):
	    # if teammate has died (haven't received a legit packet from them)
	    # if teammate is penalized, update that bool too
	    # never set this to false. getting a packet will do that
	    if (self.isTeammateDead(mate) or 
		self.isTeammatePenalized(mate)):
		mate.inactive = True

    def updateTeammateCharges(self):
	'''updates teammate field charges. bigger repulsor for chaser.'''

	for i,mate in enumerate(self.teammates):
	    # don't update self
	    if self.playerNumber == mate.playerNumber:
		continue

	    ## update potential field repulsor at teammate (x,y) ##
	    # if teammate is inactive but charge is still there
	    # (ie he just became inactive), remove repulsor
	    if (mate.inactive or 
		self.formation == self.KICKOFF or
		(self.role == self.DEFENDER and 
		 self.brain.my.y < Constants.FIELD_HEIGHT * 1./4.)):
		if mate.charge is not None:
		    #print "removing mate %d repulsor" % (i)
		    self.removeTeammateRepulsor(mate)	    
	    elif mate.role == self.CHASER and mate.y < self.brain.my.y:
		#if ((mate.grabbing or mate.dribbling or mate.kicking) and
		#    ):
		#self.updateTeammateCharge(mate,self.CHASER_TO_GOAL_CHARGE)
		#else:
		self.updateTeammateCharge(mate,self.CHASER_CHARGE)	    
	    else:
		self.updateTeammateCharge(mate,self.NORMAL_CHARGE)
		
    def isTeammatePenalized(self,teammate):
	'''
	this checks GameController to see if a player is penalized.
	this check is more redundant than anything, because our players stop
	sending packets when they are penalized, so they will most likely
	fall under the isTeammateDead() check anyways.
	'''
	if (self.brain.game.myTeam.players[teammate.playerNumber-1].penalty):
	    return True
	else:
	    return False

    def isTeammateDead(self,teammate):
	'''
	returns True if teammates' last timestamp is sufficiently behind ours.
	however, the dog could still be on but sending really laggy packets.
	'''
	return (teammate.timeStamp == 0 or
		teammate.lastPacketTime < (self.brain.getTime() -
					   self.PACKET_DEAD_PERIOD))

    def getNumInactiveFieldPlayers(self):
	'''cycles through teammate objects and returns number of teammates
	that are 'dead'. ignores myself'''
	num_inactive_teammates = 0
	for i,mate in enumerate(self.teammates): 	
	    if (mate.inactive and mate.playerNumber != self.playerNumber and 
		mate.playerNumber != self.GOALIE_NUMBER):
		num_inactive_teammates += 1
	return num_inactive_teammates

    def highestActivePlayerNumber(self):
	'''returns true if the player is the highest active player number'''
	activeMates = self.getOtherActiveTeammates()
	
	for mate in activeMates:
	    if mate.playerNumber > self.playerNumber:
		return False

	return True


    def getNumPenalizedOpponents(self):
	'''returns number of penalized robots on opponents team'''
	return self.brain.game.theirTeam.numPenalized

    def teammateHasBall(self):
	'''returns True if any mate has the ball'''
	for i,mate in enumerate(self.teammates):
	    if (mate.inactive or 
		mate.playerNumber == self.playerNumber):
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

    ######################################################
    ############ PUBLIC METHODS (sort of) ################
    ######################################################

    def update(self,packet):
	'''public method called by Brain.py to update a teammates' info
	with a new packet'''
	self.teammates[packet.playerNumber-1].update(packet)
 
    def reset(self):
	'''resets all information stored from teammates'''
        #print "playbook:: reset!"
	self.removeMyCharge()
	for i,mate in enumerate(self.teammates):
	    mate.reset()

    def getMyChargePos(self):
	'''returns attraction position of the pf'''
	if self.my_charge_pos is None:
	    return [self.brain.my.x, self.brain.my.y]
	else:
	    return self.my_charge_pos
    
class Teammate:
    '''class for keeping track of teammates' info'''
    def __init__(self,tbrain=None):
	'''variables include lots from the Packet class'''

	# things in the Packet()
	self.playerNumber = 0
	self.timeStamp = 0
	self.x = 0
	self.y = 0
	self.h = 0
	self.uncertX = 0
	self.uncertY = 0
	self.uncertH = 0
	self.ballX = 0
	self.ballY = 0
	self.ballUncertX = 0
	self.ballUncertY = 0
	self.ballDist = 0
	self.ballLocDist = 0
	self.ballLocBearing = 0
	self.calledRole = 0
	self.lastPacketTime = 0

	self.brain = tbrain	# brain instance
	self.role = 0 # known role
	self.inactive = True # dead basically just means inactive
	self.grabbing = False # dog is grabbing
	self.dribbling = False # dog is dribbling
	self.kicking = False # dog is kicking
	self.charge = None # pf charge
	self.charge_type = None
	self.charge_pos = None
	self.chaseTime = 0 # estimated time to chase the ball
	self.bearingToGoal = 0 # bearing to goal

    def update(self,packet):
	'''
	receives a packet, updates teammate information. packets received
	have already been verified by timestamp system, so we can assume
	they are LEGIT.
	'''
	
	# stores packet information locally
	self.timeStamp = packet.timeStamp
	self.x = packet.playerX
	self.y = packet.playerY
	self.h = packet.playerH
	self.uncertX = packet.uncertX
	self.uncertY = packet.uncertY
	self.uncertH = packet.uncertH
	self.ballX = packet.ballX
	self.ballY = packet.ballY
	self.ballUncertX = packet.ballUncertX
	self.ballUncertY = packet.ballUncertY
	self.ballDist = packet.ballDist
	self.calledSubRole = packet.calledSubRole
	if self.isChaser():
	    self.calledRole = PlayBook.CHASER
	elif self.isDefender():
	    self.calledRole = PlayBook.DEFENDER
	self.chaseTime = packet.chaseTime

	# calculates ball localization distance, bearing
	self.ballLocDist = self.getDistToBall()
	self.ballLocBearing = self.getBearingToBall()
	self.bearingToGoal = self.getBearingToGoal()

	# if a penalized timestamp, throw that up
	if packet.timeStamp == Constants.PENALIZED_TIMESTAMP:
	    self.inactive = True
	elif packet.timeStamp == Constants.SOS_TIMESTAMP:
	    print "DOG #%d DIED" % (self.playerNumber)
	    self.inactive = True
	else:
	    self.inactive = False

	self.grabbing = (packet.ballDist == 
			 Constants.BALL_TEAMMATE_DIST_GRABBING)
	self.dribbling = (packet.ballDist == 
			  Constants.BALL_TEAMMATE_DIST_DRIBBLING)
	self.kicking = False
	#(packet.ballDist == 
	#		Constants.BALL_TEAMMATE_DIST_KICKING)

	self.lastPacketTime = self.brain.getTime()

    def updateMe(self):
	'''updates my information as a teammate (since we don't get our own 
	packets)'''
	self.playerNumber = self.brain.my.playerNumber
	self.timeStamp = self.brain.time.timestamp()
	self.x = self.brain.my.x
	self.y = self.brain.my.y
	self.h = self.brain.my.h
	self.uncertX = self.brain.my.uncertX
	self.uncertY = self.brain.my.uncertY
	self.uncertH = self.brain.my.uncertH
	self.ballX = self.brain.ball.x
	self.ballY = self.brain.ball.y
	self.ballUncertX = self.brain.ball.uncertX
	self.ballUncertY = self.brain.ball.uncertY
	self.ballDist = self.brain.ball.dist
	self.ballLocDist = self.brain.ball.locDist
	self.ballLocBearing = self.brain.ball.locBearing
	self.inactive = False
	self.grabbing = (self.ballDist == 
			 Constants.BALL_TEAMMATE_DIST_GRABBING)
	self.dribbling = (self.ballDist == 
			  Constants.BALL_TEAMMATE_DIST_DRIBBLING)
	#self.kicking = (self.ballDist == 
	#		Constants.BALL_TEAMMATE_DIST_KICKING)

    def getBearingToGoal(self):
	'''returns bearing to goal'''
	return self.brain.getOthersRelativeBearing(self.x,
						   self.y,
						   self.h,
						   Constants.LANDMARK_OPP_GOAL_X,
						   Constants.LANDMARK_OPP_GOAL_Y)

    def getDistToBall(self):
	'''
	returns teammate distance to ball in centimeters.
	-based on its own localization but my own ball estimates
	'''
	return hypot(self.brain.ball.x - self.x,
		     self.brain.ball.y - self.y)
    
    def getBearingToBall(self):
	'''
	returns teammate bearing to the ball in degrees. 
	-based on its own localization but my own ball estimates
	-return values is between -180,180
	'''

	return self.brain.getOthersRelativeBearing(self.x,
						   self.y,
						   self.h,
						   self.brain.ball.x,
						   self.brain.ball.y)
    def reset(self):
	'''Reset all important Teammate variables'''
	#self.playerNumber = 0 # doesn't reset player number
	self.timeStamp = 0
	self.x = 0
	self.y = 0
	self.h = 0
	self.uncertX = 0
	self.uncertY = 0
	self.uncertH = 0
	self.ballX = 0
	self.ballY = 0
	self.ballUncertX = 0
	self.ballUncertY = 0
	self.ballDist = 0
	self.ballLocDist = 0
	self.ballLocBearing = 0
	self.grabbing = False
	self.kicking = False
	self.dribbling = False
	self.role = 0 # known role
	self.inactive = True # dead basically just means inactive
    
    def isChaser(self):
	return (self.calledSubRole == PlayBook.CHASE_NORMAL or 
	    self.calledSubRole == PlayBook.CHASE_AROUND_BOX)

    def isDefender(self):
	return (self.calledSubRole == PlayBook.STOPPER or
		self.calledSubRole == PlayBook.DEEP_STOPPER or
		self.calledSubRole == PlayBook.SWEEPER or
		self.calledSubRole == PlayBook.DEFENSIVE_MIDFIELD or
		self.calledSubRole == PlayBook.LEFT_DEEP_BACK or
		self.calledSubRole == PlayBook.RIGHT_DEEP_BACK)
    
	
