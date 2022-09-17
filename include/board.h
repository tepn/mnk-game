#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdio.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define BOARD_MAX 64
#define NONE '_'
#define PLAYER1 'X'
#define PLAYER2 'O'

typedef struct
{
	uint64_t* rotate0;   /* Bitboard rotated of 0 degrees */
	uint64_t* rotate90;  /* Bitboard rotated of 0 degrees */
	uint64_t* rotate45l; /* Bitboard rotated of 0 degrees */
	uint64_t* rotate45r; /* Bitboard rotated of 0 degrees */
} bitboard_t;

typedef struct
{
	int m; /* Board width */
	int n; /* Board height */
	bitboard_t* player1; /* Bitboard for player1 */
	bitboard_t* player2; /* Bitboard for player2 */
} board_t;

/*
	allocate a board_t, returns NULL if one allocation is failed
	otherwise, returns a pointer  to the board_t
*/
board_t* board_new(const int m, const int n);

/*
	free the structure of the whole board
*/
void board_delete(board_t* board);

/*
	returns a pointer to a copy of the board,
	returns NULL if the pointer of the board is NULL
	or if the allocation of the new board fails
*/
board_t* board_copy(const board_t* board);

/*
	sets all the uint64_t to 0
	does nothing if the pointer is NULL
*/
void board_clear(const board_t* board);

/*
	sets a stone at coordinates <line,column>
	returns 1 if all went right, 0 if the cell is occupied, -1 otherwise
*/
int board_set(const board_t* board, int player, const int line, const int column);

/*
	returns 0 if the cell is empty
	1 if the stone is from player1
	2 if the stone is from player2
	-1 if an error occured
*/
int board_get(const board_t* board, const int line, const int column);

/*
	displays the board
*/
void board_display(const board_t* board, FILE* stream);

/*
	returns the victorious player or 0
*/
int board_win(const board_t* board, const int k);

/*
	returns the number of empty cells on the board
	returns 0 if the pointer is NULL
*/
int board_count_empty_cells(const board_t* board);

#endif /* BOARD_H */
