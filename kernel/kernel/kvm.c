#include "x86.h"
#include "device.h"
#include"string.h"

SegDesc gdt[NR_SEGMENTS];       // the new GDT, NR_SEGMENTS=7, defined in x86/memory.h
TSS tss;

//init GDT and LDT
void initSeg() { // setup kernel segements
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	//bian ping mode or not
	//gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0x00200000,0x000fffff, DPL_USER);
	//gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0x00200000,0x000fffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
	gdt[SEG_TSS].s = 0;
	setGdt(gdt, sizeof(gdt)); // gdt is set in bootloader, here reset gdt in kernel

	/*
	 * 初始化TSS
	 */
	tss.esp0 = 0x1fffff;
	tss.ss0 = KSEL(SEG_KDATA);
	asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));

	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	//asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	//asm volatile("movw %%ax,%%fs":: "a" (KSEL(SEG_KDATA)));
	//asm volatile("movw %%ax,%%gs":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));

	lLdt(0);
	
}

void enterUserSpace(uint32_t entry) {
	/*
	 * Before enter user space 
	 * you should set the right segment registers here
	 * and use 'iret' to jump to ring3
	 */
	putStr("Hello, this is the first line in enterUserSpace\n");
	uint32_t EFLAGS = 0;
	asm volatile("pushl %0":: "r" (USEL(SEG_UDATA))); // push ss
	asm volatile("pushl %0":: "r" (0x2fffff));  //what is this?
	asm volatile("pushfl"); //push eflags, sti
	asm volatile("popl %0":"=r" (EFLAGS));
	asm volatile("pushl %0"::"r"(EFLAGS|0x200));
	asm volatile("pushl %0":: "r" (USEL(SEG_UCODE))); // push cs
	asm volatile("pushl %0":: "r" (entry)); 
	putStr("Hello, this is the line befor iret in enterUserSpace\n");
	asm volatile("iret");
	putStr("Hello, this is the line after iret in enterUserSpace\n");
}

/*
kernel is loaded to location 0x100000, i.e., 1MB
size of kernel is not greater than 200*512 bytes, i.e., 100KB
user program is loaded to location 0x200000, i.e., 2MB
size of user program is not greater than 200*512 bytes, i.e., 100KB
*/

void loadUMain(void) {
	// TODO: 参照bootloader加载内核的方式，具体加载到哪里请结合手册提示思考！
	putStr("Hello, this is the first line in loadUMain\n");
	
	
	int i = 0;
	uint32_t elf = 0x300000;
	uint32_t uMainEntry = 0;//read to 0x200000

	for (i = 0; i < 200; i++) {
		readSect((void*)(elf + i*512), 201+i);  //You ruin me!!! Yeah, it's 201 + i but not 1 + i
	}
	putStr("Hello, this is the line after the for:readSect\n");

	// TODO: 填写kMainEntry、phoff、offset...... 然后加载Kernel（可以参考NEMU的某次lab）
	ELFHeader *eh = (void *)elf;
	ProgramHeader *ph = (void *)elf + eh->phoff;
	for(int i = 0; i < eh->phnum; i++)
	{
		if (ph->type == 0x1){
			memcpy((void *)ph->paddr + 0x200000, (void*)elf + ph->off, ph->filesz);
			if (ph->memsz > ph->filesz){
				memset((void *)ph->paddr + 0x200000 + ph->filesz, 0, ph->memsz - ph->filesz);
			}
		}
		ph = (void *)ph + eh->phentsize;
	}
	uMainEntry = (unsigned)eh->entry;
	putStr("Hello, this is the line after for:load and before enterUserSpace\n");
	enterUserSpace(uMainEntry);
	
}
