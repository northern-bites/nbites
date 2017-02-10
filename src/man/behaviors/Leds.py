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

	"""
	LEDs persist throughout man restart so we need to reset them on
	startup
	"""
	initialLeds = [
		(CHEST, 0xFFFFFF, 0.0),
		(RIGHT_EYE_ZERO, 0xFF0000, 0.0),
		(RIGHT_EYE_ONE, 0xffbf00, 0.0),
		(RIGHT_EYE_TWO, 0x80ff00, 0.0),
		(RIGHT_EYE_THREE, 0x00ff40, 0.0),
		(RIGHT_EYE_FOUR, 0x00FFFF, 0.0),
		(RIGHT_EYE_FIVE, 0x0040ff, 0.0),
		(RIGHT_EYE_SIX, 0x7f00ff, 0.0),
		(RIGHT_EYE_SEVEN, 0xff00bf, 0.0),
		(RIGHT_EYE_ZERO, 0xFF0000, 0.0),
		(LEFT_EYE_ONE, 0xffbf00, 0.0),
		(LEFT_EYE_TWO, 0x80ff00, 0.0),
		(LEFT_EYE_THREE, 0x00ff40, 0.0),
		(LEFT_EYE_FOUR, 0x00FFFF, 0.0),
		(LEFT_EYE_FIVE, 0x0040ff, 0.0),
		(LEFT_EYE_SIX, 0x7f00ff, 0.0),
		(LEFT_EYE_SEVEN, 0xff00bf, 0.0),
		(RIGHT_EAR_ZERO, 0x000000, 0.0),
		(RIGHT_EAR_ONE, 0x000000, 0.0),
		(RIGHT_EAR_TWO, 0x000000, 0.0),
		(RIGHT_EAR_THREE, 0x000000, 0.0),
		(RIGHT_EAR_FOUR, 0x000000, 0.0),
		(RIGHT_EAR_FIVE, 0x000000, 0.0),
		(RIGHT_EAR_SIX, 0x000000, 0.0),
		(RIGHT_EAR_SEVEN, 0x000000, 0.0),
		(RIGHT_EAR_EIGHT, 0x000000, 0.0),
		(RIGHT_EAR_NINE, 0x000000, 0.0),
		(LEFT_EAR_ZERO, 0x000000, 0.0),
		(LEFT_EAR_ONE, 0x000000, 0.0),
		(LEFT_EAR_TWO, 0x000000, 0.0),
		(LEFT_EAR_THREE, 0x000000, 0.0),
		(LEFT_EAR_FOUR, 0x000000, 0.0),
		(LEFT_EAR_FIVE, 0x000000, 0.0),
		(LEFT_EAR_SIX, 0x000000, 0.0),
		(LEFT_EAR_SEVEN, 0x000000, 0.0),
		(LEFT_EAR_EIGHT, 0x000000, 0.0),
		(LEFT_EAR_NINE, 0x000000, 0.0),
		]

	def __init__(self, brain):
		print("initialized leds obj")
		self.brain = brain

	"""
	executeLeds takes in an array of tuples representing LED commands, and executes
	each LED command in the array immediately.
	"""
	def executeLeds(self, listOfLeds):
		for ledTuple in listOfLeds:
			if len(ledTuple) != 3:
				print "invalid led command " + str(ledTuple)
				continue

			self.brain.interface.ledCommand.add_led_id(ledTuple[0])
			self.brain.interface.ledCommand.add_rgb_hex(ledTuple[1])


	"""
	Sets the chest to a color, represented by a hexadecimal number (0xFFFFFF).
	"""
	def setChestLed(self, color):
		self.executeLeds([(self.CHEST, color, 0.0)])
		return

	"""
	Sets the entire right eye to a color.
	"""
	def setRightEyeFull(self, color):
		list = [
			(self.RIGHT_EYE_ZERO, color, 0.0),
			(self.RIGHT_EYE_ONE, color, 0.0),
			(self.RIGHT_EYE_TWO, color, 0.0),
			(self.RIGHT_EYE_THREE, color, 0.0),
			(self.RIGHT_EYE_FOUR, color, 0.0),
			(self.RIGHT_EYE_FIVE, color, 0.0),
			(self.RIGHT_EYE_SIX, color, 0.0),
			(self.RIGHT_EYE_SEVEN, color, 0.0),
		]
		
		self.executeLeds(list)
		return

	"""
	Sets the entire left eye to a color.
	"""
	def setLeftEyeFull(self, color):
		list = [
			(self.LEFT_EYE_ZERO, color, 0.0),
			(self.LEFT_EYE_ONE, color, 0.0),
			(self.LEFT_EYE_TWO, color, 0.0),
			(self.LEFT_EYE_THREE, color, 0.0),
			(self.LEFT_EYE_FOUR, color, 0.0),
			(self.LEFT_EYE_FIVE, color, 0.0),
			(self.LEFT_EYE_SIX, color, 0.0),
			(self.LEFT_EYE_SEVEN, color, 0.0),
		]

		self.executeLeds(list)
		return

	"""
	Sets a specific segment of the right eye to a color. Segments
	are numbered from 0 to 7, starting at the top and going around
	clockwise.
	"""
	def setRightEyeSegment(self, segment, color):
		if (segment < 0 or segment > 7):
			return

		self.executeLeds([(self.RIGHT_EYE_ZERO + segment, color, 0.0)])
		return

	"""
	Sets a specific segment of the left eye to a color. Segments
	are numbered from 0 to 7, starting at the top and going around
	clockwise.
	"""
	def setLeftEyeSegment(self, segment, color):
		if (segment < 0 or segment > 7):
			return 

		self.executeLeds([(self.LEFT_EYE_ZERO + segment, color, 0.0)])
		return

	"""
	Turns the entire right ear off, then turns on all the segments
	from 0 to the specified segment number, inclusive. Segments in 
	the ear are numbered from 0 to 9, with 0 at around 1:00, increasing 
	clockwise.
	"""
	def setRightEarSegmentsUpTo(self, segment):

		if (segment < 0 or segment > 9):
			return

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

		if (segment < 0 or segment > 9):
			return 

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