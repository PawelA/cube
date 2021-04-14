CFLAGS = -O2
LDLIBS = -lX11 -lGL -lm
cube: cube.o gl.o loadgl.o x.o fps.o
cube.o: cube.c gl.h loadgl.h x.h fps.h
gl.o: gl.c gl.h
loadgl.o: loadgl.c gl.h
x.o: x.c x.h
fps.o: fps.c fps.h

clean:
	rm -f cube *.o
