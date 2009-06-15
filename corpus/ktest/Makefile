


MAN_DIR=../man

CXX_INCLUDES=-I../../include -I../../corpus -I../../motion
CXX_FLAGS=-Wall -Wno-unused #-DNDEBUG -O3
CXX=g++



oldik : OldInverseKinematics.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INCLUDES) -c ../OldInverseKinematics.cpp

newik : InverseKinematics.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INCLUDES) -c ../InverseKinematics.cpp

oldmain : oldik
	$(CXX) $(CXX_FLAGS) $(CXX_INCLUDES) -o oldiktest main.cpp OldInverseKinematics.o

newmain : newik
	$(CXX) $(CXX_FLAGS) $(CXX_INCLUDES) -o newiktest main.cpp InverseKinematics.o

com : COM.cpp
		$(CXX) $(CXX_FLAGS) $(CXX_INCLUDES) -o comtest COM.cpp

all: com