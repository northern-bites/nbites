
import socket
import struct

MAX_LEN = 1024

class Packet(object):
    header = ''
    ts = 0
    team = 0
    player = 0
    color = 0
    values = ()

    def __str__(self):
        return  ('<' + self.__class__.__module__ + "." +
                self.__class__.__name__ + ' object: \n' +
                '  header = ' + str(self.header) + '\n' +
                '  ts     = ' + str(self.ts    ) + '\n' +
                '  team   = ' + str(self.team  ) + '\n' +
                '  player = ' + str(self.player) + '\n' +
                '  color  = ' + str(self.color ) + '\n' +
                '  values = ' + ' '.join(map(str, self.values)))


def recv_packet(s):
    '''Read a packet from the network.  Error is raised on invalid packets
    (i.e. those not properly formed.  Does not check e.g. header value).'''
    # recieve a raw message
    msg, addr = s.recvfrom(MAX_LEN)
    # extract header data
    packet = Packet()
    packet.header = msg[:12]
    packet.ts, packet.team, packet.player, packet.color = \
        struct.unpack("<qiii", msg[12:32])
    # extract float values
    i = 32
    values = []
    while (i + 4) < len(msg):
        values.append(struct.unpack("<f", msg[i:i+4])[0])
        i += 4
    packet.values = tuple(values)
    return packet


def main(port=4000):

    # create and bind socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', port))
    # receive messages in loop
    while s:
        try:
            packet = recv_packet(s)
            print packet
        except IndexError:
            print "Bad packet received"
            s.close()
            s = None

if __name__ == '__main__':
    import sys
    main(*map(int, sys.argv[1:]))
