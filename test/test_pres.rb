
if ENV['AL_DIR'].nil?
  puts "ERROR : AL_DIR not defined!"
  exit 1
end

# include the Aldebaran libraries
require ENV['AL_DIR']+'/extern/ruby/aldebaran/allib'


# constants
$larm = [
  [120, 0, 0, 0],
  [-40 * MOTION::TO_RAD, 
    0,
   -70 * MOTION::TO_RAD, 
   -90 * MOTION::TO_RAD
  ],
  [-40 * MOTION::TO_RAD, 
    30 * MOTION::TO_RAD,
   -70 * MOTION::TO_RAD, 
   -90 * MOTION::TO_RAD
  ]
]

$rarm = [
  [120, 0, 0, 0],
  [-40 * MOTION::TO_RAD, 
    0,
    70 * MOTION::TO_RAD, 
    90 * MOTION::TO_RAD
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



# create proxies to the robot
$host = "139.140.77.208"
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


# wake up
motion.postGotoChainAngles("Head", $head[1], 3, MOTION::INTERPOLATION_SMOOTH)
motion.postGotoChainAngles("LArm", $larm[1], 3, MOTION::INTERPOLATION_SMOOTH)
sleep(0.5)
motion.gotoChainAngles("RArm", $rarm[1], 3, MOTION::INTERPOLATION_SMOOTH)
sleep(0.5)


# rub eyes
# back
motion.postGotoAngle("LElbowYaw", -55*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
motion.gotoAngle("RElbowYaw",  55*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
# forth
motion.postGotoAngle("LElbowYaw", -70*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
motion.gotoAngle("RElbowYaw",  70*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
# back
motion.postGotoAngle("LElbowYaw", -55*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
motion.gotoAngle("RElbowYaw",  55*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
# forth
motion.postGotoAngle("LElbowYaw", -70*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
motion.gotoAngle("RElbowYaw",  70*MOTION::TO_RAD, 0.5, MOTION::INTERPOLATION_SMOOTH)
sleep(1)


# notice the crowd
motion.postGotoChainAngles("LArm", $larm[2], 1, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("RArm", $rarm[2], 1, MOTION::INTERPOLATION_SMOOTH)
sleep(0.3)
motion.gotoChainAngles("Head", $head[2], 1, MOTION::INTERPOLATION_SMOOTH)
sleep(0.5)
motion.gotoAngle("HeadYaw", -30 * MOTION::TO_RAD, 1.5, MOTION::INTERPOLATION_SMOOTH)
sleep(0.5)
motion.gotoAngle("HeadYaw",  30 * MOTION::TO_RAD, 1.5, MOTION::INTERPOLATION_SMOOTH)
sleep(0.5)
motion.postGotoChainAngles("Head", $head[2], 1, MOTION::INTERPOLATION_SMOOTH)
motion.postGotoChainAngles("LArm", $larm[0], 1, MOTION::INTERPOLATION_SMOOTH)
motion.gotoChainAngles("RArm", $rarm[0], 1, MOTION::INTERPOLATION_SMOOTH)


# say hello
tts.say("Well, hello everyone.  My name is Mister Zander.  How do you do?")


# free speech mode
puts "Ready for talking, enter text to speak, CTRL-D to quit:"
$line = gets
while $line
  tts.say($line)
  $line = gets
end


exit 0
