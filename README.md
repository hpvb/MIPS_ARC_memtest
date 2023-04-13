# MIPS ARC Firmware memory test

This repository contains a naive memory testing program that runs natively on the MIPS ARC firmware in little endian mode.

Currently tested on the following configurations:

* MIPS ARCSystem Magnum 4000PC-50
* Qemu emulator using -M magnum
* MAME emulator mmr4000le

Should work on the following as well, but untested:

* ACER PICA-61
* MIPS ARCSystem Millennium 4000PC-50
* MIPS ARCSystem Millennium 4000SC-50
* NEC RISCstation Image
* NEC RISCstation 2000
* Olivetti M700-10
* And other Microsoft Jazz based ARC systems

![Memtest running on MIPS Magnum hardware](images/ARC_Memtest_MIPS_Magnum_4000PC.jpg?raw=true)

# Building

## Requirements

* A MIPS compiler (Assuming to be targeting mips64-linux-gnu-)
* curl (To download binutils)
* (optional) genisoimage

## Building

Simply type `make`. This will build binutils 2.24.51.0.1 and the program.

If you would like an iso image that can be loaded directly into Qemu or be burnt onto a CD run `make iso`

# Running

Insert the CD or Floppy with the exe file into the computer.

Once at the ARC Miltiboot menu select "Run a program" and type `cd:\memtest` if running from CD, or `a:\memtest` when running from a floppy.

**NOTE** When running the system for the first time you must first:

* select "Run setup" from the ARC Multiboot menu. 
* select "Initialize system"
* select "Set default configuration"
* select a monitor resolution and leave everything else at defaults
* select "Return to main menu"
* select "Exit"

On real hardware and MAME at this point the system will reboot, in Qemu the system will hang. Close Qemu and restart it.

## Firmware

The required firmware files are available on various places online:

NT prom:
one of:

* [mmr4000le.zip](https://archive.org/download/MAME221RomsOnlyMerged/mmr4000le.zip)
* [setup.zip](http://hpoussineau.free.fr/qemu/firmware/magnum-4000/setup.zip)

MAME specific:

* [ps2_keybc.zip](https://archive.org/download/MAME208RomsOnlyMerged/ps2_keybc.zip)
* [kb_ms_natural.zip](https://archive.org/download/MAME208RomsOnlyMerged/[kb_ms_natural.zip)

## Qemu

`qemu-system-mips64el -d unimp -M magnum -m 64 -net nic -net user -global ds1225y.filename=nvram -bios NTPROM.RAW -cdrom memtest.iso`

## MAME

`mame mmr4000le -window -resolution0 auto -ramsize 32M -autoframeskip -nomax -cdrom memtest.iso`

# Limitations / Bugs

The compiler uses system include files for all C library functions, and arclib only implements a small handful. The functions used in the memtest work fine. Furthermore to use malloc() and friends the memory regions available have to be parsed and registered with the malloc implementation. The memory test program does not do this.

Also note that in the start.S of this repository memory caches are disabled. This is to aid in the memory testing, if used as a template for other programs this should probably be removed.
