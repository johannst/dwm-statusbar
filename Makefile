# dwm-statusbar - Makefile
# author: johannst

SOURCE = statusbar.cc
OBJS = $(SOURCE:%.cc=%.o)

CXX_FLAGS = -O2
LD_FLAGS = -lX11

build: $(OBJS)
	g++ -o statusbar $(OBJS) $(LD_FLAGS)

%.o: %.cc
	g++ -o $@ -c $< $(CXX_FLAGS)

clean:
	rm -f $(OBJS)
	rm -f statusbar
