PROJECT=eventflow
IDIR=./src
SRCDIR=./src
BINDIR=./bin
CC=g++
CPPFLAGS=-I$(IDIR)
LIBS=-lboost_program_options -lyaml-cpp


run: all
	./bin/eventflow

all: src/Main.cpp src/SystemContext.cpp src/SystemContext.h bin_dir
	g++ src/Main.cpp src/SystemContext.cpp -I./src -lboost_program_options -lyaml-cpp -o bin/$(PROJECT)

.PHONY: bin_dir

bin_dir:
	mkdir -p $(BINDIR)

.PHONY: clean

clean:
	rm -rf $(BINDIR)