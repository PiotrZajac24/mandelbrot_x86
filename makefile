CXX=gcc
CFLAGS= -Wall -m64
LDFLAGS=-L/usr/lib -lallegro
INCLUDE=-I. -I/usr/include/allegro5
OBJS=mandelbrot.o f.o
OUT=mandelbrot

all:	mandel

mandelbrot.o: mandelbrot.c

f.o: f.s
	nasm -f elf64 -o f.o f.s

clean:	
	rm -rf *.o	mandelbrot

mandel: $(OBJS)
	$(CXX) $(OBJS) -o $(OUT) $(INCLUDE) $(CFLAGS) $(LDFLAGS)
