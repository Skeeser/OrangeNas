CXX ?= gcc

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

client: client.c 
	$(CXX) -o client  $^ $(CXXFLAGS) 

clean:
	rm -r client
