#include "mnk-game.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>
#include <math.h>
#include <getopt.h>
#include <string.h>

/* Set default values */
static bool verbose = false;
static int m = 3;    /* Number of rows */
static int n = 3;    /* Number of columns */
static int k = 3;    /* Number of stones aligned to win */

/* Functions */
static int
str2index(char str[BUFFER_SIZE])
{
	/* Transform a string to decimal.
	   E.g:
	   decimal2index("002") = 1
	   decimal2index("101") = 100  */
	return atoi(str) - 1;
}

static char*
index2str(char buffer[BUFFER_SIZE], int size, int index)
{
	/* Transform a decimal to string of a given size.

	   E.g:
	   index2str(buffer, 3, 5) = "006"
	   index2str(buffer, 2, 10) = "11"  */
	int n = floor(log10(index)) + 1;    // Number of digit in index
	int f = 0;                          // Use for flag

	/* assert */
	assert(n <= size);
	assert(n <= BUFFER_SIZE);

	f = snprintf(buffer, size + 1, "%.*d", size, index + 1);

	if (f < 0)
		return NULL;

	return buffer;
}

static char**
board_alloc()
{
	/* Allocation of a board of size m by n */
	char** board = calloc(m, sizeof(char*));

	if (board == NULL)
	{
		/* Error in allocation */
		perror("Error allocation");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < m; i++)
	{
		board[i] = calloc(n, sizeof(char));
		if (board[i] == NULL)
		{
			/* Error in allocation */
			perror("Error allocation");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			board[i][j] = NONE;
		}
	}

	return board;
}

static void
board_free(char** board)
{
	/* Free the board */
	for (int i = 0; i < m; i++)
	{
		free(board[i]);
	}

	free(board);
}

static void
board_print(char** board)
{
	/* Print the state of the board */
	int digits_m = floor(log10(m)) + 1;    // Number of digit in m
	int digits_n = floor(log10(n)) + 1;    // Number of digit in n
	char buffer[BUFFER_SIZE];

	/* Header */
	for (int i = 0; i < digits_m; i++)
	{
		printf("%s", " ");
	}

	for (int j = 0; j < n; j++)
	{
		index2str(buffer, digits_n, j);
		printf(" %s", buffer);
	}

	printf("\n");

	/* Content */
	for (int i = 0; i < m; i++)
	{
		index2str(buffer, digits_m, i);
		printf("%s", buffer);

		for (int j = 0; j < n; j++)
		{
			for (int ind = 0; ind < digits_n; ind++)
			{
				printf("%s", " ");
			}

			printf("%c", board[i][j]);

		}

		printf("\n");

	}

	printf("\n");

}

static int
board_set(char stone, int i, int j, char** board)
{
	/* set a stone on board [i][j]
	   return 0 if valid, -1 otherwise. */
	if ((i < 0) || (j < 0) || (i >= m) ||(j >= n) || board[i][j] != NONE)
	{
		return -1;
	}
	else
	{
		/* Set the stone */
		board[i][j] = stone;
		// turn++
		return 0;
	}
}

static char
is_winner(char** board)
{
	/* Return the winner of the board, Character NONE otherwise. */
	int count;
	char winner = NONE;

	/* Read rows */
	for (int i = 0; i < m; i++)
	{
		count = 0;

		for (int j = 0; j < n; j++)
		{
			char cursor = board[i][j];

			if (cursor == PLAYER1)
			{
				if (winner == PLAYER1)
				{
					count++;
				}
				else
				{
					count = 1;
					winner = PLAYER1;
				}
			}
			else if (cursor == PLAYER2)
			{

				if (winner == PLAYER2)
					count++;
				else
				{
					count = 1;
					winner = PLAYER2;
				}

			}
			else
				count = 0;

			if (count == k)
			{
				printf("Test: Row\n");
				return winner;
			}
		}
	}

	/* Read columns */
	for (int j = 0; j < n; j++)
	{
		count = 0;

		for (int i = 0; i < m; i++)
		{
			char cursor = board[i][j];

			if (cursor == PLAYER1)
			{
				if (winner == PLAYER1)
					count++;
				else
				{
					count = 1;
					winner = PLAYER1;
				}
			}
			else if (cursor == PLAYER2)
			{

				if (winner == PLAYER2)
					count++;
				else
				{
					count = 1;
					winner = PLAYER2;
				}

			}
			else
				count = 0;

			if (count == k)
				return winner;
		}
	}

	/* Read diagonally (from upper right to bottom left) */
	for (int i = 0; i < m - k + 1; i++)
	{
		count = 0;

		for (int j = k - 1; j > n; j++)
		{
			char cursor = board[i][j];

			if (cursor == NONE)
				count = 0;
			else
			{
				winner = cursor;
				for (int l = 0; l < k; l++)
				{
					if (board[i+l][j+l] == winner)
						count++;
					else
						count = 0;
				}

				if (count == k)
					return winner;
			}
		}
	}

	/* Read diagonally (from upper left to bottom right) */
	for (int i = 0; i < m - k + 1; i++)
	{
		count = 0;

		for (int j = n - k + 1; j > n; j++)
		{
			char cursor = board[i][j];

			if (cursor == NONE)
				count = 0;
			else
			{
				winner = cursor;
				for (int l = 0; l < k; l++)
				{
					if (board[i+l][j+l] == winner)
						count++;
					else
						count = 0;
				}

				if (count == k)
					return winner;
			}
		}
	}

	return NONE;
}

static char
game(char** board)
{
	/* Mechanic of the game for a turn. */
	bool valid = false;

	printf("\nNumber of stones to align to win (k) = %i)\n\n", k);
	board_print(board);

	while (valid == false)
	{
		char player;

		/* Get player to play */
		int stone_X = 0;
		int stone_O = 0;

		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (board[i][j] == PLAYER1)
					stone_X++;
				else if (board[i][j] == PLAYER2)
					stone_O++;
			}
		}

		player = (stone_X <= stone_O) ? PLAYER1 : PLAYER2;

		/* Print state of board and prompt */
		printf("Player '%c' give your move 'line column' (eg: 1 2), "
			"press 'Q' to quit: ", player);

		/* Read the input */
		char buffer[BUFFER_SIZE];
		char* arg;
		int arg_i = 0;

		fgets(buffer, sizeof(buffer), stdin);
		printf("\n");

		arg = strtok(buffer, " ");

		/* Get arguments */
		int x, y;

		while(arg != NULL)
		{
			if ((str2index(arg) != -1) && (arg_i == 0))
				x = str2index(arg);
			else if ((str2index(arg) != -1) && (arg_i == 1))
			{
				y = str2index(arg);
				valid = true;
			}
			else if ((*arg == 'Q') || (*arg == 'q'))
			{
				board_free(board);
				exit(EXIT_SUCCESS);
			}

			arg_i++;
			arg = strtok(NULL, " ");

		}

		if (valid)
		{
			if (board_set(player, x, y, board) == 0)
			{
				if (player == PLAYER1)
					player = PLAYER2;
				else
					player = PLAYER1;
			}
			else
				printf("Invalid move\n");
		}
		else
			printf("Invalid syntaxe\n");
	}

	return is_winner(board);

}

static void
usage(int status) {
	if (status == EXIT_SUCCESS) {
		fputs(
			"Usage: mnk-game [OPTION] [-c FILE]\n"
			"Allow to play to mnk-games with chosen values for m, n or k.\n"
			"\n"
			" -m, --set-m M       set the value of m to M\n"
			" -n, --set-n N       set the value of n to N\n"
			" -k, --set-k K       set the value of k to K\n"
			" -0, --all-ai        set both player to be an AI\n"
			" -1, --player1-ai    set first player as an AI\n"
			" -2, --player2-ai    set second player as an AI\n"
			" -c, --contest FILE  enable 'contest mode'\n"
			" -v, --verbose       verbose output\n"
			" -V, --version       display version and exit\n"
			" -h, --help          display this help\n"
			"\n", stdout);
	} else {
		fprintf(stderr, "Try 'mnk-game --help' for more information.\n");
	}
	exit(status);
}

static void
version(void) {
	printf("mnk-game %i.%i.%i\n"
		"This software is an mnk-game with AI players.\n",
		PROG_VERSION, PROG_SUBVERSION, PROG_REVISION);
}

/* Main */
int
main(int argc, char* argv[]) {
	int optc = 0;
	
	static struct option long_opts[] = {
		{"set-m",      required_argument, NULL, 'm'},
		{"set-n",      required_argument, NULL, 'n'},
		{"set-k",      required_argument, NULL, 'k'},
		{"all-ai",     no_argument,       NULL, '0'},
		{"player1-ai", no_argument,       NULL, '1'},
		{"player2-ai", no_argument,       NULL, '2'},
		{"contest",    required_argument, NULL, 'c'},
		{"verbose",    no_argument,       NULL, 'v'},
		{"version",    no_argument,       NULL, 'V'},
		{"help",       no_argument,       NULL, 'h'},
		{NULL, 0, NULL, 0}
	};
	
	while ((optc =
		getopt_long(argc, argv, "m:n:k:012c:vVh", long_opts, NULL)) != -1) {
		switch(optc) {
			case 'm': /* 'm' option */
				if ((atoi(optarg) > 0) && (atoi(optarg) <= BOARD_MAX)) {
					m = atoi(optarg);
				} else {
					fprintf(stderr,
						"Argument invalid, -m must be between 1 and %i\n",
						BOARD_MAX);
					exit(EXIT_FAILURE);
				}
				break;

			case 'n': /* 'n' option */
				if ((atoi(optarg) > 0) && (atoi(optarg) <= BOARD_MAX)) {
					n = atoi(optarg);
				} else {
					fprintf(stderr,
						"Argument invalid, -n must be between 1 and %i\n",
						BOARD_MAX);
					exit(EXIT_FAILURE);
				}
				break;

			case 'k': /* 'k' option */
				if ((atoi(optarg) > 0) && (atoi(optarg) <= BOARD_MAX)) {
					k = atoi(optarg);
				} else {
					fprintf(stderr,
						"Argument invalid, -k must be between 1 and %i\n",
						BOARD_MAX);
					exit(EXIT_FAILURE);
				}
				break;

			case '0': /* AI vs AI option */
				printf("Unimplemented yet\n");
				break;

			case '1': /* player 1 as an AI option */
				printf("Unimplemented yet\n");
				break;

			case '2': /* player 2 as an AI option */
				printf("Unimplemented yet\n");
				break;

			case 'c': /* contest option */
				printf("Unimplemented yet\n");
				break;

			case 'v': /* verbose option */
				verbose = true;
				break;

			case 'V': /* version option */
				version();
				exit(EXIT_SUCCESS);

			case 'h': /* help option */
				usage(EXIT_SUCCESS);
				break;

			default:
				usage(EXIT_FAILURE);
		}
	}

	/* Game loop */
	char** board = board_alloc();
	int turn = 0;

	while(turn < m * n)
	{
		char winner = game(board);

		if (winner != NONE)
		{
			board_print(board);
			printf("The winner is '%c' !\n", winner);
			board_free(board);
			exit(EXIT_SUCCESS);
		}

		turn++;

	}

	board_print(board);
	printf("No winner, this is a draw.\n");
	board_free(board);

	return EXIT_SUCCESS;
}
