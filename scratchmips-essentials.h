#pragma once

enum Symbol {
	RED_HEART = 0,		RED_CIRCLE, 		RED_ALT_0,		RED_ALT_1,		RED_SQUARE,
	BLUE_HEART,			BLUE_CIRCLE,		BLUE_ALT_0,		BLUE_ALT_1,		BLUE_SQUARE,
	YELLOW_HEART,		YELLOW_CIRCLE,		YELLOW_ALT_0,	YELLOW_ALT_1,	YELLOW_SQUARE,
	GREEN_HEART,		GREEN_CIRCLE,		GREEN_ALT_0,	GREEN_ALT_1,	GREEN_SQUARE,
	ORANGE_HEART,		ORANGE_CIRCLE,		ORANGE_ALT_0,	ORANGE_ALT_1,	ORANGE_SQUARE,
	PURPLE_HEART,		PURPLE_CIRCLE,		PURPLE_ALT_0,	PURPLE_ALT_1,	PURPLE_SQUARE,
	WHITE_HEART,		WHITE_CIRCLE,		WHITE_ALT_0,	WHITE_ALT_1,	WHITE_SQUARE,
	BLACK_HEART,		BLACK_CIRCLE,		BLACK_ALT_0,	BLACK_ALT_1,	BLACK_SQUARE,
	BROWN_HEART,		BROWN_CIRCLE,		BROWN_ALT_0,	BROWN_ALT_1,	BROWN_SQUARE,
};

void _putchar(char c);
int _puts(const char *s);
void putsym(int sym);
void __exit(int status);
void _putint(int n);
void *_sbrk(int inc);
char *_gets(char *buf, int size);
char _getchar();
void _cls();
void _sleep(unsigned int ms);
void _printf(char *fmt, ...);

void *_memset(void *s, int c, int n);
void *_memcpy(void *dest, const void *src, int n);
int _strlen(const char *s);
int _strcmp(const char *s1, const char *s2);

int _rand();

void *_malloc(unsigned int size);
void _free(void *ptr);

void heap_init();

int atoi(char *s);
void itoa(int n, char *s);

#define NULL ((void *)0)
#define putchar _putchar
#define puts _puts
#define exit __exit
#define putint _putint
#define sbrk _sbrk
#define gets _gets
#define getchar _getchar
#define printf _printf
#define memset _memset
#define memcpy _memcpy
#define strlen _strlen
#define strcmp _strcmp
#define malloc _malloc
#define free _free
#define cls _cls
#define sleep _sleep
#define rand _rand

#define KEY_UP 17
#define KEY_DOWN 18
#define KEY_LEFT 19
#define KEY_RIGHT 20

#include "scratchmips-essentials.c"
