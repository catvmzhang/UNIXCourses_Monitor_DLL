CC = gcc
PROG = fsmon.so

all: $(PROG)

%.o:%.c
	$(CC) $^ -c -o $@ -fPIC  

fsmon.so: fsmon.c def.o
	$(CC) -shared -o $@ def.o -fPIC $< -D_GNU_SOURCE -ldl 
