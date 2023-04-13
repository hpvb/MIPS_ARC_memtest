.ONESHELL:

PROGNAME=memtest

CC=mips64-linux-gnu-gcc
LD=mips64-linux-gnu-ld

CFLAGS=-mabi=32 -march=mips1 -mfp32 -G0 -EL -nostdlib -fno-builtin -mmemcpy -mno-abicalls -fno-pic -mno-unaligned-access
LINKFLAGS=-EL -G0 -static --oformat=elf32-tradlittlemips -x -N -Ttext 80200000 -e start

DEPS = 

ARCOBJ = arclib/arc.o arclib/stdio.o arclib/stdlib.o arclib/string.o
# start.o *must* be first
OBJ = start.o $(ARCOBJ) main.o 

all: $(PROGNAME).exe
iso: $(PROGNAME).iso

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.S $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGNAME).exe: $(PROGNAME).elf tools/binutils-2.24.51.0.1/binutils/objcopy
	tools/binutils-2.24.51.0.1/binutils/objcopy -O ecoff-littlemips -j .text -j .data -j .bss -j .rodata $(PROGNAME).elf $@

$(PROGNAME).elf: $(OBJ)
	$(LD) -o $@ $^ $(LINKFLAGS)

$(PROGNAME).iso: $(PROGNAME).exe
	genisoimage -o $@ $(PROGNAME).exe

tools/binutils-2.24.51.0.1/binutils/objcopy:
	mkdir -p tools
	cd tools
	curl -O https://cdn.kernel.org/pub/linux/devel/binutils/binutils-2.24.51.0.1.tar.bz2
	tar xf binutils-2.24.51.0.1.tar.bz2
	cd binutils-2.24.51.0.1
	./configure --target=mipsel-ecoff --enable-targets=mipsel-elf --disable-werror
	make -j$(nproc)
	cd ../..

distclean:
	rm -rf tools/
	rm -f *.o *.exe *.elf *.iso
	rm -f arclib/*.o

clean:
	rm -f *.o *.exe *.elf *.iso
	rm -f arclib/*.o

