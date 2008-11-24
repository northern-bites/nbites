C++ = g++
C++-FLAGS = -wall -02 -g
RM = rm -f
INCLUDE = -I ../../include/ -I ../../vision/ -I ./../ -I ./ -I /sw/include/boost-1_33_1/

VL_SRCS = ../../vision/VisualLine.cpp

VC_SRCS = ../../vision/VisualCorner.cpp \
	../../vision/VisualLandmark.cpp

CL_SRCS = ../../vision/ConcreteLine.cpp

CC_SRCS = ../../vision/ConcreteLandmark.cpp \
	../../vision/ConcreteCorner.cpp

UTILITY_SRCS = ../../vision/Utility.cpp

FO_SRCS = ../../vision/FieldObjects.cpp

CCFO_SRCS = ../../vision/ConcreteFieldObject.cpp

LOG_SRCS = MCLLogger.cpp \
	MCLLogger.h

OBJS = 	Utility.o \
	visualLine.o \
	visualCorner.o \
	concreteCorner.o \
	Observation.o

OBS_SRCS = ../Observation.cpp

MCL_SRCS = ../MCL.cpp

all : mclLogger

mclLogger : $(LOG_SRCS) $(OBS_SRCS) $(CC_SRCS) $(CL_SRCS) $(VL_SRCS) $(VC_SRCS) $(UTILITY_SRCS) $(CCFO_SRCS) $(FO_SRCS) $(MCL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -DNO_ZLIB -o mclLogger $(LOG_SRCS) $(OBS_SRCS) $(CC_SRCS) $(CL_SRCS) $(VL_SRCS) $(VC_SRCS) $(UTILITY_SRCS) $(CCFO_SRCS) $(FO_SRCS) $(MCL_SRCS)

mclTest : mclTest.cpp # $(OBJS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -DNO_ZLIB -o mclTest mclTest.cpp $(OBS_SRCS) $(CC_SRCS) $(CL_SRCS) $(VL_SRCS) $(VC_SRCS) $(UTILITY_SRCS) $(CCFO_SRCS) $(FO_SRCS) $(MCL_SRCS)

Observation.o : visualLine.o visualCorner.o Observation.cpp Observation.h
	$(C++) $(C++-FLAGS) $(INCLUDE) -o Observation.o $(OBS_SRCS)

concreteLine.o : $(CL_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -o conreteLine.o $(CL_SRCS)

concreteCorner.o : $(CC_SRCS) # Utility.o #visualLine.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -o concreteCorner.o $(CC_SRCS)

visualLine.o : $(VL_SRCS) Utility.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -o visualLine.o $(VL_SRCS)

visualCorner.o : $(VC_SRCS) Utility.o
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $(VC_SRCS)

Utility.o : $(UTILITY_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $(UTILITY_SRCS)

concreteFO.o : $(CCFO_SRCS)
	$(C++) $(C++-FLAGS) $(INCLUDE) -c $(CCFO_SRCS)

.Phony : clean

clean :
	$(RM) $(OBJS) mclLogger