include ../Makefile.config

LDFLAGS	+= -Ttext 0 
CFLAGS	+= -Iinclude -I./
CPP	+= -Iinclude -I./

ARCHIVES = 
DRIVERS  =
LIBS	 = library/syscall.o library/stdio.o library/system.o library/string.o library/ctype.o 

%.o: %.nasm
	$(NASM) -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
%.bin: %.o $(LIBS)
	$(LD) $(LDFLAGS) --oformat binary -o $@ $^

	 #programs/program1.bin programs/program2.bin
all: $(LIBS)\
	 programs/lab8.bin programs/lab9.bin
	mv ./programs/*.bin ./files/



clean:
	-rm *.o
	-rm *.bin
	-rm programs/*.o
	-rm programs/*.bin
	-rm library/*.o
	-rm library/*.bin
