# Makefile for dd_reader
# (C) Ramsey Kant 2013

CC := gcc
SRCDIR := src
BINDIR := bin
BUILDDIR := build
TARGET := dd_reader

# GCC Debug Flags, -pg for profiling
DEBUGFLAGS := -g3 -O0 -Wall
RTCHECKS := -fmudflap -fstack-check -gnato
# Production Flags
PRODFLAGS := -Wall -O2
# Active Flags
CFLAGS := $(DEBUGFLAGS) -std=c99
LINK := $(DEBUGFLAGS)

# File Paths
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

$(TARGET): $(OBJECTS)
	@echo " Linking..."; $(CC) $(LINK) $^ -o $(BINDIR)/$(TARGET)
 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) $(CFLAGS) -c -o $@ $<
	
all: $(TARGET)
 
clean:
	@echo " Cleaning..."; rm -rf $(BUILDDIR) $(BINDIR)/$(TARGET)* $(BINDIR)/*.out
 
.PHONY: all clean
