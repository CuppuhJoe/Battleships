CXXFLAGS	= -std=c++11 -Wall -O3
MAKEFLAGS	+= --silent

SRCS = $(wildcard *.cpp)
PROGS = $(patsubst %.cpp, %, $(SRCS))

all: $(PROGS)
	echo "mv *.ai ../AI_Executables"
	echo "cp *.py ../AI_Executables"
	mv *.ai ../AI_Executables
	cp *.py ../AI_Executables

%: %.cpp
	echo "$(CXX) $(CXXFLAGS) $(CFLAGS) -o $@.ai $<"
	$(CXX) $(CXXFLAGS) $(CFLAGS) -o $@.ai $<





