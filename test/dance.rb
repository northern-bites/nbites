

if ENV['AL_DIR'].nil?
  puts "ERROR : AL_DIR not defined!"
  exit 1
end

# include the Aldebaran libraries
require ENV['AL_DIR']+'/extern/ruby/aldebaran/allib'


# constants
$larm = [
  [0, 0, 0, 0],
  [-20 * MOTION::TO_RAD, 
     0 * MOTION::TO_RAD,
     0 * MOTION::TO_RAD, 
     0 * MOTION::TO_RAD
  ],
  [-40 * MOTION::TO_RAD, 
    30 * MOTION::TO_RAD,
   -70 * MOTION::TO_RAD, 
   -90 * MOTION::TO_RAD
  ]
]

$rarm = [
  [0, 0, 0, 0],
  [-20 * MOTION::TO_RAD, 
     0 * MOTION::TO_RAD,
     0 * MOTION::TO_RAD, 
     0 * MOTION::TO_RAD
  ],
  [-40 * MOTION::TO_RAD, 
   -30 * MOTION::TO_RAD,
    70 * MOTION::TO_RAD, 
    90 * MOTION::TO_RAD
  ]
]

$head = [
  [ 0,
    45 * MOTION::TO_RAD
  ],
  [ 0,
   -45 * MOTION::TO_RAD
  ],
  [ 0, 0 ]
]

$step = 0.5


# create proxies to the robot
$host = "139.140.77.210"
#if ! $*[0].nil?
#  $host = $*[0]
#end
puts "Connecting to " + $host
motion = Aldebaran::ALProxy.new("ALMotion", $host, 9559)
puts motion
tts = Aldebaran::ALProxy.new("ALTextToSpeech", $host, 9559)

if motion.nil? or tts.nil?
  exit 2
end

motion.setBodyStiffness(1, 1)


# init state
puts "Moving into initial sleep position"
motion.gotoChainAngles("LArm", $larm[0], 3, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("RArm", $rarm[0], 3, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("Head", $head[0], 3, MOTION::INTERPOLATION_SMOOTH)

puts "Robot ready, press enter to wake up"
sleep(2)
gets


# dance
tts.say("Get ready")
sleep(1)
tts.say("Dance")
motion.gotoChainAngles("LArm", $larm[1], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[0], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[1], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[0], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[1], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[0], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[1], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[0], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[1], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[0], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[1], $step, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("LArm", $larm[0], $step, MOTION::INTERPOLATION_SMOOTH)

motion.gotoChainAngles("RArm", $rarm[1], $step, MOTION::INTERPOLATION_SMOOTH)

