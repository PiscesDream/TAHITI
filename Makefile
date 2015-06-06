OS = Tahiti 

# configurations of makefile
include Makefile.config

LDFLAGS	+= -Ttext 0 #-e startup_32
CFLAGS	+= -Iinclude -I./
CPP	+= -Iinclude -I./

ARCHIVES= kernel/kernel.o
DRIVERS =
LIBS	=
FILEDIR = user/files
FILES = $(FILEDIR)/hi.txt  $(FILEDIR)/sub.txt $(FILEDIR)/program1.bin $(FILEDIR)/long.txt   

%.o: %.nasm
	$(NASM) -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: disk.img

#build/build.sh boot/bootstrap.bin boot/setup.bin build/system.stripped disk.img 

disk.img: programs boot/bootstrap.bin boot/setup.bin build/system.stripped
	#python build/burn.py -rec=on\
	#	-seek=0 boot/bootstrap.bin\
	#	-seek=512 boot/setup.bin\
	#	-seek=3072 build/system.stripped 
	dd if=/dev/zero of=disk.img count=2880
	dd if=boot/bootstrap.bin of=disk.img conv=notrunc
	mount -o loop,fat=12 -t msdos disk.img ./mountpoint/
	cp boot/setup.bin ./mountpoint/
	cp build/system.stripped ./mountpoint/
	cp -r $(FILEDIR)/* ./mountpoint/
	rm build/system.stripped
	sync
	@echo "TAHITI is done."

programs:
	make -C user

# make a striped system
build/system.stripped: build/system.tmp 
	$(STRIP) build/system.tmp
	$(OBJCOPY) -O binary -R .comment -R .note build/system.tmp build/system.stripped
	#rm build/system.tmp

build/system.tmp: boot/head.o init/main.o \
		$(ARCHIVES) $(DRIVERS) $(LIBS)
	$(LD) $(LDFLAGS) boot/head.o init/main.o $(ARCHIVES) $(DRIVERS) $(LIBS) -o build/system.tmp 

boot/bootstrap.bin: boot/bootstrap.o
	$(LD) $(LDFLAGS) --oformat binary -o $@ $^

boot/setup.bin: boot/setup.o
	$(LD) $(LDFLAGS) --oformat binary -o $@ $^

kernel/kernel.o: 
	make -C kernel


clean:
	-umount ./mountpoint
	@rm -f disk.img build/system.*
	@rm -f boot/*.bin boot/*.o init/*.o
	@for i in kernel boot; do make clean -C $$i; done 

run: disk.img
	@qemu-system-x86_64 -m 16M -boot a -fda disk.img #-hda $(HDA_IMG)

debug:
	@echo $(OS)
	@qemu-system-x86_64 -m 16M -boot a -fda disk.img -s -S #-hda $(HDA_IMG) -s -S

bochs: disk.img
	@bochs -q

