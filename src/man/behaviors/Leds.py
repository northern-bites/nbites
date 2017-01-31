class Leds():
	LED_OFF = 0
	LED_ON  = 1

	NUM_LED_GROUPS = 26

	(LEFT_CALIBRATION_ONE_LED, # left ear front
	LEFT_CALIBRATION_TWO_LED,
	LEFT_CALIBRATION_THREE_LED,
	LEFT_CALIBRATION_FOUR_LED,
	LEFT_CALIBRATION_FIVE_LED,
	RIGHT_CALIBRATION_ONE_LED, # right ear front
	RIGHT_CALIBRATION_TWO_LED,
	RIGHT_CALIBRATION_THREE_LED,
	RIGHT_CALIBRATION_FOUR_LED,
	RIGHT_CALIBRATION_FIVE_LED,
	LEFT_COMM_ONE_LED, # left ear back
	LEFT_COMM_TWO_LED,
	LEFT_COMM_THREE_LED,
	LEFT_COMM_FOUR_LED,
	LEFT_COMM_FIVE_LED,
	RIGHT_COMM_ONE_LED, # right ear back
	RIGHT_COMM_TWO_LED,
	RIGHT_COMM_THREE_LED,
	RIGHT_COMM_FOUR_LED,
	RIGHT_COMM_FIVE_LED,
	ROLE_LED, # full left eye
	BALL_LED, # right eye 1-4
	GOALBOX_LED, # one of the top right eyes
	CHEST_LED, # the chest
	LEFT_FOOT_LED,
	RIGHT_FOOT_LED) = range(NUM_LED_GROUPS)

	"""
	LEDs persist throughout man restart so we need to reset them on
	startup
	"""
	initialLeds = [
		(GOALBOX_LED, 0xFF00FF, 0.0),
		(CHEST_LED, 0xFF00FF, 0.0)
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
		for ledTuple in listOfLeds:
			if len(ledTuple) != 3:
				print "invalid led command " + str(ledTuple)
				continue
			# ledTuple[0] is an int
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
	def setRightEarSegmentsUpTo(segment):
		return

	"""
	Turns the entire left ear off, then turns on all the segments
	from 0 to the specified segment number, inclusive. Segments in
	the ear are numbered from 0 to 9, with 0 at around 11:00, increasing
	counter-clockwise.
	"""
	def setLeftEarSegmentsUpTo(segment):
		return

	"""
	Turns the entire brain (the LEDs in the top of the head) off, then
	turns on all the segments from 0 to the specified segment number,
	inclusive. Segments in the brain are numbered from 0 to 11, with 
	0 at around 1:00, increasing clockwise if you're looking at the top
	of the head with the back of the robot facing you.
	"""
	def setBrainSegmentsUpTo(segment):
		return

	"""
	Turns on or off a specific segment in the brain.
	"""
	def setBrainSegment(segment, onOrOff):
		return

	"""
	Sets the left foot to a specific color.
	"""
	def setLeftFoot(color):
		return

	"""
	Sets the right foot to a specific color.
	"""
	def setRightFoot(color):
		return