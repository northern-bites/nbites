class Leds(object):
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

	def __init__(self, brain):
		print "initialized leds obj"
		self.brain = brain
		self.chestBlue = ((CHEST_LED, 0xFF00FF, 0.0),)

	def executeLeds(self, listOfLeds):
		for ledTuple in listOfLeds:
			if len(ledTuple) != 3:
				print "invalid led command " + str(ledTuple)
				continue
			self.brain.interface.ledCommand.add_led_id(ledTuple[0])
			self.brain.interface.ledCommand.add_rgb_hex(ledTuple[1])