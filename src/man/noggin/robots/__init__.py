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
			name='zoe',
			long_name='Zoe Washburne',
			tts_name='zoe',
			doc='''\
I am Zoe Washburne. I served with Mal during the Unification War and stuck
with him when he bought Serenity. As second-in-command, I back Mal up during
our jobs and can stay completely cool when we're in hot water. Although I may
seem more tough than lovable, I'm married to Wash.''',
			gait = gaits.MARVIN_GAIT,
			dribble_gait = gaits.MARVIN_DRIBBLE_GAIT,
			backwards_gait = gaits.MARVIN_BACKWARDS_GAIT,
			slow_gait = gaits.MARVIN_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

slarti = BirthCertificate.BirthCertificate(
			name="wash",
			long_name="Hoban Washburne",
			tts_name="wash",
			doc='''\
I am Hoban Washburne, better known as Wash. I am the pilot of Serenity and
married to Zoe. My collection of toy dinosaurs keep me company as I expertly
maneuver the ship from planet to planet. I am a leaf on the wind; watch how
I soar...''',
			gait = gaits.SLARTI_GAIT,
			dribble_gait = gaits.SLARTI_DRIBBLE_GAIT,
			backwards_gait = gaits.SLARTI_BACKWARDS_GAIT,
			slow_gait = gaits.SLARTI_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

trillian = BirthCertificate.BirthCertificate(
			name='mal',
			long_name='Malcolm Reynolds',
			tts_name='mal',
			doc='''\
I am Malcolm Reynolds, captain of the Firefy-class spaceship Serenity. I fought
against the Alliance in the Unification war, and now I stick to the outskirts
of civilization and take whatever work comes my way, legal or no. As long as
Serenity can keep on flying, it's enough for me.''',
			gait = gaits.TRILLIAN_GAIT,
			dribble_gait = gaits.TRILLIAN_DRIBBLE_GAIT,
			backwards_gait = gaits.TRILLIAN_BACKWARDS_GAIT,
			slow_gait = gaits.TRILLIAN_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

zaphod = BirthCertificate.BirthCertificate(
			name='river',
			long_name='River Tam',
			tts_name='river',
			doc='''\
I am River Tam. My brother Simon and I are on the run from the Alliance because
he saved me from a cruel experimental program where they removed part of my brain.
Although sometimes mentally unstable, I also have incredible abilities. No
power in the 'verse can stop me.''',
			gait = gaits.ZAPHOD_GAIT,
			dribble_gait = gaits.ZAPHOD_DRIBBLE_GAIT,
			backwards_gait = gaits.ZAPHOD_BACKWARDS_GAIT,
			slow_gait = gaits.ZAPHOD_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

spock = BirthCertificate.BirthCertificate(
		   name='jayne',
		   long_name='Jayne Cobb',
		   tts_name='jayne',
		   doc='''\
I am Jayne Cobb. I'm a total mercenary, so I turned on my old employer and joined
the crew of Serenity when Mal offered me my own bunk on the ship. Despite my
selfish personality, I somehow became a hero, statue and all, to a poor town
where I was forced to drop some money I stole from their rich magistrate.''',
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

