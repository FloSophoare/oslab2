#include "boot.h"
#include "string.h"


#define SECTSIZE 512



void bootMain(void) {
	//my code 4 
	//Yeah, my code 4! Bitterness, bleed and tears!! 
	void (*kMainEntry)(void);
	int KernelSize = SECTSIZE * 200;
	unsigned char buf[KernelSize];
	for(int i =0;i < 200; i++)
		readSect(buf + i * SECTSIZE, i+1);
	ELFHeader *elf = (void *)buf;
	ProgramHeader *ph = (void *)elf + elf->phoff;
	for(int i = 0; i < elf->phnum; i++)
	{
		memcpy((void *)ph->vaddr, buf + ph->off, ph->filesz);
		if (ph->memsz > ph->filesz){
			memset((void *)ph->vaddr + ph->filesz, 0, ph->memsz - ph->filesz);
		}
		ph = (void *)ph + elf->phentsize;
	}
	kMainEntry = (void(*)(void))elf->entry; //function pointer
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
