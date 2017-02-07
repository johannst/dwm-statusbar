# dwm-statusbar - Makefile
# author: johannst

BIN = dwm-statusbar
SOURCE = statusbar.cc
OBJS = $(SOURCE:%.cc=%.o)

CXX_FLAGS = -O2 -std=c++11
LD_FLAGS = -lX11

build: $(OBJS)
	g++ -o $(BIN) $(OBJS) $(CXX_FLAGS) $(LD_FLAGS)

%.o: %.cc
	g++ -o $@ -c $< $(CXX_FLAGS)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)
