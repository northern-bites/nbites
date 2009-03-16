C++ = g++
C++-FLAGS = -Wall -O2 -Wconversion -DNDEBUG -DUSING_LAB_FIELD
RM = rm -f
INCLUDE = -I ../../include/ -I ../../vision/ -I ./../ -I ./ -I /sw/include/boost-1_33_1/

VL_SRCS = ../../vision/VisualLine.cpp \
	../../vision/VisualLine.h
VC_SRCS = ../../vision/VisualCorner.cpp \
	   ../../vision/VisualCorner.h
CL_SRCS = ../../vision/ConcreteLine.cpp \
	  ../../vision/ConcreteLine.h
CC_SRCS = ../../vision/ConcreteCorner.cpp \
	  ../../vision/ConcreteCorner.h
UTILITY_SRCS = ../../vision/Utility.cpp \
	../../vision/Utility.h
FO_SRCS = ../../vision/VisualFieldObject.cpp \
	../../vision/VisualFieldObject.h
CCFO_SRCS = ../../vision/ConcreteFieldObject.cpp \
	    ../../vision/ConcreteFieldObject.h
VFO_SRCS = ../../vision/VisualFieldObject.cpp \
	 ../../vision/VisualFieldObject.h
EKF_SRCS = ../EKF.cpp \
	   ../EKF.h
VISBALL_SRCS = ../../vision/VisualBall.cpp \
	  ../../vision/VisualBall.h
VLANDMARK_SRCS = ../../vision/VisualLandmark.cpp \
	 ../../vision/VisualLandmark.h
VISDETECTION_SRCS = ../../vision/VisualDetection.cpp \
	 ../../vision/VisualDetection.h
CLANDMARK_SRCS = ../../vision/ConcreteLandmark.cpp \
	         ../../vision/ConcreteLandmark.h
BALLEKF_SRCS = ../BallEKF.cpp \
	 ../BallEKF.h
OBS_SRCS = ../Observation.cpp \
	   ../Observation.h
MCL_SRCS = ../MCL.cpp \
	../MCL.h
FAKER_SRCS = LocLogFaker.cpp \
	  LocLogFaker.h

OBJS = Utility.o \
       ConcreteLandmark.o \
       ConcreteCorner.o \
       ConcreteFieldObject.o \
       ConcreteLine.o \
       VisualLandmark.o \
       VisualDetection.o \
       VisualFieldObject.o \
       VisualCorner.o \
       VisualLine.o \
       VisBall.o \
       Observation.o \
       MCL.o \
       EKF.o \
       BallEKF.o

LDLIBS = $(OBJS)
LDFLAGS = $(LDLIBS)

all : faker

# The faker tool
faker : $(FAKER_SRCS) $(OBJS) faker.o
	$(C++) $(C++-FLAGS) $(INCLUDE) $(LDFLAGS) faker.o -DNO_ZLIB -o $@
faker.o : $(FAKER_SRCS) $(OBJS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

# Vision utilities
Utility.o : $(UTILITY_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

# Vision stuff
# Concrete Vision Stuff
ConcreteLandmark.o : $(CLANDMARK_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
ConcreteCorner.o : $(CC_SRCS) ConcreteLandmark.o
	 $(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
ConcreteFieldObject.o : $(CCFO_SRCS) ConcreteLandmark.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
ConcreteLine.o : $(CL_SRCS)
	 $(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
# Visual Vision Stuff
VisualDetection.o : $(VISDETECTION_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
VisualLandmark.o : $(VLANDMARK_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
VisualCorner.o : $(VC_SRCS) VisualLandmark.o
	 $(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
VisualFieldObject.o : $(VFO_SRCS) VisualLandmark.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
VisualLine.o : $(VL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
VisBall.o : $(VISBALL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

# Localization stuff
Observation.o : $(OBS_SRCS)
	 $(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
MCL.o : $(MCL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
EKF.o : $(EKF_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@
BallEKF.o :$(BALLEKF_SRCS) EKF.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $< -o $@

.Phony : clean

clean :
	$(RM) $(OBJS) faker.o faker