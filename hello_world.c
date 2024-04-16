#define puts _puts

void _puts(const char *s) {
	__asm__(
		"li $v0, 4\n"
		"syscall\n"
		:
		: "a"(s)
	);
}

int __start() {
	puts("Hello, World!\n");
	return 0;
}
