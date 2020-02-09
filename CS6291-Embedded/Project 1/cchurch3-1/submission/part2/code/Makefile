CC=/usr/bin/g++
CFLAGS=-std=c++11
OUT=vliwSpeculation

OBJS=vliwSpeculation.o

all:$(OUT)

clean:
	rm -r *.o $(OUT)

$(OUT):$(OBJS)
	$(CC) -o $(OUT) $(OBJS)

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

vliwSpeculation.o: vliwSpeculation.cpp
