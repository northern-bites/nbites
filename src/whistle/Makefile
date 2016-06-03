CYAN            = `tput setaf 6`
NORMAL          = `tput sgr0`

SRC= src/*.cpp 
HDR= src/*.h 

whistle: $(HDR) $(SRC)
	@echo $(CYAN) "Compiling whistle.." $(NORMAL)
	g++ -std=c++11 -o whistle $(SRC) lib/libfftw3f.a -lasound -lpthread -lrt

clean:
	rm whistle
