from socket import gethostname

from . import BirthCertificate

from man.motion import RobotGaits as gaits
from man.motion import SweetMoves as moves

def read_hostname():
	"""
	Method reads the hostname from the hostname file
	"""
	return gethostname()

# TODO: We really don't need all of this stuff for each robot!
#       Someone please figure out what we do need/delete the rest

zoe = BirthCertificate.BirthCertificate(
			name = 'zoe',
			long_name = 'Zoe Washburne',
			tts_name = 'zoe',
			doc = '''\
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

wash = BirthCertificate.BirthCertificate(
			name = "wash",
			long_name = "Hoban Washburne",
			tts_name = "wash",
			doc = '''\
I am Hoban Washburne, better known as Wash. I am the pilot of Serenity and
am married to Zoe. My collection of toy dinosaurs keeps me company as I
expertly maneuver the ship from planet to planet. I am a leaf on the wind;
watch how I soar...''',
			gait = gaits.SLARTI_GAIT,
			dribble_gait = gaits.SLARTI_DRIBBLE_GAIT,
			backwards_gait = gaits.SLARTI_BACKWARDS_GAIT,
			slow_gait = gaits.SLARTI_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
            back_standup = moves.STAND_UP_BACK,
			)

mal = BirthCertificate.BirthCertificate(
			name = 'mal',
			long_name = 'Malcolm Reynolds',
			tts_name = 'mal',
			doc = '''\
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

river = BirthCertificate.BirthCertificate(
			name = 'river',
			long_name = 'River Tam',
			tts_name = 'river',
			doc = '''\
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

jayne = BirthCertificate.BirthCertificate(
		   name = 'jayne',
		   long_name = 'Jayne Cobb',
		   tts_name = 'jayne',
		   doc = '''\
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

vera = BirthCertificate.BirthCertificate(
		   name = 'vera',
		   long_name = 'Vera',
		   tts_name = 'vera',
		   doc = '''\
I am Vera, Jayne's favorite gun in the entire universe. I'm a Callahan full-bore
auto-lock with a customized trigger and double cartridge thorough gauge. When
I get dressed up (in a space suit), I get taken out somewhere fun!''',
		   gait = gaits.SCOTTY_GAIT,
		   dribble_gait = gaits.SCOTTY_DRIBBLE_GAIT,
		   backwards_gait = gaits.SCOTTY_BACKWARDS_GAIT,
		   slow_gait = gaits.SCOTTY_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

kaylee = BirthCertificate.BirthCertificate(
		   name = 'kaylee',
		   long_name = 'Kaywinnet Lee Frye',
		   tts_name = 'kaylee',
		   doc = '''\
I am Kaylee Frye, Serenity's mechanic. I can fix just about anything because
machines just talk to me. Although you might not guess it from the fact that
I'm usually wearing work clothes and engine grease, I really like pretty
pink frilly dresses! And strawberries!''',
		   gait = gaits.DATA_GAIT,
		   dribble_gait = gaits.DATA_DRIBBLE_GAIT,
		   backwards_gait = gaits.DATA_BACKWARDS_GAIT,
		   slow_gait = gaits.DATA_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

simon = BirthCertificate.BirthCertificate(
		   name = 'simon',
		   long_name = 'Simon Tam',
		   tts_name = 'simon',
		   doc = '''\
I am Simon Tam, ship's doctor. I gave up a promising medical career back on
Osiris to save my sister, River, from the evil experiments at the Academy.
As a member of Serenity's crew, I can keep the two of us on the move and out
of the Alliance's evil grasp--and keep an eye on that pretty mechanic...''',
		   gait = gaits.DAX_GAIT,
		   dribble_gait = gaits.DAX_DRIBBLE_GAIT,
		   backwards_gait = gaits.DAX_BACKWARDS_GAIT,
		   slow_gait = gaits.DAX_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

inara = BirthCertificate.BirthCertificate(
		   name = 'inara',
		   long_name = 'Inara Serra',
		   tts_name = 'inara',
		   doc = '''\
I am Inara Serra, a registered Companion who rents one of Serenity's shuttles
for my work. I'm glamorous and surprisingly respectable given the rest of
the ship's crew, which makes me the ideal ambassador to open certain doors
that might otherwise be closed.''',
		   gait = gaits.ANNIKA_GAIT,
		   dribble_gait = gaits.ANNIKA_DRIBBLE_GAIT,
		   backwards_gait = gaits.ANNIKA_BACKWARDS_GAIT,
		   slow_gait = gaits.ANNIKA_SLOW_GAIT,
		   spin_gait = gaits.CUR_SPIN_GAIT,
		   )

robot_map = { mal.name	  : mal,
			  river.name  : river,
			  zoe.name    : zoe,
			  wash.name	  : wash,
			  jayne.name  : jayne,
			  vera.name   : vera,
			  kaylee.name : kaylee,
			  simon.name  : simon,
			  inara.name  : inara,
			  }

unknown = BirthCertificate.BirthCertificate(
			name = 'unknown',
			long_name = 'Unknown Robot',
			tts_name = 'John Doe',
			doc = '''\
This robot is currently unknown.  That is, we could not identify it
from its hostname (%s).	 The recognized robots are %s.''' %
			#(socket.gethostname(), robot_map.keys()),# temp removed -js
			('\'' + read_hostname() + '\'', robot_map.keys()),
			gait = gaits.DAX_GAIT,
			dribble_gait = gaits.ANNIKA_DRIBBLE_GAIT,
			backwards_gait = gaits.BACKWARDS_GAIT,
			slow_gait = gaits.DAX_SLOW_GAIT,
			spin_gait = gaits.CUR_SPIN_GAIT,
			)


def get_certificate():
	return robot_map.setdefault(read_hostname(),
								unknown) # switched to use the file read method
	#return robot_map.setdefault(socket.gethostname(), unknown)

