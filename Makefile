CC = g++
CPPFLAGS = -g -pthread -I./src -I./spdlog/include/ 
LIBS = -lboost_program_options -lboost_system -lyaml-cpp -lcrypto -lssl 
BINDIR = bin
PROJECT = eventflow
OBJECTS = $(BINDIR)/Main.o $(BINDIR)/StatusLog.o $(BINDIR)/SystemContext.o $(BINDIR)/AccessList.o $(BINDIR)/Topic.o $(BINDIR)/Client.o 
CROW = $(BINDIR)/crow_all.h.gch
PUBSINK = $(BINDIR)/publish_to_topic_sink.h.gch
LOGDIR = logs

all: $(BINDIR) $(LOGDIR) $(BINDIR)/$(PROJECT)

$(BINDIR)/$(PROJECT): $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) $(LIBS) -o $@

$(BINDIR)/Main.o: src/Main.cpp src/SystemContext.h 
	$(CC) -c $(CPPFLAGS) src/Main.cpp -o $@

$(BINDIR)/SystemContext.o: src/SystemContext.cpp src/SystemContext.h $(CROW)
	$(CC) -c $(CPPFLAGS) -I./$(BINDIR) src/SystemContext.cpp -o $@

$(BINDIR)/AccessList.o: src/AccessList.cpp
	$(CC) -c $(CPPFLAGS) src/AccessList.cpp -o $@

$(BINDIR)/Topic.o: src/Topic.cpp src/Topic.h
	$(CC) -c $(CPPFLAGS) src/Topic.cpp -o $@

$(BINDIR)/Client.o: src/Client.cpp src/Client.h
	$(CC) -c $(CPPFLAGS) src/Client.cpp -o $@

$(BINDIR)/StatusLog.o: src/StatusLog.cpp src/StatusLog.h $(PUBSINK)
	$(CC) -c $(CPPFLAGS) src/StatusLog.cpp -o $@

$(PUBSINK): src/publish_to_topic_sink.h
	$(CC) -w -c $(CPPFLAGS) src/publish_to_topic_sink.h -o $@

$(CROW): src/crow_all.h
	$(CC) -w -c $(CPPFLAGS) src/crow_all.h -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

$(LOGDIR):
	mkdir -p $(LOGDIR)
	touch $(LOGDIR)/logfile.txt

.PHONY: clean

clean:
	rm -rf $(BINDIR) $(LOGDIR)
