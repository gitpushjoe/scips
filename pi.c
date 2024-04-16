#include "scratchmips-essentials.h"

int ask_user(bool first) {
	printf("How many %sdigits of pi would you like to see?\n", first ? "" : "more ");
	printf("Enter nothing to exit.\n");
	char buf[10];
	gets(buf, 10);
	int n = atoi(buf);
	while (1) {
		if (*buf == 0) {
			return -1;
		}
		if (n <= 0) {
			printf("Invalid input. \n");
			n = atoi(gets(buf, 10));
		} else {
			break;
		}
	}
	printf("\n%s", first ? "" : "...");
	return n;
}

int pi__start() {
	int r[2800 + 1];
	int i, k;
	int b, d;
	int c = 0;
	int target = ask_user(true);
	if (target == -1) {
		return 0;
	}
	int digits_curr = 0;

	for (i = 0; i < 2800; i++) {
		r[i] = 2000;
	}
	r[i] = 0;

	for (k = 2800; k > 0; k -= 14) {
		d = 0;

		i = k;
		for (;;) {
			d += r[i] * 10000;
			b = 2 * i - 1;

			r[i] = d % b;
			d /= b;
			i--;
			if (i == 0) break;
			d *= i;
		}
		int val = c + d / 10000;
		char digits[4];
		for (int j = 0; j < 4; j++) {
			digits[j] = val % 10;
			val /= 10;
		}
		for (int j = 3; j >= 0; j--) {
			if (++digits_curr > target) {
				printf("...\n\n");
				int n = ask_user(false);
				if (n == -1) {
					return 0;
				}
				target += n;
			}
			if (digits_curr == 2) {
				putchar('.');
			}
			putchar(digits[j] + '0');
		}
		c = d % 10000;
	}

	printf("\nCalculation complete.\n");
	sleep(5000);

	return 0;
}

// https://cs.uwaterloo.ca/~alopez-o/math-faq/mathtext/node12.html
