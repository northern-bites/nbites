import os

from . import BirthCertificate

from man.motion import RobotGaits as gaits
from man.motion import SweetMoves as moves

def read_hostname():
	"""
	Method reads the hostname from the hostname file
	"""
	result = os.system('/bin/hostname > /tmp/hostname.txt')
	try:
		hostfile = open('/tmp/hostname.txt','r')
		host = hostfile.readline()
		host = host.rstrip("\n")
		return host
	except IOError:
		print ("Unable to read hostname from this platform")
		return "NONE"
#technically, we should set gaits according to the number stored in the chest
#board, however, reading this is not currently setup. -js

marvin = BirthCertificate.BirthCertificate(
			name='marvin',
			long_name='Marvin the Paranoid Android',
			tts_name='marvin',
			doc='''\
I am Marvin the Paranoid Android. Forced to suffer millenia in abjecet boredom,
Marvin was eventually kidnapped by the Krikkit robots who wished to use his
vast intelligence for evil. However, Marvin's depression infects his captors
and is thus responsible for saving the Universe.''',
			gait = gaits.MARVIN_GAIT,
			dribble_gait = gaits.MARVIN_DRIBBLE_GAIT,
			backwards_gait = gaits.MARVIN_BACKWARDS_GAIT,
			slow_gait = gaits.MARVIN_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

slarti = BirthCertificate.BirthCertificate(
			name="slarti",
			long_name="Slartibartfast",
			tts_name="slartibartfast",
			doc='''\
I am Slartibartfast of Magrathea.  I am best known for my award-winning
fjords on the coastline of Norway.	Join me in CamTim, the Campaign for
Real Time, and stop the robots of Krikkit from bringing together the
pieces of the Wikkit Gate!''',
			gait = gaits.SLARTI_GAIT,
			dribble_gait = gaits.SLARTI_DRIBBLE_GAIT,
			backwards_gait = gaits.SLARTI_BACKWARDS_GAIT,
			slow_gait = gaits.SLARTI_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

trillian = BirthCertificate.BirthCertificate(
			name='trillian',
			long_name='Tricia MacMillan',
			tts_name='trillian',
			doc='''\
I am Tricia MacMillian. After saving the Universe from the Krikkit
rebellion, I became a highly successful Sub Etha radio reporter.''',
			gait = gaits.TRILLIAN_GAIT,
			dribble_gait = gaits.TRILLIAN_DRIBBLE_GAIT,
			backwards_gait = gaits.TRILLIAN_BACKWARDS_GAIT,
			slow_gait = gaits.TRILLIAN_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

zaphod = BirthCertificate.BirthCertificate(
			name='zaphod',
			long_name='Zaphod Beeblebrox',
			tts_name='zafoed beebel brocks',
			doc='''\
I am Zaphod Bebblebrox. I invented the legendary Pan-Galatic
Garble Blaster, the strongest drink known to man. As President of the Galaxy, I
stole the Heart of Gold Spaceship, and helped rescue the Universe from the
Krikkit robots.''',
			gait = gaits.ZAPHOD_GAIT,
			dribble_gait = gaits.ZAPHOD_DRIBBLE_GAIT,
			backwards_gait = gaits.ZAPHOD_BACKWARDS_GAIT,
			slow_gait = gaits.ZAPHOD_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

spock = BirthCertificate.BirthCertificate(
		   name='spock',
		   long_name='Spock',
		   tts_name='spock',
		   doc='''\
I am Spock. My first name is unpronounceable by you humans. I am the son of a
Vulcan ambassador, Sarek, and a human, Amanda Grayson. I declined studying at the
Vulcan Science Academy and joined Starfleet instead. My actions really pissed off
my father. I provide an emotionally detached, logical perspective to the crew.''',
		   gait = gaits.SPOCK_GAIT,
		   dribble_gait = gaits.SPOCK_DRIBBLE_GAIT,
		   backwards_gait = gaits.SPOCK_BACKWARDS_GAIT,
		   slow_gait = gaits.SPOCK_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

scotty = BirthCertificate.BirthCertificate(
		   name='scotty',
		   long_name='Montgomery "Scotty" Scott',
		   tts_name='scotty',
		   doc='''\
I am Montgomery Scott. Call me Scotty. I am the Chief Engineer of the Enterprise
among other things. People call me a "miracle worker". My technical skill and
knowledge allow me to devise unconventional and effective last-minute solutions
to dire problems. When I crashed onto a Dyson Sphere, I rigged the ship's
transporter to keep me suspended in transit for 75 years.''',
		   gait = gaits.SCOTTY_GAIT,
		   dribble_gait = gaits.SCOTTY_DRIBBLE_GAIT,
		   backwards_gait = gaits.SCOTTY_BACKWARDS_GAIT,
		   slow_gait = gaits.SCOTTY_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

data = BirthCertificate.BirthCertificate(
		   name='data',
		   long_name='Lt. Commander Data',
		   tts_name='data',
		   doc='''\
I am Data, an android built by Dr. Noonien Soong. My positronic brain allows
me to possess greater strength, agility, and computation than most living
beings, but I would give it all up to be human. I was found by Starfleet on
Omicron Theta and joined Starfleet soon after. I was decorated several times
despite Dr. Maddox's attempt to prove me non-sentient.''',
		   gait = gaits.DATA_GAIT,
		   dribble_gait = gaits.DATA_DRIBBLE_GAIT,
		   backwards_gait = gaits.DATA_BACKWARDS_GAIT,
		   slow_gait = gaits.DATA_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

dax = BirthCertificate.BirthCertificate(
		   name='dax',
		   long_name='Jedzia Dax',
		   tts_name='dacks',
		   doc='''\
I am Jedzia Dax. I am a joined Trill living in symbiosis with the symbiont
Dax. As a result, I have the personality characteristics of both and have
access to the knowledge and memories of past hosts. I am the Chief Science
Officer on the Deep Space Nine space station.''',
		   gait = gaits.DAX_GAIT,
		   dribble_gait = gaits.DAX_DRIBBLE_GAIT,
		   backwards_gait = gaits.DAX_BACKWARDS_GAIT,
		   slow_gait = gaits.DAX_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

annika = BirthCertificate.BirthCertificate(
		   name='annika',
		   long_name='Annika Hansen',
		   tts_name='annika',
		   doc='''\
I am Annika Hansen. I was assimilated by the Borg at a young age and was
given the designation Seven of Nine, Tertiary Adjunct of Unimatrix Zero-One.
I was eventually rescued by the crew of the Voyager and disconnected from
the Collective. I chose to retain my Borg designation as it is the life I
know best, although the crew shortened it to Seven.''',
		   gait = gaits.ANNIKA_GAIT,
		   dribble_gait = gaits.ANNIKA_DRIBBLE_GAIT,
		   backwards_gait = gaits.ANNIKA_BACKWARDS_GAIT,
		   slow_gait = gaits.ANNIKA_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

robot_map = { marvin.name	: marvin,
			  slarti.name	: slarti,
			  trillian.name : trillian,
			  zaphod.name	: zaphod,
			  spock.name    : spock,
			  scotty.name   : scotty,
			  data.name     : data,
			  dax.name      : dax,
			  annika.name   : annika,
			  }

unknown = BirthCertificate.BirthCertificate(
			name='unknown',
			long_name='Unknown Robot',
			tts_name='John Doe',
			doc='''\
This robot is currently unknown.  That is, we could not identify it
from its hostname (%s).	 The recognized robots are %s.''' %
			#(socket.gethostname(), robot_map.keys()),# temp removed -js
			('\''+ read_hostname() +'\'', robot_map.keys()),
			gait = gaits.WEBOTS_GAIT,
			dribble_gait = gaits.WEBOTS_GAIT,
			backwards_gait = gaits.BACKWARDS_GAIT,
			slow_gait = gaits.WEBOTS_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
			)


def get_certificate():
	return robot_map.setdefault(read_hostname(),
								unknown) # switched to use the file read method
	#return robot_map.setdefault(socket.gethostname(), unknown)

