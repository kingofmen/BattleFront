#------------------------------------------------------------------------------
CXX=g++ 
LD=g++  

CXXFLAGS     = -O3

ifneq ($(DEBUG),)
CXXFLAGS += -g -fno-inline
endif


ifneq ($(WINDOWS),)
INCLUDES += -I"..\Desktop\ParadoxParser" -I"..\Desktop\boost_1_54_0\boost" -I"..\Desktop\boost_1_54_0" -I"..\..\..\Qt\2010.05\qt\include\QtOpenGL" -I".\SDL2-2.0.1\include"
LIBS += "..\Desktop\ParadoxParser\libParser.lib" -L"C:\MinGW\msys\1.0\local\lib" -lSDL2 -L"c:\Qt\2010.05\qt\lib" -static-libgcc -lopengl32
CXXFLAGS += -DWINDOWS
else
INCLUDES += -I$(PWD) -I/usr/include/boost/ -I/usr/include/GL/ -I$(PWD)/ParadoxParser-master/ 
LIBS += -L/usr/lib/ -lGL $(SDLLIBS) -L$(PWD)/ParadoxParser-master/ -lParser 
endif

.SUFFIXES: 
.PHONY:		clean 

%.o:	%.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@ 

front:	gfront.o utils.o Packet.o Factory.o Tile.o 
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@



clean:	
	@rm -f *.o front 

