#include "boot.h"

#define SECTSIZE 512





void bootMain(void) {
	int i = 0;
	int phoff = 0x34; //why 0x34? Cuz start of program header is 52 bytes, which is 0x34 .
	int offset = 0x1000; //why 0x1000?
	unsigned int elf = 0x100000; // what is it used for?
	void (*kMainEntry)(void);
	kMainEntry = (void(*)(void))0x100000; // This is a function at 0x100000, so  maybe I need to load a function to 0x100000

	for (i = 0; i < 200; i++) {
		readSect((void*)(elf + i*512), 1+i);
	}

	// TODO: 填写kMainEntry、phoff、offset...... 然后加载Kernel（可以参考NEMU的某次lab）
	ELFHeader* eh = (ELFHeader*) elf;  //This is wrong?
	ProgramHeader*  ph = eh + phoff;
	//ProgramHeader* eph = ph + eh->phnum;
	int phnum = eh->phnum;
	for (int i = 0; i < phnum; i++){
		if (ph->type == 0x1){
			int paddr = ph->vaddr;
			memcpy(paddr, ph->off,  ph->filesz);
			if (ph->memsz > ph->filesz){
				memset(paddr + ph->filesz, 0, ph->memsz - ph->filesz);
			}
		}
		ph = ph + eh->phentsize;
	}

	kMainEntry = (void(*)(void))eh->entry; //function pointer
	kMainEntry();
}

void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
	int i;
	waitDisk();
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}
