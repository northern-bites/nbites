
import socket
import os

import BirthCertificate

marvin = BirthCertificate.BirthCertificate(
            name='marvin',
            long_name='Marvin the Paranoid Android',
            tts_name='marvin',
            player_number=1,
            )

slarti = BirthCertificate.BirthCertificate(
            name="slarti",
            long_name="Slartibartfast",
            tts_name="slartibartfast",
            player_number=2,
            doc='''\
I am Slartibartfast of Magrathea.  I am best known for my award-winning
fjords on the coastline of Norway.  Join me in CamTim, the Campaign for
Real Time, and stop the robots of Krikkit from bringing together the
pieces of the Wikkit Gate!''',
            )

trillian = BirthCertificate.BirthCertificate(
            name='trillian',
            long_name='Tricia MacMillan',
            tts_name='trillian',
            player_number=3,
            )

zaphod = BirthCertificate.BirthCertificate(
            name='zaphod',
            long_name='Zaphod Beeblebrox',
            tts_name='zafoed beebel brocks',
            player_number=4,
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
    (socket.gethostname(), robot_map.keys()),
            )


def get_certificate():
    return robot_map.setdefault(socket.gethostname(), unknown)



