CC=/usr/bin/g++
CFLAGS=
OUT=vliwScheduler

OBJS=vliwScheduler.o

all:$(OUT)

clean:
	rm -r *.o $(OUT)

$(OUT):$(OBJS)
	$(CC) -o $(OUT) $(OBJS)

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

vliwScheduler.o: vliwScheduler.cpp
