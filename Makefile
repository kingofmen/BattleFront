#------------------------------------------------------------------------------
CXX=g++ 
LD=g++  

CXXFLAGS     = -O3
SDLLIBS = $(shell sdl2-config --libs)
SDLFLAGS = $(shell sdl2-config --cflags)
CXXFLAGS += $(SDLFLAGS)

ifneq ($(DEBUG),)
CXXFLAGS += -g -fno-inline
endif


INCLUDES += -I$(PWD) -I/usr/include/boost/ -I/usr/include/GL/ -I$(PWD)/ParadoxParser-master/ 
LIBS += -L/usr/lib/ -lGL $(SDLLIBS) -L$(PWD)/ParadoxParser-master/ -lParser 

.SUFFIXES: 
.PHONY:		clean 

%.o:	%.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@ 

front:	gfront.o utils.o Packet.o Factory.o Tile.o 
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@



clean:	
	@rm -f *.o front 

