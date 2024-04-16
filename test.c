#include "scratchmips-essentials.h"

int fibonacci(int n) {
	if (n <= 1) {
		return n;
	}
	return fibonacci(n - 1) + fibonacci(n - 2);
}

int __start() {
	int n = 5;
	puts("Fibonacci series:\n");
	for (int i = 0; i < n; i++) {
		puts("fibonacci(");
		putint(i);
		puts(") = ");
		putint(fibonacci(i));
		putchar('\n');
	}
	const char *test = "Hello, world!";
	((char*)test)[0] = 'h';
	for (const char *p = test; *p; p++) {
		putchar(*p);
	}
	int *p = (int *)_malloc(32 * sizeof(int));
	puts("Pointer: ");
	putint((long)p);
	for (int i = 0; i < 32; i++) {
		p[i] = 32 - i;
	}
	puts("\nArray: ");
	for (int *q = p; q < p + 32; q++) {
		putchar(' ');
		putint(*q);
	}
	putchar('\n');
	char buf[32];
	puts(buf);
	putchar('\n');
	putint(-2);
	while (1) {
		puts("Enter a string: ");
		gets(buf, 32);
		putchar('\n');
		if (!_strcmp(buf, "EXIT")) {
			break;
		}
		puts("You entered: ");
		puts(buf);
		putchar('\n');
	}

	puts("Size of int = ");
	putint(sizeof(int));
	putchar('\n');

	int *a = (int *)_malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) {
		a[i] = i;
	}
	int *b = (int *)_malloc(5 * sizeof(int));
	for (int i = 0; i < 5; i++) {
		b[i] = 100 - i;
	}
	free(a);
	int *c = (int *)_malloc(6 * sizeof(int));
	for (int i = 0; i < 6; i++) {
		c[i] = 200 - i;
	}
	int *d = (int *)_malloc(2 * sizeof(int));
	for (int i = 0; i < 2; i++) {
		d[i] = 500 - i;
	}

	puts("Heap data:\n");
	for (int i = 0; i < 30; ++i) {
		putint(a[i]);
		putchar('\n');
	}

	puts("\n b: ");
	for (int i = 0; i < 5; i++) {
		putint(b[i]);
		putchar(' ');
	}

	puts("\n a = ");
	putint((long)a);

	puts("\n b = ");
	putint((long)b);

	puts("\n c = ");
	putint((long)c);

	puts("\n d = ");
	putint((long)d);

	puts("\n c: ");
	for (int i = 0; i < 6; i++) {
		putint(c[i]);
		putchar(' ');
	}

	puts("\n d: ");
	for (int i = 0; i < 2; i++) {
		putint(d[i]);
		putchar(' ');
	}

	puts("\n\n");
	putint(d - a);


	return 0;
}
