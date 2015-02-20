#/*Makefile*/
#g++ -std=c++11 main.cpp objects.cpp ray.cpp Transform.cpp -o test -L/Users/pavela/Development/raytracer/lib/FreeImage/ -lfreeimage

CXX = g++
CXXFLAGS = -Wall -std=c++11
#INC = -I include/src -I include/glm -I include/FreeImage -I include/* -I include/gtest/../
INC = -I include
VPATH = include #general search path, instead of defining every directory and subdirectory
LDLIBS = -lfreeimage -lgtest -lgtest_main
LDFLAGS = -L lib -L lib/gtest
SRCDIR = src
BUILDDIR = build
TARGET = bin
EXE = rt

# src files with directory i.e. src/test.cpp
DIR_SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# src files
SOURCES = $(patsubst $(SRCDIR)/%.cpp, %.cpp, $(DIR_SOURCES))

# one object for each src files
OBJECTS = $(SOURCES:%.cpp=%.o)
# Note that this relies on the assumption of a successful build, and is not dynamic
DIR_OBJECTS = $(patsubst %.o, $(BUILDDIR)/%.o, $(OBJECTS))

all: clean $(EXE)
	
# An empty rule
%.cpp:
	
# targets …: target-pattern: prereq-patterns …
$(OBJECTS): %.o: %.cpp
	@echo "Building $@ from $<"
	$(CXX) $(CXXFLAGS) $(INC) -c $(SRCDIR)/$< -o $(BUILDDIR)/$@

#$(DIR_OBJECTS): $(OBJECTS)
#	$@ = $(wildcard $(BUILDDIR)/*.o)
#	@echo "DIR" $(DIR_OBJECTS)

$(EXE): $(OBJECTS)
	@echo "Linking objects"
	@echo $(DIR_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(DIR_OBJECTS) -o $(TARGET)/$(EXE)

clean:
	rm -rf $(BUILDDIR)/*.o
	rm -rf $(TARGET)/*
