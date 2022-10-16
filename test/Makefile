CC:=gcc
CPP:=g++

# COMPILE OPTIONS
WARNINGS=-Wall -Wextra -Wpedantic -Wno-unused-const-variable
FLAGS=-fsanitize=address -fno-omit-frame-pointer

# Source files and include dirs
SOURCES := $(wildcard *.c)
# Create .o and .d files for every .cc and .S (hand-written assembly) file
OBJECTS := $(patsubst %.c, %.o, $(patsubst %.S, %.o, $(SOURCES)))

# The first rule is the default, ie. "make", "make all" and "make kernel8.img" mean the same
all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

%.o: %.c Makefile
	$(CC) $(WARNINGS) $(FLAGS) $< -o $@

%.o: %.cpp Makefile
	$(CPP) $(WARNINGS) $< -o $@
