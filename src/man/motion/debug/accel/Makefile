
LDLIBS =
#GLDLIBS = -framework AGL -framework OpenGL -framework GLUT -framework Foundation
LDFLAGS  = $(LDLIBS) $(GLDLIBS) -lm
MAN_DIR = ../../..
INCLUDE = -I$(MAN_DIR)/include -I$(MAN_DIR)/motion -I$(MAN_DIR)/noggin -I$(MAN_DIR)/corpus -I$(MAN_DIR)/vision

CC = g++ -g -Wall -Wno-unused

PROGS = simple1
OBJS = test.o AccEKF.o ZmpEKF.o ZmpAccEKF.o

#LOG_FILE = ../../../../Desktop/accels.csv
LOG_FILE = /tmp/com_log.xls

default: accel

accel: $(OBJS)
	$(CC) -o $@ $(OBJS)   $(LDFLAGS)

run: accel
	./accel $(LOG_FILE) && R --no-save < zmp_log.R

clean::
	rm -f $(OBJS)
	rm -f accel
	rm -f accel_log-*.pdf


%.o:: %.cpp
	$(CC) $(INCLUDE) -c  $< -o $@
