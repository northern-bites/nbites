=begin

@author Jerome Monceaux
Aldebaran Robotics (c) 2007 All Rights Reserved. This is an example of use.

=end

puts '        START TESTING MyModule MODULE'

#Checking AL_DIR
if ENV['AL_DIR'].nil?
  puts "ERROR : AL_DIR not defined!"
  exit 1
end

#________________________________
# Load the aldebaran Library
#________________________________
require ENV['AL_DIR']+'/extern/ruby/aldebaran/allib'

#________________________________
# Proxy to myModule
#________________________________
myModule = Aldebaran::ALProxy.new("MyModule", "127.0.0.1", 9559)


myModule.helloWorld

