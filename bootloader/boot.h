#ifndef BOOT_H
#define BOOT_H

struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;  //entry point address
	unsigned int   phoff;  // start of program header
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum; //number of program header
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

/* ELF32 Program header */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz; //this segment's size in the elf file
	unsigned int memsz; //this segment's size in memory
	unsigned int flags; // authority
	unsigned int align;
};

typedef struct ELFHeader ELFHeader;//typedef ,discard "struct"
typedef struct ProgramHeader ProgramHeader;

void waitDisk(void);

void readSect(void *dst, int offset);

/* I/O functions */
static inline char inByte(short port) {
	char data;
	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline int inLong(short port) {
	int data;
	asm volatile("in %1, %0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outByte(short port, char data) {
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

#endif
