# Makefile for jsh project

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -g
INCLUDES = -I include

# Valgrind options
VALGRIND = valgrind
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --errors-for-leak-kinds=all

# Code formatting tool
CLANG_FORMAT = clang-format
FORMAT_FLAGS = -i

# Project structure
SRCDIR = src
BUILTINDIR = $(SRCDIR)/builtins
UTILSDIR = $(SRCDIR)/utils
PARSERDIR = $(SRCDIR)/parser
TESTDIR = tests
BUILTINTESTDIR = $(TESTDIR)/builtins
UTILSTESTDIR = $(TESTDIR)/utils
PARSERTESTDIR = $(TESTDIR)/parser
OBJDIR = obj
BINDIR = bin

# Source, object, and test files
SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(BUILTINDIR)/*.c) $(wildcard $(UTILSDIR)/*.c) $(wildcard $(PARSERDIR)/*.c)
APP_SOURCES = $(filter-out $(SRCDIR)/main.c, $(SOURCES))
APP_OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(APP_SOURCES))
TEST_SOURCES = $(wildcard $(TESTDIR)/*.c) $(wildcard $(BUILTINTESTDIR)/*.c) $(wildcard $(UTILSTESTDIR)/*.c) $(wildcard $(PARSERTESTDIR)/*.c)
TEST_OBJECTS = $(APP_OBJECTS) $(patsubst $(TESTDIR)/%.c,$(OBJDIR)/%.o,$(TEST_SOURCES))

# Executable names
EXECUTABLE = $(BINDIR)/jsh
TEST_EXECUTABLE = $(BINDIR)/test_main

# Default target
all: $(EXECUTABLE)

# Linking the executable
$(EXECUTABLE): $(APP_OBJECTS) $(OBJDIR)/main.o
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Compiling source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compiling test source files
$(OBJDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile the test executable
$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Running tests with Valgrind
test: $(TEST_EXECUTABLE)
	@echo "Running tests with Valgrind"
	$(VALGRIND) $(VFLAGS) ./$(TEST_EXECUTABLE)
	@echo "Tests completed"

# Run the executable
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Code formatting
format:
	$(CLANG_FORMAT) $(FORMAT_FLAGS) $(SOURCES) $(wildcard include/*.h)

# Check code formatting
check-format:
	$(CLANG_FORMAT) --dry-run --Werror $(SOURCES) $(wildcard include/*.h)

# Cleaning up
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets
.PHONY: all clean test format run
