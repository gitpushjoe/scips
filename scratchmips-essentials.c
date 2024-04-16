#define NULL ((void *)0)

#define bool int
#define true 1
#define false 0

#define HEAP_CAPACITY 65536

#define _printf(...) __printf(__VA_ARGS__)

void _putchar(char c) {
	__asm__(
		"li $v0, 11\n"
		"syscall\n"
		:
		: "a"(c)
	);
}

int _puts(const char *s) {
	__asm__(
		"li $v0, 4\n"
		"syscall\n"
		:
		: "a"(s)
	);
	return 0;
}

void putsym(int sym) {
	__asm__(
		"li $v0, 3\n"
		"syscall\n"
		:
		: "a"(sym)
	);
}

void __exit(int status) {
	__asm__(
		"li $v0, 10\n"
		"syscall\n"
		:
		: "a"(status)
	);
}

void _putint(int n) {
	if (n < 0) {
		_putchar('-');
		n *= -1;
	}
	int d = n / 10;
	if (d != 0) {
		_putint(d);
	}
	_putchar('0' + n % 10);
}

void *_sbrk(int inc) {
	void *result;
	__asm__(
		"li $v0, 9\n"
		"syscall\n"
		: "=r"(result)
		: "a"(inc)
	);
	return result;
}

char *_gets(char *buf, int size) {
	__asm__(
		"li $v0, 8\n"
		"syscall\n"
		:
		: "a"(buf), "b"(size)
	);
	return buf;
}

char _getchar() {
	char c;
	__asm__(
		"li $v0, 12\n"
		"syscall\n"
		: "=r"(c)
	);
	return c;
}

void __printf(char *fmt, ...) {
	char *s = fmt;
	int *args = (int *)&fmt + 1;
	while (*s) {
		if (*s == '%') {
			s++;
			if (*s == 'd') {
				_putint(*(args++));
			} else if (*s == 's') {
				_puts((char *)(long)(*(args++)));
			} else if (*s == 'c') {
				_putchar(*(args++));
			} else if (*s == '%') {
				_putchar('%');
			}
		} else {
			_putchar(*s);
		}
		s++;
	}
}

void _cls() {
	__asm__(
		"li $v0, 0\n"
		"syscall\n"
	);
}

void *_memset(void *s, int c, int n) {
	char *p = (char *)s;
	while (n--) {
		*(p++) = c;
	}
	return s;
}

void *_memcpy(void *dest, const void *src, int n) {
	char *d = (char *)dest;
	const char *s = (const char *)src;
	while (n--) {
		*(d++) = *(s++);
	}
	return dest;
}


int _strlen(const char *s) {
	int len = 0;
	while (*(s++)) {
		len++;
	}
	return len;
}

int _strcmp(const char *s1, const char *s2) {
	while (*s1 && *s2 && *(s1++) == *(s2++));
	return *s1 - *s2;
}

int rand() {
	int out;
	__asm__(
		"li $v0, 41\n"
		"syscall\n"
		: "=r"(out)
	);
	return out;
}

typedef struct {
	int meta;
} heap_meta;

heap_meta *heap_start = (heap_meta *)NULL; // { size: 0, free: false, start: true, end: false }
heap_meta *heap_end = (heap_meta *)NULL; // { size: 0, free: true, start: false, end: true }

int heap_chunk_size(heap_meta *header) {
	return (header->meta >> 3) << 3;
}

int heap_chunk_is_free(heap_meta *footer) {
	return footer->meta & 1;
}

int heap_chunk_is_start(heap_meta *footer) {
	return footer->meta & 2;
}

int heap_chunk_is_end(heap_meta *footer) {
	return footer->meta & 4;
}

heap_meta *heap_meta_header_to_footer(heap_meta *header) {
	return (heap_meta *)((char *)header + heap_chunk_size(header) + sizeof(heap_meta));
}

heap_meta *heap_meta_footer_to_header(heap_meta *footer) {
	return (heap_meta *)(footer - heap_chunk_size(footer) - sizeof(heap_meta));
}

heap_meta *heap_meta_header_next(heap_meta *header) { // returns the header of the next chunk
	return (heap_meta *)((char *)header + heap_chunk_size(header) + 2 * sizeof(heap_meta));
}

heap_meta *heap_meta_header_prev(heap_meta *header) { // returns the header of the previous chunk
	heap_meta *footer = (heap_meta *)((char *)header - sizeof(heap_meta));
	return heap_meta_footer_to_header(footer);
}

heap_meta *heap_meta_header_update(heap_meta *header, int size, bool free, bool start, bool end) {
	header->meta = (size >> 3 << 3) | (free << 0) | (start << 1) | (end << 2);
	heap_meta *footer = heap_meta_header_to_footer(header);
	footer->meta = (size >> 3 << 3) | (free << 0) | (start << 1) | (end << 2);
	return header;
}

heap_meta *heap_chunk_append(heap_meta *footer, int size, bool free, bool start, bool end) { // appends a new chunka after the given footer
	heap_meta *new_header = (heap_meta *)((char *)footer + sizeof(heap_meta));
	return heap_meta_header_update(new_header, size, free, start, end);
}

heap_meta *heap_chunk_append_end(heap_meta *footer) {
	heap_meta *end = heap_chunk_append(footer, 0, true, false, true);
	heap_end = end;
	return end;
}

void heap_init() {
	heap_meta *start = (heap_meta *)_sbrk(HEAP_CAPACITY);
	heap_meta_header_update(start, 0, false, true, false);
	heap_meta *end = heap_chunk_append_end(heap_meta_header_to_footer(start));
	heap_start = start;
	heap_end = end;
}

void *_malloc(unsigned int size) {
	if (heap_start == NULL) {
		heap_init();
	}
	if (size <= 0) {
		return NULL;
	}
	size = ((size / 8) + 1) * 8; // round up to the nearest multiple of 8
	heap_meta *chunk = heap_start;
	while (1) {
		chunk = heap_meta_header_next(chunk); // chunk header
		if (!heap_chunk_is_free(chunk)) { // note: the end chunk is always free
			continue;
		}

		bool is_end = heap_chunk_is_end(chunk);

		if (is_end) { // append chunk to the end of the heap
			heap_meta_header_update(chunk, size, false, false, false); // initialize the new chunk
			heap_meta *footer = heap_meta_header_to_footer(chunk);
			heap_chunk_append_end(footer); // initialize the end chunk
			return (void *)((char *)chunk + sizeof(heap_meta));
		}

		int curr_chunk_size = heap_chunk_size(chunk);
		if (curr_chunk_size == size) { // if the chunk is exactly the right size
			heap_meta_header_update(chunk, size, false, false, false);
			return (void *)((char *)chunk + sizeof(heap_meta));
		}
		if (curr_chunk_size >= size + sizeof(heap_meta) * 2) { // if there is enough space for a header and footer in between
			heap_meta_header_update(chunk, size, false, false, false); // initialize the new chunk
			heap_meta *footer = heap_meta_header_to_footer(chunk);
			heap_meta *next_chunk = heap_chunk_append(footer, curr_chunk_size - size - sizeof(heap_meta) * 2, true, false, false);
			return (void *)((char *)chunk + sizeof(heap_meta));
		}


	}

	return NULL;
}

void _free(void *ptr) {
	if (ptr == NULL) {
		return;
	}
	heap_meta *header = (heap_meta *)((char *)ptr - sizeof(heap_meta));
	heap_meta_header_update(header, heap_chunk_size(header), true, false, false);
	heap_meta *next = heap_meta_header_next(header);
	if (heap_chunk_is_free(next) && !heap_chunk_is_end(next)) {
		int size = heap_chunk_size(header) + heap_chunk_size(next) + sizeof(heap_meta) * 2;
		heap_meta_header_update(header, size, true, false, false);
	}
	heap_meta *prev = heap_meta_header_prev(header);
	if (heap_chunk_is_free(prev)) {
		int size = heap_chunk_size(prev) + heap_chunk_size(header) + sizeof(heap_meta) * 2;
		heap_meta_header_update(prev, size, true, false, false);
	}
}

int atoi(char *s) {
	int n = 0;
	char *c = s;
	if (*s == '-') {
		s++;
	}
	while (*s) {
		n = n * 10 + (*s - '0');
		s++;
	}
	return n * (c[0] == '-' ? -1 : 1);
}

void itoa(int n, char *s) {
	if (n < 0) {
		*(s++) = '-';
		n = -n;
	}
	int d = n / 10;
	if (d != 0) {
		itoa(d, s);
		while (*s) {
			s++;
		}
	}
	*(s++) = '0' + n % 10;
	*s = '\0';
}

void sleep(unsigned int ms) {
	__asm__(
		"li $v0, 32\n"
		"syscall\n"
		:
		: "a"(ms)
	);
}
