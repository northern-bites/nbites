CYAN   = `tput setaf 6`
NORMAL = `tput sgr0`

LGSHARE_DIR= $(NBITES_DIR)/src/share/logshare/
LOGGING_DIR= $(NBITES_DIR)/src/man/logging/
NBCROSS_DIR= $(NBITES_DIR)/src/nbcross/

SHARE_DIR = $(NBITES_DIR)/src/share/include/

LGSHARE_SRC= $(LGSHARE_DIR)/*.cpp
LOGGING_SRC= $(LOGGING_DIR)/*.cpp
NBCROSS_SRC= $(NBCROSS_DIR)/nbcross.cpp

SOUND_SRC=src/sound/*.cpp

NBC_SND_SRC= src/nbcross_sound/SoundDefs.cpp
WHS_SND_SRC= src/whistle/whistle.cpp

SENDR_SRC= src/sendr/*.cpp src/nowifi/*.cpp
RECVR_SRC= src/recvr/*.cpp src/nowifi/*.cpp

DBFLAGS= -ggdb

BUILD= g++ -std=c++11 -w -D NBL_STANDALONE -D WHISTLE_COMPILE $(LGSHARE_SRC) -I $(LGSHARE_DIR) -I $(SHARE_DIR) -I ./lib_include/ -lpthread

default: detection

detection: whistle cross

nowifi: sendr recvr

#####################
#targets for whistle detection
.PRECIOUS: cross
cross:
	@echo $(CYAN) compiling cross executable $(NORMAL)
	$(BUILD) -ggdb -I $(NBCROSS_DIR) $(NBCROSS_SRC) -D DETECT_LOG_RESULTS $(SOUND_SRC) $(NBC_SND_SRC) -lasound -lrt lib/libfftw3f.a -o cross
	./cross

whistle:
	@echo $(CYAN) compiling whistle executable $(NORMAL)
	$(BUILD) -I $(LOGGING_DIR) $(LOGGING_SRC) $(SOUND_SRC) $(WHS_SND_SRC) -lasound -lrt lib/libfftw3f.a -o whistle

#####################
#targets for nowifi

sendr:
	@echo $(CYAN) compiling SENDR executable $(NORMAL)
	$(BUILD) -I $(LOGGING_DIR) $(LOGGING_SRC) $(SOUND_SRC) $(SENDR_SRC) -lasound -lrt lib/libfftw3f.a -o sendr

recvr:
	@echo $(CYAN) compiling RECVR executable $(NORMAL)
	$(BUILD) -I $(LOGGING_DIR) $(LOGGING_SRC) $(SOUND_SRC) $(RECVR_SRC) -lasound -lrt lib/libfftw3f.a -o recvr

clean:
	rm src/**.o
	rm *.o
	rm whistle cross sendr recvr


