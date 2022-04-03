#include "boot.h"
#include "string.h"


#define SECTSIZE 512



void bootMain(void) {
	// my code 1
	/*int i = 0;
	int phoff = 0x34; //why 0x34? Cuz start of program header is 52 bytes, which is 0x34 .
	int offset = 0x1000; //why 0x1000?
	//unsigned int elf = 0x100000; // what is it used for?
	unsigned int elf = 0x100000;
	void (*kMainEntry)(void);
	kMainEntry = (void(*)(void))0x100000; // This is a function at 0x100000, so  maybe I need to load a function to 0x100000
	//kMainEntry = (void(*)(void))0x300000; // This is in my code but not in zuchuan code
	for (i = 0; i < 200; i++) {
		readSect((void*)(elf + i*512), 1+i);
	}*/

	// TODO: 填写kMainEntry、phoff、offset...... 然后加载Kernel（可以参考NEMU的某次lab）
	/*ELFHeader* eh = (ELFHeader*) elf;  //This is wrong?
	phoff = eh->phoff;
	ProgramHeader*  ph = eh + phoff;
	int phnum = eh->phnum;
	for (int i = 0; i < phnum; i++){
		if (ph->type == 0x1){
			int paddr = ph->paddr;
			//memcpy(paddr, elf+ph->off,  ph->filesz);
			for (int j = 0; j < ph->filesz; j++){
				*(char*) (ph->paddr + j) = *(char*)(elf + ph->off + j);
			}
		}
		ph = ph + eh->phentsize;
	}

	kMainEntry = (void(*)(void))eh->entry; //function pointer
	kMainEntry();*/


	// zuchuan code, able to run but wrong
	/*ELFHeader* eh=(ELFHeader*)elf;
    kMainEntry=(void(*)(void))(eh->entry);
    phoff=eh->phoff;
    ProgramHeader* ph=(ProgramHeader*)(elf+phoff);
    offset=ph->off;

    for (i = 0; i < 200 * 512; i++) {
        *(unsigned char *)(elf + i) = *(unsigned char *)(elf + i + offset);
    }
    kMainEntry();*/

	//my code2
	/*int i = 0;
	int phoff = 0x34;
	unsigned int elf = 0x300000;
	void (*kMainEntry)(void);
	kMainEntry = (void(*)(void))0x100000;
	for (i = 0; i < 200; i++) {
		readSect((void*)(elf + i*512), 1+i);
	}
	// TODO: 填写kMainEntry、phoff、offset...... 然后加载Kernel（可以参考NEMU的某次lab）
	ELFHeader* eh= (ELFHeader*)0x300000;
	phoff=eh->phoff;
	kMainEntry=(void*)eh->entry;
	ProgramHeader* ph=(ProgramHeader*)(elf+phoff);
	for (i = 0; i < eh->phnum; i++){
		if (ph[i].type==1){
			for (int j = 0; j < ph[i].filesz; j++)
				*(char*)(ph[i].paddr+j) = *(char*)(elf+ph[i].off+j);
		}
	}
	kMainEntry = (void(*)(void))eh->entry; //function pointer
	kMainEntry();*/


	//my code 3
	/*void (*p_kern)();
	int SectNum = 200;
	int KernelSize = SECTSIZE * SectNum;
	unsigned char buf[KernelSize];
	for(int i =0;i < SectNum;++i)
		readSect(buf + i * SECTSIZE, i+1);
	struct ELFHeader *elf = (void *)buf;
	struct ProgramHeader *ph = (void *)elf + elf->phoff;
	for(int i = elf->phnum; i > 0; --i)
	{
		memcpy((void *)ph->vaddr, buf + ph->off, ph->filesz);
		memset((void *)ph->vaddr + ph->filesz, 0, ph->memsz - ph->filesz);
		ph = (void *)ph + elf->phentsize;
	}
	p_kern = (void *)elf->entry;
	p_kern();*/

	//my code 4
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
