import os

from . import BirthCertificate

from man.motion import RobotGaits as gaits

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
            turn_gait = gaits.MARVIN_TURN_GAIT,
            )

slarti = BirthCertificate.BirthCertificate(
            name="slarti",
            long_name="Slartibartfast",
            tts_name="slartibartfast",
            doc='''\
I am Slartibartfast of Magrathea.  I am best known for my award-winning
fjords on the coastline of Norway.  Join me in CamTim, the Campaign for
Real Time, and stop the robots of Krikkit from bringing together the
pieces of the Wikkit Gate!''',
            gait = gaits.SLARTI_GAIT,
            turn_gait = gaits.SLARTI_TURN_GAIT,
            )

trillian = BirthCertificate.BirthCertificate(
            name='trillian',
            long_name='Tricia MacMillan',
            tts_name='trillian',
            doc='''\
I am Tricia MacMillian. After saving the Universe from the Krikkit
rebellion, I became a highly successful Sub Etha radio reporter.''',
            gait = gaits.TRILLIAN_GAIT,
            turn_gait = gaits.TRILLIAN_TURN_GAIT,
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
            turn_gait = gaits.ZAPHOD_TURN_GAIT,

            )


robot_map = { marvin.name   : marvin,
              slarti.name   : slarti,
              trillian.name : trillian,
              zaphod.name   : zaphod,
              }

unknown = BirthCertificate.BirthCertificate(
            name='unknown',
            long_name='Unknown Robot',
            tts_name='John Doe',
            doc='''\
This robot is currently unknown.  That is, we could not identify it
from its hostname (%s).  The recognized robots are %s.''' %
            #(socket.gethostname(), robot_map.keys()),# temp removed -js
            ('\''+open("/etc/hostname",'r').readline().rstrip()+'\'', robot_map.keys()), 
            gait = gaits.WEBOTS_GAIT,
            turn_gait = gaits.WEBOTS_GAIT,
            )


def get_certificate():
    return robot_map.setdefault(read_hostname(),
                                unknown) # switched to use the file read method
    #return robot_map.setdefault(socket.gethostname(), unknown)

def read_hostname():
    """
    Method reads the hostname from the hostname file
    """
    return open("/etc/hostname",'r').readline().rstrip()


