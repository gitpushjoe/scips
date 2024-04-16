#include "scratchmips-essentials.h"

typedef struct game {
	char board[9];
	char player;
	char winner;
} game;

game *game_create() {
	game *g = (game *)_malloc(sizeof(game));
	memset(g->board, ' ', 9);
	g->player = 'X';
	g->winner = ' ';
	return g;
}

void game_destroy(game *g) {
	_free(g);
}

void game_print(game *g) {
	printf("  %c  |  %c |  %c\n", g->board[0] == ' ' ? '1' : g->board[0], g->board[1] == ' ' ? '2' : g->board[1], g->board[2] == ' ' ? '3' : g->board[2]);
	printf("-----+-----+-----\n");
	printf("  %c  |  %c |  %c\n", g->board[3] == ' ' ? '4' : g->board[3], g->board[4] == ' ' ? '5' : g->board[4], g->board[5] == ' ' ? '6' : g->board[5]);
	printf("-----+-----+-----\n");
	printf("  %c  |  %c  |  %c\n", g->board[6] == ' ' ? '7' : g->board[6], g->board[7] == ' ' ? '8' : g->board[7], g->board[8] == ' ' ? '9' : g->board[8]);
}

void game_evaluate(game *g) {
	char *b = g->board;
	if (b[0] == b[1] && b[1] == b[2] && b[0] != ' ') {
		g->winner = b[0];
	} else if (b[3] == b[4] && b[4] == b[5] && b[3] != ' ') {
		g->winner = b[3];
	} else if (b[6] == b[7] && b[7] == b[8] && b[6] != ' ') {
		g->winner = b[6];
	} else if (b[0] == b[3] && b[3] == b[6] && b[0] != ' ') {
		g->winner = b[0];
	} else if (b[1] == b[4] && b[4] == b[7] && b[1] != ' ') {
		g->winner = b[1];
	} else if (b[2] == b[5] && b[5] == b[8] && b[2] != ' ') {
		g->winner = b[2];
	} else if (b[0] == b[4] && b[4] == b[8] && b[0] != ' ') {
		g->winner = b[0];
	} else if (b[2] == b[4] && b[4] == b[6] && b[2] != ' ') {
		g->winner = b[2];
	} else if (b[0] != ' ' && b[1] != ' ' && b[2] != ' ' &&
			   b[3] != ' ' && b[4] != ' ' && b[5] != ' ' &&
			   b[6] != ' ' && b[7] != ' ' && b[8] != ' ') {
		g->winner = 'D';
	}
}

int game_play(game *g, int pos) {
	if (g->board[pos] != ' ') {
		return 0;
	}
	g->board[pos] = g->player;
	g->player = g->player == 'X' ? 'O' : 'X';
	game_evaluate(g);
	return 1;
}

int tictactoe__start() {
	game *g = game_create();
	int pos;
	while (1) {
		cls();
		game_print(g);
		printf("\nPlayer %d's turn: \n", (g->player == 'O') + 1);
		printf("  [1-9]: Play\n");
		printf("  [E]: Exit\n");
		int chr = getchar();
		if (chr == 'e' || chr == 'E') {
			break;
		}
		pos = chr - '1';
		if (pos < 0 || pos > 8) {
			printf("\nInvalid position.\n");
			sleep(1000);
			continue;
		}
		if (!game_play(g, pos)) {
			printf("\nPosition already taken.\n");
			sleep(1000);
			continue;
		}
		if (g->winner != ' ') {
			cls();
			game_print(g);
			if (g->winner == 'D') {
				printf("\nIt's a draw!\n");
			} else {
				printf("\nPlayer %d wins!\n", (g->winner == 'O') + 1);
			}
			printf(" [R]: Restart\n");
			printf(" [E]: Exit\n");
			int chr = getchar();
			while (1) {
				if (chr == 'r' || chr == 'R') {
					break;
				}
				if (chr == 'e' || chr == 'E') {
					goto end;
				}
			}
			g = game_create();
		}
	}
end:
	game_destroy(g);
	return 0;
}
