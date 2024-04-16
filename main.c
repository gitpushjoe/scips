#include "mergesort.c"
#include "pi.c"
#include "tictactoe.c"
#include "connectfour.c"

void display_icon(bool selected) {
	selected ? putsym(GREEN_CIRCLE) : printf("  ");
}

void connect_four__warning() {
	printf("WARNING: Connect Four uses the minimax algorithm to\n");
	printf("determine the best move. This algorithm is very slow and may take a long time to run.\n");
	printf("For best results, it is highly recommended to run this\n");
	printf("program using TurboWarp. To do so, replace");
	printf("\n\"scratch.mit.edu\" with \"turbowarp.org\" in the URL.\n\n");
	
	printf("    [C] Continue\n");
	printf("    [E] Exit\n");

	int chr = 0;
	while (chr != 'c' && chr != 'e') {
		chr = getchar();
		if (chr == 'e') {
			cls();
			return;
		}
		if (chr == 'c') {
			connect_four__start();
		}
	}
}


int __start() {
	int selected = 0;
	while (1) {
		printf("This is scips, a MIPS32 emulator in Scratch.\n");
		printf("Please select a program to run.\n");
		printf("Use the arrow keys to navigate and Space to select.\n\n");

		display_icon(selected == 0);
		printf("  Merge Sort\n");
		display_icon(selected == 1);
		printf("  Digits of Pi\n");
		display_icon(selected == 2);
		printf("  Tic Tac Toe\n");
		display_icon(selected == 3);
		printf("  Connect Four (with Minimax)\n");

		int chr = 0;
		while (!chr) {
			chr = getchar();
			selected += (chr == KEY_DOWN) - (chr == KEY_UP);
			selected = (selected + 4) % 4;
			if (chr == ' ') {
				cls();
				switch (selected) {
					case 0:
						merge_sort__start();
						break;
					case 1:
						pi__start();
						break;
					case 2:
						tictactoe__start();
						break;
					case 3:
						connect_four__warning();
						break;
				}
			}
			chr = (chr == KEY_UP || chr == KEY_DOWN || chr == ' ');
		}
		cls();
	}
}
