
import noggin
import noggin.Brain
import noggin.robots

def test_Brain():
   b = noggin.Brain.Brain()

def test_robots():
   cert = noggin.robots.get_certificate()
   assert cert is noggin.robots.unknown


if __name__ == '__main__':
    test_Brain()
    test_robots()
