########################################################################
####################### Makefile Template ##############################
########################################################################

# I used a makefile template but it didn't work as expected so I heavily modified it.

# Compiler settings - Can be customized.
CC = g++
CXXFLAGS = -std=c++11 -Wall -g -I.
LDFLAGS = -lpthread

# Makefile settings - Can be customized.
APPNAME = Golf-Server
EXT = .cpp
SRCDIR = src
OBJDIR = obj
BINDIR = bin

############## Do not change anything from here downwards! #############
# Recursive wildcar to search in all subdirectories no matter the depth.
# Source: https://stackoverflow.com/questions/4036191/sources-from-subdirectories-in-makefile
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Look at all the subdirectories in the src directory and compile them.
SRC := $(call rwildcard,$(SRCDIR)/,*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
# UNIX-based OS variables & settings
RM = rm -rf
########################################################################
####################### Targets beginning here #########################
########################################################################

all: $(APPNAME)

# Builds the app
$(APPNAME): $(OBJ) 
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	mkdir -p $(BINDIR)
	mv $(APPNAME) $(BINDIR)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -o $@ -c $<

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project and also deletes objdir even if its not empty
.PHONY: clean
clean:
	$(RM) $(OBJDIR) $(BINDIR) $(APPNAME)
	