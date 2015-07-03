How to install the updated done script, kdone:

1) get kdone.sh to the robot
scp ./kdone.sh nao@<robot>:nbites/kdone.sh

2a) ssh into the robot
2b) get root
su

3) add alias to /etc/profile
nano /etc/profile

#Add the following line to the bottom of the file right next to the done alias:
alias kdone='source /home/nao/nbites/kdone.sh'


The What and the Why:
    The nao robot processes, naoqi-bin and hal, delay or even prevent shutting down the robot with
a simple 'sudo shutdown -h now'.  kdone.sh sends SIGTERM to those two processes, waits,
and if they're still alive sends SIGKILL (which cannot be caught/ignored).  Then it does 'sudo shutdown -h now'.


