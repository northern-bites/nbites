class Leds():
	LED_OFF = 0
	LED_ON  = 1

	NUM_LED_GROUPS = 51

	"""
	Note that left LEDs will be on the robot's left side --
	they will be on the right if you're facing the robot head-on.
	"""
	(BRAIN_ZERO, #0
	BRAIN_ONE,	#1
	BRAIN_TWO,	#2
	BRAIN_THREE,	#3
	BRAIN_FOUR,	#4
	BRAIN_FIVE,	#5
	BRAIN_SIX,	#6
	BRAIN_SEVEN,	#7
	BRAIN_EIGHT,	#8
	BRAIN_NINE,	#9
	BRAIN_TEN,	#10
	BRAIN_ELEVEN, #11
	RIGHT_EYE_ZERO,	#12
	RIGHT_EYE_ONE,	#13
	RIGHT_EYE_TWO,	#14
	RIGHT_EYE_THREE,	#15
	RIGHT_EYE_FOUR,	#16
	RIGHT_EYE_FIVE,	#17
	RIGHT_EYE_SIX,	#18
	RIGHT_EYE_SEVEN,	#19
	LEFT_EYE_ZERO, #0
	LEFT_EYE_ONE,   #1
	LEFT_EYE_TWO,   #2
	LEFT_EYE_THREE, #3
	LEFT_EYE_FOUR,  #4
	LEFT_EYE_FIVE,	#5
	LEFT_EYE_SIX,	#6
	LEFT_EYE_SEVEN,
	RIGHT_EAR_ZERO,	#26
	RIGHT_EAR_ONE,	#27
	RIGHT_EAR_TWO,	#28
	RIGHT_EAR_THREE,	#29
	RIGHT_EAR_FOUR,	#30
	RIGHT_EAR_FIVE,	#31
	RIGHT_EAR_SIX,	#32
	RIGHT_EAR_SEVEN,	#33
	RIGHT_EAR_EIGHT,	#34
	RIGHT_EAR_NINE,	#35
	LEFT_EAR_ZERO,	#16
	LEFT_EAR_ONE,	#17
	LEFT_EAR_TWO,	#18
	LEFT_EAR_THREE,	#19
	LEFT_EAR_FOUR,	#20
	LEFT_EAR_FIVE,	#21
	LEFT_EAR_SIX,	#22
	LEFT_EAR_SEVEN,	#23
	LEFT_EAR_EIGHT,	#24
	LEFT_EAR_NINE,	#25
	CHEST,	#36
	LEFT_FOOT,	#37
	RIGHT_FOOT	#38
	) = range(NUM_LED_GROUPS)

	# (LEFT_CALIBRATION_ONE_LED, # left ear front
	# LEFT_CALIBRATION_TWO_LED,
	# LEFT_CALIBRATION_THREE_LED,
	# LEFT_CALIBRATION_FOUR_LED,
	# LEFT_CALIBRATION_FIVE_LED,
	# RIGHT_CALIBRATION_ONE_LED, # right ear front
	# RIGHT_CALIBRATION_TWO_LED,
	# RIGHT_CALIBRATION_THREE_LED,
	# RIGHT_CALIBRATION_FOUR_LED,
	# RIGHT_CALIBRATION_FIVE_LED,
	# LEFT_COMM_ONE_LED, # left ear back
	# LEFT_COMM_TWO_LED,
	# LEFT_COMM_THREE_LED,
	# LEFT_COMM_FOUR_LED,
	# LEFT_COMM_FIVE_LED,
	# RIGHT_COMM_ONE_LED, # right ear back
	# RIGHT_COMM_TWO_LED,
	# RIGHT_COMM_THREE_LED,
	# RIGHT_COMM_FOUR_LED,
	# RIGHT_COMM_FIVE_LED,
	# ROLE_LED, # full left eye
	# BALL_LED, # right eye 1-4
	# GOALBOX_LED, # one of the top right eyes
	# CHEST_LED, # the chest
	# LEFT_FOOT_LED,
	# RIGHT_FOOT_LED,
	# BRAIN_ZERO,
	# BRAIN_ONE,
	# BRAIN_TWO,
	# BRAIN_THREE,
	# BRAIN_FOUR,
	# BRAIN_FIVE,
	# BRAIN_SIX,
	# BRAIN_SEVEN,
	# BRAIN_EIGHT,
	# BRAIN_NINE,
	# BRAIN_TEN,
	# BRAIN_ELEVEN) = range(NUM_LED_GROUPS)

	"""
	LEDs persist throughout man restart so we need to reset them on
	startup
	"""
	initialLeds = [
		(CHEST, 0xFF00FF, 0.0),
		(RIGHT_EYE_ZERO, 0xFF0000, 0.0),
		(RIGHT_EYE_ONE, 0xffbf00, 0.0),
		(RIGHT_EYE_TWO, 0x80ff00, 0.0),
		(RIGHT_EYE_THREE, 0x00ff40, 0.0),
		(RIGHT_EYE_FOUR, 0x00FFFF, 0.0),
		(RIGHT_EYE_FIVE, 0x0040ff, 0.0),
		(RIGHT_EYE_SIX, 0x7f00ff, 0.0),
		(RIGHT_EYE_SEVEN, 0xff00bf, 0.0)
		# (LEFT_FOOT_LED, 0xFF0000, 0.0),
		# (RIGHT_FOOT_LED, 0xFF0000, 0.0),
		# (RIGHT_EYE_ZERO, 00000000, 0.0),
		# (RIGHT_EYE_ONE, 00000000, 0.0),
		# (RIGHT_EYE_TWO, 00000000, 0.0),
		# (RIGHT_EYE_THREE, 00000000, 0.0),
		# (RIGHT_EYE_FOUR, 00000000, 0.0),
		# (RIGHT_EYE_FIVE, 00000000, 0.0),
		# (RIGHT_EYE_SIX, 00000000, 0.0),
		# (RIGHT_EYE_SEVEN, 00000000, 0.0),
		# (BRAIN_ONE, 0xFFFFFF, 0.0),
		# (BRAIN_FIVE, 0xFFFFFF, 0.0),
		# (BRAIN_TEN, 0xFFFFFF, 0.0),
	]

	def __init__(self, brain):
		print("initialized leds obj")
		self.brain = brain
		# self.chestBlue = [(Leds.CHEST_LED, 0x000000, 0.0)]

	"""
	executeLeds takes in an array of tuples representing LED commands, and executes
	each LED command in the array immediately.
	"""
	def executeLeds(self, listOfLeds):
		print "--- New executeLeds command in Leds.py ---"
		for ledTuple in listOfLeds:
			if len(ledTuple) != 3:
				print "invalid led command " + str(ledTuple)
				continue
			# ledTuple[0] is an int
			# print ledTuple[0]
			self.brain.interface.ledCommand.add_led_id(ledTuple[0])
			self.brain.interface.ledCommand.add_rgb_hex(ledTuple[1])


	"""
	Sets the chest to a color, represented by a hexidecimal number (0xFFFFFF).
	"""
	def setChestLed(color):
		return

	"""
	Sets the entire right eye to a color.
	"""
	def setRightEyeFull(color):
		return

	"""
	Sets the entire left eye to a color.
	"""
	def setLeftEyeFull(color):
		return

	"""
	Sets a specific segment of the right eye to a color. Segments
	are numbered from 0 to 7, starting at the top and going around
	clockwise.
	"""
	def setRightEyeSegment(segment, color):
		return

	"""
	Sets a specific segment of the left eye to a color. Segments
	are numbered from 0 to 7, starting at the top and going around
	clockwise.
	"""
	def setLeftEyeSegment(segment, color):
		return

	"""
	Turns the entire right ear off, then turns on all the segments
	from 0 to the specified segment number, inclusive. Segments in 
	the ear are numbered from 0 to 9, with 0 at around 1:00, increasing 
	clockwise.
	"""
	def setRightEarSegmentsUpTo(self, segment):

		earLEDs = []

		for i in range(0, segment):
			earLEDs.append([self.RIGHT_EAR_ZERO + i, 0xFFFFFF, 0.0])

		self.executeLeds(earLEDs)
		return

	"""
	Turns the entire left ear off, then turns on all the segments
	from 0 to the specified segment number, inclusive. Segments in
	the ear are numbered from 0 to 9, with 0 at around 11:00, increasing
	counter-clockwise.
	"""
	def setLeftEarSegmentsUpTo(self, segment):

		earLEDs = []

		for i in range(0, segment):
			earLEDs.append([self.LEFT_EAR_ZERO + i, 0xFFFFFF, 0.0])

		self.executeLeds(earLEDs)
		return

	"""
	Turns the entire brain (the LEDs in the top of the head) off, then
	turns on all the segments from 0 to the specified segment number,
	inclusive. Segments in the brain are numbered from 0 to 11, with 
	0 at around 1:00, increasing clockwise if you're looking at the top
	of the head with the back of the robot facing you.
	"""
	def setBrainSegmentsUpTo(self, segment):
		return

	"""
	Turns on or off a specific segment in the brain.
	"""
	def setBrainSegment(self, segment, onOrOff):
		return

	"""
	Sets the left foot to a specific color.
	"""
	def setLeftFoot(self, color):
		self.executeLeds([[self.LEFT_FOOT, color, 0.0]])
		return

	"""
	Sets the right foot to a specific color.
	"""
	def setRightFoot(self, color):
		self.executeLeds([[self.RIGHT_FOOT, color, 0.0]])
		return