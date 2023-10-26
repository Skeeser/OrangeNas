CXX ?= gcc

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

server: server.c 
	$(CXX) -o server  $^ $(CXXFLAGS) 

clean:
	rm -r server
