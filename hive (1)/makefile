# Compiler
CC := g++
# Flags
CFLAGS := -std=c++11 -Wall -Wextra -pedantic -g
# Include directories
INCLUDES := -I./header
# Source files
SRCDIR := src
# Object files directory
OBJDIR := obj
# Executable name
TARGET := final.out

# Source files
SRCS := $(wildcard $(SRCDIR)/*.cpp)
# Object files
OBJS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

# Targets
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

.PHONY: all clean

