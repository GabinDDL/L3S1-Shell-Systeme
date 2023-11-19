# Makefile for jsh project

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g
INCLUDES = -I include

# Valgrind options
VALGRIND = valgrind
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

# Code formatting tool
CLANG_FORMAT = clang-format
FORMAT_FLAGS = -i

# Project structure
SRCDIR = src
BUILTINDIR = $(SRCDIR)/builtins
UTILSDIR = $(SRCDIR)/utils
TESTDIR = tests
OBJDIR = obj
BINDIR = bin

# Source, object, and test files
SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(BUILTINDIR)/*.c) $(wildcard $(UTILSDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TESTS = $(wildcard $(TESTDIR)/*.c)

# Executable name
EXECUTABLE = $(BINDIR)/jsh

# Default target
all: $(EXECUTABLE)

# Linking the executable
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Compiling source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Running tests with Valgrind
test: $(EXECUTABLE)
	@for test in $(TESTS); do \
		echo "Running $$test with Valgrind"; \
		$(VALGRIND) $(VFLAGS) ./$(EXECUTABLE) < $$test; \
	done

# Run the executable
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Code formatting
format:
	$(CLANG_FORMAT) $(FORMAT_FLAGS) $(SOURCES) $(wildcard include/*.h)

# Cleaning up
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets
.PHONY: all clean test format
