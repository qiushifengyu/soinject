unsigned char clone_64[] = {
  0x41, 0x54, 0x4c, 0x8b, 0x64, 0x24, 0x10, 0x50, 0x57, 0x56, 0x52, 0x41,
  0x52, 0x41, 0x51, 0xb8, 0x38, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x20, 0x00,
  0x00, 0x48, 0x31, 0xf6, 0x48, 0x31, 0xd2, 0x4d, 0x31, 0xd2, 0x4d, 0x31,
  0xc9, 0x0f, 0x05, 0x48, 0x83, 0xf8, 0x00, 0x75, 0x03, 0x41, 0xff, 0xe4,
  0x41, 0x59, 0x41, 0x5a, 0x5a, 0x5e, 0x5f, 0x58, 0x41, 0x5c, 0xc3
};
unsigned int clone_64_len = 59;

unsigned char while_print_64[] = {
  0xeb, 0x12, 0xb8, 0x01, 0x00, 0x00, 0x00, 0xbf, 0x02, 0x00, 0x00, 0x00,
  0x5e, 0xba, 0x06, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xe8, 0xe9, 0xff, 0xff,
  0xff, 0x63, 0x75, 0x6c, 0x6f, 0x21, 0x0a
};
unsigned int while_print_64_len = 31;

int main() {
	__asm__ __volatile__ (
	"push %0"		
	:
	:"r" (while_print_64)
	:
	);
	((void (*)())clone_64)();
	while (1)
		printf("**ciao\n");
}