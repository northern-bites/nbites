class Leds():
	LED_OFF = 0
	LED_ON  = 1

	NUM_LED_GROUPS = 26

	(LEFT_CALIBRATION_ONE_LED,
	LEFT_CALIBRATION_TWO_LED,
	LEFT_CALIBRATION_THREE_LED,
	LEFT_CALIBRATION_FOUR_LED,
	LEFT_CALIBRATION_FIVE_LED,
	RIGHT_CALIBRATION_ONE_LED,
	RIGHT_CALIBRATION_TWO_LED,
	RIGHT_CALIBRATION_THREE_LED,
	RIGHT_CALIBRATION_FOUR_LED,
	RIGHT_CALIBRATION_FIVE_LED,
	LEFT_COMM_ONE_LED,
	LEFT_COMM_TWO_LED,
	LEFT_COMM_THREE_LED,
	LEFT_COMM_FOUR_LED,
	LEFT_COMM_FIVE_LED,
	RIGHT_COMM_ONE_LED,
	RIGHT_COMM_TWO_LED,
	RIGHT_COMM_THREE_LED,
	RIGHT_COMM_FOUR_LED,
	RIGHT_COMM_FIVE_LED,
	ROLE_LED,
	BALL_LED,
	GOALBOX_LED,
	CHEST_LED,
	LEFT_FOOT_LED,
	RIGHT_FOOT_LED) = range(NUM_LED_GROUPS)

	initialLeds = [
		(BALL_LED, 0xFF00FF, 0.0)
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
			self.brain.interface.ledCommand.add_led_id(ledTuple[0])
			self.brain.interface.ledCommand.add_rgb_hex(ledTuple[1])


	"""
	Sets the chest to a color, represented by a hexidecimal number (0xFFFFFF).
	"""
	def setChestLed(color):
		#

	"""
	Sets the entire right eye to a color.
	"""
	def setRightEyeFull(color):
		#

	"""
	Sets the entire left eye to a color.
	"""
	def setLeftEyeFull(color):
		#

	"""
	Sets a specific segment of the right eye to a color. Segments
	are numbered from 0 to 7, starting at the top and going around
	clockwise.
	"""
	def setRightEyeSegment(segment, color):
		#

	"""
	Sets a specific segment of the left eye to a color. Segments
	are numbered from 0 to 7, starting at the top and going around
	clockwise.
	"""
	def setLeftEyeSegment(segment, color):
		#

	"""
	Turns the entire right ear off, then turns on all the segments
	from 0 to the specified segment number, inclusive. Segments in 
	the ear are numbered from 0 to 9, with 0 at around 1:00, increasing 
	clockwise.
	"""
	def setRightEarSegmentsUpTo(segment):
		#

	"""
	Turns the entire left ear off, then turns on all the segments
	from 0 to the specified segment number, inclusive. Segments in
	the ear are numbered from 0 to 9, with 0 at around 11:00, increasing
	counter-clockwise.
	"""
	def setLeftEarSegmentsUpTo(segment):
		#

	"""
	Turns the entire brain (the LEDs in the top of the head) off, then
	turns on all the segments from 0 to the specified segment number,
	inclusive. Segments in the brain are numbered from 0 to 11, with 
	0 at around 1:00, increasing clockwise if you're looking at the top
	of the head with the back of the robot facing you.
	"""
	def setBrainSegmentsUpTo(segment):
		#

	"""
	Turns on or off a specific segment in the brain.
	"""
	def setBrainSegment(segment, onOrOff):
		#

	"""
	Sets the left foot to a specific color.
	"""
	def setLeftFoot(color):
		#

	"""
	Sets the right foot to a specific color.
	"""
	def setRightFoot(color):
		#