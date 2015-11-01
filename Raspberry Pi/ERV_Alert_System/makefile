SRC=$(wildcard src/*.c)
CFLAGS=-Wall -Wextra
OPENVGFLAGS=-I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -I..
LIBRARIES=-lshapes -lm

all: project

project:
	gcc  $(SRC) $(CFLAGS) $(OPENVGFLAGS) -o2 -o runApp $(LIBRARIES)