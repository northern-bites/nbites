C++ = g++
C++-FLAGS = -Wall -02 -g
RM = rm -f
INCLUDE = -I ../../include/ -I ../../vision/ -I ./../ -I ./ -I /sw/include/boost-1_33_1/ \
	-I ../../corpus/

VL_SRCS = ../../vision/VisualLine.cpp

VC_SRCS = ../../vision/VisualCorner.cpp \
	../../vision/VisualLandmark.cpp

CL_SRCS = ../../vision/ConcreteLine.cpp \
	../../vision/ConcreteLine.h

CC_SRCS = ../../vision/ConcreteCorner.cpp \
	../../vision/ConcreteCorner.h

UTILITY_SRCS = ../../vision/Utility.cpp

FO_SRCS = ../../vision/VisualFieldObject.cpp

CCFO_SRCS = ../../vision/ConcreteFieldObject.cpp \
	../../vision/ConcreteFieldObject.h

VFO_SRCS = ../../vision/VisualFieldObject.cpp \
	../../vision/VisualFieldObject.h

VLANDMARK_SRCS = ../../vision/VisualLandmark.cpp \
	../../vision/VisualLandmark.h

CLANDMARK_SRCS = ../../vision/ConcreteLandmark.cpp \
	../../vision/ConcreteLandmark.h

EKF_SRCS = ../EKF.cpp \
	../EKF.h

BALLEKF_SRCS = ../BallEKF.cpp \
	../BallEKF.h

OBJS = 	Utility.o \
	VisualLine.o \
	ConcreteLine.o \
	VisualLandmark.o \
	ConcreteLandmark.o \
	VisualCorner.o \
	ConcreteCorner.o \
	Observation.o \
	VisualFieldObject.o \
	ConcreteFieldObject.o \
	EKF.o \
	BallEKF.o \
	MCL.o

OBS_SRCS = ../Observation.cpp \
	   ../Observation.h

MCL_SRCS = ../MCL.cpp \
	   ../MCL.h

FAKER_SRCS = LocLogFaker.cpp \
	LocLogFaker.h

LDLIBS = $(OBJS)
LDFLAGS  = $(LDLIBS) -lm

all : faker

faker : $(FAKER_SRCS) $(OBJS)
	$(C++) $(C++-FLAGS) $(INCLUDE) $(LDFLAGS) -DNO_ZLIB faker.o -o $@

faker.o : $(FAKER_SRCS) $(OBJS)
	$(C++) $(C++-FLAGS) $(INCLUDE) $(LDFLAGS) -c $< -o $@

MCL.o : $(MCL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

Observation.o : $(OBS_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

ConcreteCorner.o : $(CC_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

VisualCorner.o : $(VC_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

ConcreteLine.o : $(CL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

VisualLine.o : $(VL_SRCS) Utility.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

Utility.o : $(UTILITY_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

ConcreteFieldObject.o : $(CCFO_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

VisualFieldObject.o : $(VFO_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

VisualLandmark.o : $(VLANDMARK_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

ConcreteLandmark.o : $(CLANDMARK_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

EKF.o :$(EKF_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

BallEKF.o :$(BALLEKF_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

.Phony : clean

clean :
	$(RM) $(OBJS) mclLogger faker