#------------------------------------------------------------------------------
CXX=g++ 
LD=g++  

CXXFLAGS     = -O3

ifneq ($(DEBUG),)
CXXFLAGS += -g -fno-inline
endif

ifneq ($(WINDOWS),)
INCLUDES += -I"..\Desktop\ParadoxParser" -I"..\Desktop\boost_1_54_0\boost" -I"..\Desktop\boost_1_54_0" -I"C:\Program Files\Microsoft SDKs\Windows\v7.1\Include\gl" -I".\SDL2-2.0.1\include"
LIBS += "..\Desktop\ParadoxParser\libParser.lib" -L"..\..\..\MinGW\msys\1.0\local\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mconsole -lm -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc
CXXFLAGS += -DWINDOWS 
LDFLAGS += -enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,-subsystem,console 
else
INCLUDES += -I$(PWD) -I/usr/include/boost/ -I/usr/include/GL/ -I$(PWD)/ParadoxParser-master/ 
LIBS += -L/usr/lib/ -lGL $(SDLLIBS) -L$(PWD)/ParadoxParser-master/ -lParser 
endif

.SUFFIXES: 
.PHONY:		clean 

%.o:	%.cc
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@ 

# NB, under Windows the sdl-config output must *follow* the input files or the SDL stuff will be 'undefined'. 
front:	gfront.o utils.o Packet.o Factory.o Button.o Graphics.o Adapters.o Tile.o StringLibrary.o StaticInitialiser.o OpenGL32.Lib 
	$(LD) $^ -o $@ $(LDFLAGS) $(LIBS) 



clean:	
	@rm -f *.o front 

