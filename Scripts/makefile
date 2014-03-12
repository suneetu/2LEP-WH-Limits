# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -O3 -pthread -m64 -I $(ROOTSYS)/include
CXXLIBS = -L $(ROOTSYS)/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

# ****************************************************
# Targets needed to bring the executable up to date

all: tcuts tcuts_tables

tcuts: tcuts.o
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o tcuts tcuts.o
	
tcuts_tables: tcuts_tables.o
	$(CXX) $(CXXFLAGS) $(CXXLIBS) -o tcuts_tables tcuts_tables.o

tcuts.o: tcuts.cxx
	$(CXX) $(CXXFLAGS) -c tcuts.cxx

tcuts_tables.o: tcuts_tables.cxx
	$(CXX) $(CXXFLAGS) -c tcuts_tables.cxx
	
clean:
	rm tcuts.o tcuts_table.o
