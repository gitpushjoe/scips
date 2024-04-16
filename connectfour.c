#include "scratchmips-essentials.h"

#define EMPTY_SLOT "----"
#define INT_MAX 0x7FFFFFFF 
#define INT_MIN -0x7FFFFFFF

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a < b ? a : b;
}

typedef enum Status {
	IN_PROGRESS = 0,
	PLAYER_WINS = 1,
	COMPUTER_WINS = -1,
	DRAW = 2,
	OTHER = 3
} Status;

typedef struct Board {
	int board[6][7];
	bool is_player_turn;
	Status status;
	int piece_count;
	int static_eval;
} Board;


#define EMPTY 0

#define EMPTY_SQUARE 0

#define CONTESTANT_BITMASK  0x30000000
#define PLAYER   0x30000000
#define COMPUTER 0x10000000

int get_contestant(int piece) {
	return piece & CONTESTANT_BITMASK;
}

Board *board_create() {
	Board *board = (Board *)malloc(sizeof(Board));
	board->is_player_turn = true;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			board->board[i][j] = EMPTY_SQUARE;
		}
	}
	board->piece_count = 0;
	board->static_eval = 0;
	return board;
}

void board_destroy(Board *board) {
	free(board);
}

void board_print(const Board *board) {
	cls();
	for (int i = 0; i < 6; i++) {
		putsym(BLACK_SQUARE);
		putchar(' ');
		for (int j = 0; j < 7; j++) {
			switch (get_contestant(board->board[i][j])) {
				case EMPTY:
					puts(EMPTY_SLOT);
					break;
				case PLAYER:
					putsym(YELLOW_CIRCLE);
					break;
				case COMPUTER:
					putsym(PURPLE_CIRCLE);
					break;
			}
		}
		putsym(BLACK_SQUARE);
		printf("\n");
	}
	for (int i = 0; i < 9; i++) {
		putsym(BLACK_SQUARE);
	}
	printf("\n      1   2   3   4   5   6   7");
}

bool square_in_bounds(int i, int j) {
	return j >= 0 && j < 7 && i >= 0 && i < 6;
}

const int board_status_lookup[] = { // { di, dj, mask }
	 1,  0,        0x3, // VERTICAL
	 0,  1,      0x300, // HORIZONTAL
	 1,  1,    0x30000, // NEGATIVE DIAGONAL 
	-1,  1,  0x3000000, // POSITIVE DIAGONAL
};

// This function will return the status of the board after a move is played.
// It will also update the board's static evaluation, for use in the minimax algorithm.
Status board_play(Board *board, int i, int j) { // assumes the move is valid
	const int curr_contestant = board->is_player_turn ? PLAYER : COMPUTER;
	int curr_piece = board->is_player_turn ? PLAYER : COMPUTER;
	int sign = board->is_player_turn ? 1 : -1;
	if (board->piece_count == 42) {
		board->board[i][j] = curr_contestant;
		board->status = DRAW;
		return DRAW;
	}
	for (int k = 0; k < 4; ++k) {
		int di = board_status_lookup[k * 3];
		int dj = board_status_lookup[k * 3 + 1];
		int mask = board_status_lookup[k * 3 + 2];
		int forward = 0, backward = 0;
		if (square_in_bounds(i + di, j + dj)) {
			int piece = board->board[i + di][j + dj];
			if (get_contestant(piece) == curr_contestant) {
				forward = (piece & mask) >> (k * 8); // number of consecutive pieces in the forward direction
			} else {
				forward = (piece != EMPTY) ? 8 : 0; // 8 is a special value to indicate that there is an enemy piece in the way
			}
		}
		if (square_in_bounds(i - di, j - dj)) {
			int piece = board->board[i - di][j - dj];
			if (get_contestant(piece) == curr_contestant) {
				backward += (piece & mask) >> (k * 8); // number of consecutive pieces in the backward direction
			} else {
				backward = (piece != EMPTY) ? 8 : 0;
			}
		}
		int sum = (1 + forward + backward) % 8; // total number of consecutive pieces; note that it ignores enemy pieces (8)
		if (sum >= 4) {
			board->board[i][j] = curr_contestant; // heuristics are no longer necessary, but we still need to update the board for printing
			board->static_eval = sign == 1 ? 99999 : -99999; // update the static evaluation
			return curr_contestant == PLAYER ? PLAYER_WINS : COMPUTER_WINS;
		}
		curr_piece |= (sum << (k * 8)); // update this piece with the new consecutive count for this direction
		board->static_eval += sign * sum; // update the static evaluation
		
		switch ((backward << 4) | forward) { // iterate through all possible combinations, updating the consecutive counts
			case ((0 << 4) | 0):
				break;
			case ((0 << 4) | 1): {
				int *forward_piece = &board->board[i + di][j + dj];
				*forward_piece &= ~mask;
				*forward_piece |= (sum << (k * 8)); // replace the old consecutive count with the new one
				board->static_eval += sign; // add/remove an extra point to the static evaluation because the forward piece also has a new "connection"
				break;
			}
			case ((0 << 4) | 2): {
				int *forward_piece = &board->board[i + di][j + dj];
				int *second_forward_piece = &board->board[i + 2 * di][j + 2 * dj];
				*forward_piece &= ~mask;
				*forward_piece |= (sum << (k * 8));
				*second_forward_piece &= ~mask;
				*second_forward_piece |= (sum << (k * 8));
				board->static_eval += sign * 2; // two points for the two forward pieces both gaining a new "connection"
				break;
			}
			case ((0 << 4) | 3):
				printf("ERROR: Unreachable condition { forward = 3, backward = 0, k = %d }\n", k); // 4 in a row, win logic should have been triggered
				__exit(1);
				break;
			case ((0 << 4) | 8): {
				int enemy_forward_piece = board->board[i + di][j + dj]; // the enemy piece in the forward direction
				int reward = (enemy_forward_piece & mask) >> (k * 8); // the number of consecutive enemy pieces in that direction
				board->static_eval += sign * reward * reward; // after calculating the number of enemy pieces in that direction, add a reward for blocking them
				break;
			}
			case ((1 << 4) | 0): {
				int *backward_piece = &board->board[i - di][j - dj];
				*backward_piece &= ~mask;
				*backward_piece |= (sum << (k * 8));
				board->static_eval += sign;
				break;
			}
			case ((1 << 4) | 1): {
				int *backward_piece = &board->board[i - di][j - dj];
				int *forward_piece = &board->board[i + di][j + dj];
				*forward_piece &= ~mask;
				*forward_piece |= (sum << (k * 8));
				*backward_piece &= ~mask;
				*backward_piece |= (sum << (k * 8));
				board->static_eval += sign * 2;
				break;
			}
			case ((1 << 4) | 2):
				printf("ERROR: Unreachable condition { forward = 2, backward = 1, k = %d }\n", k); 
				break;
			case ((1 << 4) | 3):
				printf("ERROR: Unreachable condition { forward = 3, backward = 1, k = %d }\n", k); 
				break;
			case ((1 << 4) | 8): {
				int enemy_forward_piece = board->board[i + di][j + dj];
				int *backward_piece = &board->board[i - di][j - dj];
				int reward = (enemy_forward_piece & mask) >> (k * 8);
				*backward_piece &= ~mask;
				*backward_piece |= (sum << (k * 8));
				board->static_eval += sign * (1 + reward * reward);
				break;
			}

			case ((2 << 4) | 0): {
				int *backward_piece = &board->board[i - di][j - dj];
				int *second_backward_piece = &board->board[i - 2 * di][j - 2 * dj];
				*backward_piece &= ~mask;
				*backward_piece |= (sum << (k * 8));
				*second_backward_piece &= ~mask;
				*second_backward_piece |= (sum << (k * 8));
				board->static_eval += sign * 2;
				break;
			}
			case ((2 << 4) | 1):
				printf("ERROR: Unreachable condition { forward = 1, backward = 2, k = %d }\n", k); 
				break;
			case ((2 << 4) | 2):
				printf("ERROR: Unreachable condition { forward = 2, backward = 2, k = %d }\n", k); 
				break;
			case ((2 << 4) | 3):
				printf("ERROR: Unreachable condition { forward = 3, backward = 2, k = %d }\n", k); 
				break;
			case ((2 << 4) | 8): {
				int enemy_forward_piece = board->board[i + di][j + dj];
				int *backward_piece = &board->board[i - di][j - dj];
				int *second_backward_piece = &board->board[i - 2 * di][j - 2 * dj];
				int reward = (enemy_forward_piece & mask) >> (k * 8);
				*backward_piece &= ~mask;
				*backward_piece |= (sum << (k * 8));
				*second_backward_piece &= ~mask;
				*second_backward_piece |= (sum << (k * 8));
				board->static_eval += sign * (2 + reward * reward);
				break;
			}

			case ((3 << 4) | 0):
				printf("ERROR: Unreachable condition { forward = 0, backward = 3, k = %d }\n", k); 
				break;
			case ((3 << 4) | 1):
				printf("ERROR: Unreachable condition { forward = 1, backward = 3, k = %d }\n", k); 
				break;
			case ((3 << 4) | 2):
				printf("ERROR: Unreachable condition { forward = 2, backward = 3, k = %d }\n", k); 
				break;
			case ((3 << 4) | 3):
				printf("ERROR: Unreachable condition { forward = 3, backward = 3, k = %d }\n", k); 
				break;
			case ((3 << 4) | 8): 
				printf("ERROR: Unreachable condition { forward = 8, backward = 3, k = %d }\n", k); 
				break;

			case ((8 << 4) | 0): {
				int enemy_backward_piece = board->board[i - di][j - dj];
				int reward = (enemy_backward_piece & mask) >> (k * 8);
				board->static_eval += sign * reward * reward;
				break;
			}
			case ((8 << 4) | 1): {
				int enemy_backward_piece = board->board[i - di][j - dj];
				int *forward_piece = &board->board[i + di][j + dj];
				int reward = (enemy_backward_piece & mask) >> (k * 8);
				*forward_piece &= ~mask;
				*forward_piece |= (sum << (k * 8));
				board->static_eval += sign * (1 + reward * reward);
				break;
			}
			case ((8 << 4) | 2): {
				int enemy_backward_piece = board->board[i - di][j - dj];
				int *forward_piece = &board->board[i + di][j + dj];
				int *second_forward_piece = &board->board[i + 2 * di][j + 2 * dj];
				int reward = (enemy_backward_piece & mask) >> (k * 8);
				*forward_piece &= ~mask;
				*forward_piece |= (sum << (k * 8));
				*second_forward_piece &= ~mask;
				*second_forward_piece |= (sum << (k * 8));
				board->static_eval += sign * (2 + reward * reward);
				break;
			}
			case ((8 << 4) | 3):
				printf("ERROR: Unreachable condition { forward = 3, backward = 8 }\n"); 
				break;
			case ((8 << 4) | 8): {
				int enemy_forward_piece = board->board[i + di][j + dj];
				int enemy_backward_piece = board->board[i - di][j - dj];
				int reward = (((enemy_forward_piece & mask) >> (k * 8)) + ((enemy_backward_piece & mask) >> (k * 8)));
				board->static_eval += sign * reward * reward;
				break;
			}
		}
	}
	board->board[i][j] = curr_piece;
	return IN_PROGRESS;
}

bool board_attempt(Board *board, int column) {
	if (column < 0 || column >= 7) {
		return false;
	}
	if (board->board[0][column] != EMPTY) {
		return false;
	}
	for (int i = 5; i >= 0; i--) {
		if (board->board[i][column] == EMPTY) {
			board->piece_count++;
			board->status = board_play(board, i, column);
			board->is_player_turn = !board->is_player_turn;
			return true;
		}
	}
	return false;
}

int alpha_beta_search(Board search_space[], int idx, int depth, int alpha, int beta, bool is_player, int *best_move) {
	Board *node = search_space + idx;
	if (depth == 0) {
		return node->static_eval;
	}
	if (node->status != IN_PROGRESS) {
		return node->static_eval;
	}
	int value;
	int best_move_value = INT_MAX;
	if (is_player) {
		value = INT_MIN;
		for (int i = 1; i < 8; i++) {
			Board *child_node = search_space + (idx * 8) + i;
			memcpy(child_node, node, sizeof(Board));
			if (!board_attempt(child_node, i - 1)) {
				continue;
			}
			int child_value = alpha_beta_search(search_space, (idx * 8) + i, depth - 1, alpha, beta, false, best_move);
			value = max(value, child_value);
			if (value >= beta) {
				break;
			}
			alpha = max(alpha, value);
		}
		return value;
	} else {
		value = INT_MAX;
		for (int i = 1; i < 8; i++) {
			Board *child_node = search_space + (idx * 8) + i;
			memcpy(child_node, node, sizeof(Board));
			if (!board_attempt(child_node, i - 1)) {
				continue;
			}
			int child_value = alpha_beta_search(search_space, (idx * 8) + i, depth - 1, alpha, beta, true, best_move);
			value = min(value, child_value);
			if (value <= alpha) {
				break;
			}
			if (idx < 8 && child_value < best_move_value) {
				*best_move = i - 1;
				best_move_value = child_value;
			}
			beta = min(beta, value);
		}
		return value;
	}
}

int minimax(Board *board) {
	if (board->status != IN_PROGRESS) {
		return -1;
	}
	Board search_space[01000]; // max depth = 3
	memcpy(&search_space->board, board, sizeof(Board));
	int best_move = 0;
	alpha_beta_search(search_space, 0, 2, INT_MIN, INT_MAX, false, &best_move);
	return best_move;
}


int connect_four__start() {
	Board *board = board_create();
	board_print(board);
	while (1) {
		int column;
		do {
			if (board->is_player_turn) {
				puts("\n\n[1 - 7]: Play move\n");
				puts("[E]: Exit\n");
				int chr = getchar();
				if (chr == 'E' || chr == 'e') {
					board_destroy(board);
					return 0;
				} else if (chr < '1' || chr > '7') {
					board_print(board);
					continue;
				}
				column = chr - '1';
				board_attempt(board, column);
			} else {
				puts("\n\nComputer is thinking...\n");
				puts("Please wait up to 5 seconds.\n");
				column = minimax(board);
				int saved_column = column;
				while (!board_attempt(board, column)) {
					printf("Computer can't play in column %d.\n", column + 1);
					column = (column + 1) % 7;
					if (column == saved_column) {
						puts("\nComputer can't play.\n");
						break;
					}
				}
			}
			board_print(board);
		} while (board->status == IN_PROGRESS);
		switch (board->status) {
			case PLAYER_WINS:
				puts("\n\nPlayer wins!");
				break;
			case COMPUTER_WINS:
				puts("\n\nComputer wins!");
				break;
			case DRAW:
				puts("\n\nDraw!");
				break;
			case IN_PROGRESS:
			case OTHER:
				break;
		}
		printf("\nPress any key to play again.\n");
		getchar();
		free(board);
		board = board_create();
		board_print(board);
	}
	return 0;
}
