include ../Makefile.config

# NOTE!: do not try to add -On to CFLAGS with gcc4.x, which will optimize the
# memcpy function a lot to let the kernel not work, for fixing this problem,
# please refer to glibc and rewrite the memcpy in include/string.h, or just not
# use any -On options with gcc 4.x when compiling it. in fact, we only can not
# use -On with gcc4.x to compile fork.c, but after that you need to isolate the
# compling procedure of it, it's not good too. for playing with linux-0.11, we
# just have fun, have no much concerning about the performance.

CFLAGS	+= -I../include
LDFLAGS	+= -r
CPP	+= -I../include


%.o: %.nasm
	$(NASM) -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) \
	-c -o $@ $<

OBJS  = debug.o screen.o asm.o panic.o\
		gdt.o interrupt/isr_utils.o interrupt/irq_utils.o interrupt/idt.o interrupt/irq.o\
		interrupt/timer.o interrupt/keyboard.o\
	 	mm/heap.o\
		syscall.o\
		multitask/task.o multitask/tss.o multitask/ldt.o multitask/schedule.o\
		lab.o\
		fs/hd.o fs/fs.o

#mm/paging.o 

kernel.o: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.o $(OBJS)
	sync

clean:
	@rm *.o
	@for i in mm interrupt fs multitask; do cd $$i; rm *.o; cd ..; done
	@for i in *.c;do rm -f `basename $$i .c`.s;done

dep:
	@sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	@(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	@cp tmp_make Makefile
	@for i in chr_drv blk_drv; do make dep -C $$i; done



