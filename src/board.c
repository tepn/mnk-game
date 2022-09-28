#include "board.h"

#include <math.h>
#include <stdlib.h>

board_t*
board_new(const int m, const int n)
{
	if ((m < 1) || (n < 1) || (m > BOARD_MAX) || (n > BOARD_MAX))
		return NULL;

	board_t* board = malloc(sizeof(board_t));

	if (board == NULL)
	{
		perror("Error allocation\n");
		return NULL;
	}

	board->player1 = malloc(sizeof(bitboard_t));

	if (board->player1 == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player2 = malloc(sizeof(bitboard_t));

	if (board->player2 == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player1->rotate0 = malloc(n * sizeof(uint64_t));

	if (board->player1->rotate0 == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player1->rotate90 = malloc(m * sizeof(uint64_t));

	if (board->player1->rotate90 == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player1->rotate45l = malloc((m + n - 1) * sizeof(uint64_t));

	if (board->player1->rotate45l == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player1->rotate45r = malloc((m + n - 1) * sizeof(uint64_t));

	if (board->player1->rotate45r == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player2->rotate0 = malloc(n * sizeof(uint64_t));

	if (board->player2->rotate0 == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player2->rotate90 = malloc(m * sizeof(uint64_t));

	if (board->player2->rotate90 == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player2->rotate45l = malloc((m + n - 1) * sizeof(uint64_t));

	if (board->player2->rotate45l == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->player2->rotate45r = malloc((m + n - 1) * sizeof(uint64_t));

	if (board->player2->rotate45r == NULL)
	{
		perror("Error allocation\n");
		board_delete(board);
		return NULL;
	}

	board->m = m;
	board->n = n;

	board_clear(board);

	return board;
}

void
board_delete(board_t* board)
{
	if (board != NULL)
	{
		if (board->player1 != NULL)
		{
			if (board->player1->rotate0 != NULL)
				free(board->player1->rotate0);

			if (board->player1->rotate90 != NULL)
				free(board->player1->rotate90);

			if (board->player1->rotate45l != NULL)
				free(board->player1->rotate45l);

			if (board->player1->rotate45r != NULL)
				free(board->player1->rotate45r);

			free(board->player1);
		}

		if (board->player2 != NULL)
		{
			if (board->player2->rotate0 != NULL)
				free(board->player2->rotate0);

			if (board->player2->rotate90 != NULL)
				free(board->player2->rotate90);

			if (board->player2->rotate45l != NULL)
				free(board->player2->rotate45l);

			if (board->player2->rotate45r != NULL)
				free(board->player2->rotate45r);

			free(board->player2);
		}

		free(board);

	}
}

board_t*
board_copy(const board_t* board)
{
	if (board == NULL)
		return NULL;

	board_t* board_c = board_new(board->m, board->n);

	if (board_c == NULL)
		return NULL;

	board_c->m = board->m;
	board_c->n = board->n;

	for (int i = 0; i < board->n; i++)
	{
		board_c->player1->rotate0[i] = board->player1->rotate0[i];
		board_c->player2->rotate0[i] = board->player2->rotate0[i];
	}

	for (int i = 0; i < board->m; i++)
	{
		board_c->player1->rotate90[i] = board->player1->rotate90[i];
		board_c->player2->rotate90[i] = board->player2->rotate90[i];
	}

	for (int i = 0; i < (board->m + board->n - 1); i++)
	{
		board_c->player1->rotate45l[i] = board->player1->rotate45l[i];
		board_c->player1->rotate45r[i] = board->player1->rotate45r[i];

		board_c->player2->rotate45l[i] = board->player2->rotate45l[i];
		board_c->player2->rotate45r[i] = board->player2->rotate45r[i];
	}

	return board_c;
}

void
board_clear(const board_t* board)
{
	if (board != NULL)
	{
		for (int i = 0; i < board->n; i++)
		{
			board->player1->rotate0[i] = 0ULL;
			board->player2->rotate0[i] = 0ULL;
		}

		for (int i = 0; i < board->m; i++)
		{
			board->player1->rotate90[i] = 0ULL;
			board->player2->rotate90[i] = 0ULL;
		}

		for (int i = 0; i < (board->m + board->n - 1); i++)
		{
			board->player1->rotate45l[i] = 0ULL;
			board->player1->rotate45r[i] = 0ULL;

			board->player2->rotate45l[i] = 0ULL;
			board->player2->rotate45r[i] = 0ULL;
		}
	}
}

int
board_set(const board_t* board, int player, const int line, const int column)
{
	if (board == NULL)
		return -1;

	if ((line < 1) || (line > board->n) || (column < 1) || (column > board->m))
		return -1;

	if (board_get(board, line, column) == -1)
	{
		return -1;
	}
	else if (board_get(board, line, column) > 0)
	{
		return 0;
	}
	else if (board_get(board, line, column) == 0)
	{
		/* Cell is empty */
		if (player == 1)
		{
			board->player1->rotate0[line - 1] |= (1ULL << (column - 1));
			board->player1->rotate90[column - 1] |= (1ULL << (line - 1));

			/* rotate 45l */
			if (line <= MIN(board->m, board->n))
			{
				board->player1->rotate45l[abs(board->m - abs(line - column)) - 1]
				 |= (1ULL << (MIN(line , column) - 1));
			}
			else
			{
				board->player1->rotate45l[board->m + abs(line - column) - 1]
				 |= (1ULL << (MIN(line , column) - 1));
			}

			/* rotate 45r */
			board->player1->rotate45r[line + column - 2]
			 |= (1ULL << (column - 1));
		}
		else if (player == 2)
		{
			board->player2->rotate0[line - 1] |= (1ULL << (column - 1));
			board->player2->rotate90[column - 1] |= (1ULL << (line - 1));

			/* rotate 45l */
			if (line <= MIN(board->m, board->n))
			{
				board->player2->rotate45l[abs(board->m - abs(line - column)) - 1]
				 |= (1ULL << (MIN(line , column) - 1));
			}
			else
			{
				board->player2->rotate45l[board->m + abs(line - column) - 1]
				 |= (1ULL << (MIN(line , column) - 1));
			}

			/* rotate 45r */
			board->player2->rotate45r[line + column - 2]
			 |= (1ULL << (column - 1));
		}

		return 1;

	}
	return -1;
}

int
board_get(const board_t* board, const int line, const int column)
{
	if (board == NULL)
		return -1;

	if ((line < 1)
	 || (column < 1)
	 || (line > board->n)
	 || (column > board->m))
	{
		return -1;
	}

	if ((board->player1->rotate0[line - 1] & (1ULL << (column - 1)))
	 == (1ULL << (column - 1)))
	{
		return 1;
	}
	else if ((board->player2->rotate0[line - 1] & (1ULL << (column - 1)))
	 == (1ULL << (column - 1)))
	{
		return 2;
	}

	return 0;
}

void
board_display(const board_t* board, FILE* stream)
{
	if (board !=  NULL)
	{
		int columns_width = floor(log10(board->m)) + 1;
		int first_column_width = floor(log10(board->n)) + 1;

		/* Print header */
		for (int i = 0; i < first_column_width; i++)
		{
			fprintf(stream, "%s", " ");
		}

		for (int i = 0; i < board->m; i++)
		{
			fprintf(stream, " %*i", columns_width, i + 1);
		}

		fputs("\n", stream);

		/* print lines */
		for (int i = 1; i <= board->n; i++)
		{
			fprintf(stream, "%*i", first_column_width, i);

			for (int j = 1; j <= board->m; j++)
			{
				if (board_get(board, i, j) == 1)
				{
					fprintf(stream, " %*c", columns_width, PLAYER1);
				}
				else if (board_get(board, i, j) == 2)
				{
					fprintf(stream, " %*c", columns_width, PLAYER2);
				}
				else
				{
					fprintf(stream, " %*c", columns_width, NONE);
				}
			}

			fputs("\n", stream);
			
		}
	}
}

int
board_win(const board_t* board, const int k)
{
	if (board == NULL)
		return -1;

	if ((k < 1) || ((k > board->m) && (k > board->n)))
	{
		return -1;
	}

	uint64_t win_mask = 0ULL;

	for (int i = 0; i < k; i++)
	{
		win_mask |= (1ULL << i);
	}

	int i = 0;
	while (i < board->m + board->n - 1)
	{
		/* Read horizontally */
		if (i < board->n)
		{
			for (int j = 0; j < board->n; j++)
			{
				if ((win_mask & board->player1->rotate0[j]) == win_mask)
				{
					return 1;
				}
				else if ((win_mask & board->player2->rotate0[j]) == win_mask)
				{
					return 2;
				}
			}
		}

		/* Read vertically */
		if (i < board->m)
		{
			for (int j = 0; j < board->m; j++)
			{
				if ((win_mask & board->player1->rotate90[j]) == win_mask)
				{
					return 1;
				}
				else if ((win_mask & board->player2->rotate90[j]) == win_mask)
				{
					return 2;
				}
			}
		}
	
		/* Read diagonally rotation left */
		for (int j = 0; j < board->m + board->n - 1; j++)
		{
			if ((win_mask & board->player1->rotate45l[j]) == win_mask)
			{
				return 1;
			}
			else if ((win_mask & board->player2->rotate45l[j]) == win_mask)
			{
				return 2;
			}
		}
	
		/* Read diagonally rotation right */
		for (int j = 0; j < board->m + board->n - 1; j++)
		{
			if ((win_mask & board->player1->rotate45r[j]) == win_mask)
			{
				return 1;
			}
			else if ((win_mask & board->player2->rotate45r[j]) == win_mask)
			{
				return 2;
			}
		}

		/* loop back */
		i++;
		win_mask = win_mask << 1;
	}

	return 0;
}

int
board_count_empty_cells(const board_t* board)
{
	if (board == NULL)
		return -1;

	int count = 0;

	for (int i = 1; i <= board->n; i++)
	{
		for (int j = 1; j <= board->m; j++)
		{
			if (board_get(board, i, j) == 0)
			{
				count++;
			}
		}
	}

	return count;
}

int
board_get_player(const board_t* board)
{
	int stone_P1 = 0;
	int stone_P2 = 0;

	for (int i = 1; i <= board->n; i++)
	{
		for (int j = 1; j <= board->m; j++)
		{
			if (board_get(board, i, j) == 1)
				stone_P1++;
			else if (board_get(board, i, j) == 2)
				stone_P2++;
		}
	}

	return (stone_P1 <= stone_P2) ? 1 : 2;
}