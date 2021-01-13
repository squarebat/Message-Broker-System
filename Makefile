CC = g++
CPPFLAGS = -g
LIBS = -lboost_program_options -lyaml-cpp
SRCS = src/Main.cpp src/SystemContext.cpp src/SystemContext.h
BINDIR = bin
PROJECT = eventflow

all: bin_dir $(BINDIR)/$(PROJECT)

$(BINDIR)/$(PROJECT): $(SRCS)
	$(CC) $(CPPFLAGS) $(SRCS) $(LIBS) -o $@

.PHONY: bin_dir

bin_dir: $(BINDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean

clean:
	rm -rf $(BINDIR)
