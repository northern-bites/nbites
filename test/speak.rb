
if ENV['AL_DIR'].nil?
  puts "ERROR : AL_DIR not defined!"
  exit 1
end

# include the Aldebaran libraries
require ENV['AL_DIR']+'/extern/ruby/aldebaran/allib'


# constants
$larm = [
  [0, 0, 0, 0],
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
  [0, 0, 0, 0],
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
$host = "139.140.77.210"
#if ! $*[0].nil?
#  $host = $*[0]
#end
puts "Connecting to " + $host
tts = Aldebaran::ALProxy.new("ALTextToSpeech", $host, 9559)

if tts.nil?
  exit 2
end


# free speech mode
puts "Ready for talking, enter text to speak, CTRL-D to quit:"
$line = gets
while $line
  tts.say($line)
  $line = gets
end


exit 0
