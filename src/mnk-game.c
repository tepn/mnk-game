#define _POSIX_C_SOURCE 200809L
#include "mnk-game.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <aio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct
{
	unsigned int line;
	unsigned int column;
} move_t;

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

char**
parse_input(char* player, FILE* input_file)
{
	/* Parse input_file file.
	First get 'k'.
	Then get the character used for each player.
	Finally retrieve the state of the board. */

	const char comment = '#';
	const char* whitespaces = " \t\n";
	int k_value = -1;
	char stone_p1, stone_p2;

	/* Read header */
	char* line = NULL;
	size_t len = 0;

	/* Retrieve 'k' */
	while ((getline(&line, &len, input_file)) != -1)
	{
		char* cursor = strtok(line, whitespaces);

		if ((cursor != NULL) && (*cursor != comment))
		{
			int length = strlen(cursor);

			for (int i = 0; i < length; i++)
			{
				if (!isdigit(cursor[i]))
				{
					fprintf(stderr, "Invalid value for k = %s.\n", cursor);
					free(line);
					fclose(input_file);
					exit(EXIT_FAILURE);
				}
			}

			k_value = atoi(cursor);
			break;

		}

	}

	if (k_value == -1)
	{
		fprintf(stderr, "No value found for k\n");
		free(line);
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if (verbose)
	{
		printf("Read header from file:\n");
		printf("k = %i\n", k_value);
	}

	/* Retrieve Character for player 1 and 2 */
	while ((getline(&line, &len, input_file)) != -1)
	{
		char* cursor = strtok(line, whitespaces);

		if ((cursor != NULL) && (*cursor != comment))
		{
			if (strlen(cursor) != 1)
			{
				fprintf(stderr, "Error\n");
				free(line);
				fclose(input_file);
				exit(EXIT_FAILURE);
			}

			stone_p1 = cursor[0];

			cursor = strtok(NULL, whitespaces);

			if (strlen(cursor) != 1)
			{
				fprintf(stderr, "Error\n");
				free(line);
				fclose(input_file);
				exit(EXIT_FAILURE);
			}

			stone_p2 = cursor[0];

			break;

		}

	}

	free(line);

	/* Validate stones for players */
	if ((stone_p1 == '_') || (stone_p2 == '_') || (stone_p1 == stone_p2))
	{
		fprintf(stderr, "Invalid stone for players\n");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if (verbose)
	{
		printf("P1 : '%c'\n", stone_p1);
		printf("P2 : '%c'\n", stone_p2);
	}

	/* Scan Grid */
	char board_tmp[BOARD_MAX][BOARD_MAX]; /* temporary board to track stones */
	int m_value = 0;
	int n_value = 0;
	int i = 0;
	int j = 0;
	int stones_p1 = 0;
	int stones_p2 = 0;

	char current_char;
	int ignore = false; /* in case of comment, ignore the rest of the line */
	int board_line = false; /* true when the line read had data from board */
	int scan_first_row = false; /* true if first row had been scanned */

	/* initialize temporary board to empty board */
	for (int i = 0; i < BOARD_MAX; i++)
	{
		for (int j = 0; j < BOARD_MAX; j++)
		{
			board_tmp[i][j] = NONE;
		}
	}

	while((current_char = fgetc(input_file)) != EOF)
	{
		switch(current_char)
		{
			/* filter out blank character */
		case ' ':
		case '\t':
			break;

		default:
			/* character treatment here */
			if (current_char == comment)
			{
				ignore = true;
			}
			else if (current_char == '\n')
			{
				if (board_line)
				{
					if (!scan_first_row)
					{
						scan_first_row = true;
						n_value = j;
					}
					else
					{
						if (n_value != j)
						{
							fprintf(stderr, "Invalid number of column at line "
								"%i\n", i);
							fclose(input_file);
							exit(EXIT_FAILURE);
						}
					}

					j = 0;
					i++;
					m_value++;

				}

				ignore = false;
				board_line = false;

			}
			else if ((current_char == stone_p1) && (!ignore))
			{
				stones_p1++;
				board_tmp[i][j] = PLAYER1;
				j++;
				board_line = true;
			}
			else if ((current_char == stone_p2) && (!ignore))
			{
				stones_p2++;
				board_tmp[i][j] = PLAYER2;
				j++;
				board_line = true;
			}
			else if ((current_char == '_') && (!ignore))
			{
				j++;
				board_tmp[i][j] = NONE;
				board_line = true;
			}
			else if (!ignore)
			{
				fprintf(stderr, "Invalid character '%c' at line %i\n",
					current_char, i);
				fclose(input_file);
				exit(EXIT_FAILURE);
			}

			break;
		}
	}

	if (board_line)
	{
		if (!scan_first_row)
		{
			n_value = j;
		}
		else
		{
			if (n_value != j)
			{
				fprintf(stderr, "Invalid number of column at line %i\n", i);
				fclose(input_file);
				exit(EXIT_FAILURE);
			}
		}

		m_value++;

	}

	/* Check information */
	if ((m_value > BOARD_MAX) || (n_value > BOARD_MAX))
	{
		fprintf(stderr, "'m' or 'n' exceed the maximum value %i.\n", BOARD_MAX);
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if ((k <= 0) || ((k > m_value) && (k > n_value)))
	{
		fprintf(stderr, "'k' = %i is invalid.\n", k_value);
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	m = m_value;
	n = n_value;
	k = k_value;

	if (verbose)
	{
		printf("m = %i\n", m);
		printf("n = %i\n", n);
		printf("\n");
	}

	/* place recorded stones on board */
	char** board = board_alloc();

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			board[i][j] = board_tmp[i][j];
		}
	}

	/* Scanning state of grid */
	if ((m_value == 0) || (n_value == 0))
	{
		fprintf(stderr, "Board unreadable.\n");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if (stones_p1 < stones_p2)
	{
		fprintf(stderr, "Missing stones for player 1.\n");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if (stones_p1 > stones_p2 + 1)
	{
		fprintf(stderr, "Missing stones for player 2.\n");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if (is_winner(board) != NONE)
	{
		fprintf(stderr, "A player already won the game\n");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	if (m * n == stones_p1 + stones_p2)
	{
		fprintf(stderr, "This is a drawed game.\n");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}

	player[0] = (stones_p1 == stones_p2) ? PLAYER1 : PLAYER2;
	board_print(board);

	return board;
}

move_t
play(int player, char** board)
{
	/* Return a random free position to play */
	move_t position;
	bool valid = false;

	srand(time(NULL));
	while (!valid)
	{
		position.line = (rand() % m);
		position.column = (rand() % n);

		if (board[position.line][position.column] == NONE)
		{
			valid = true;
		}
	}

	printf("Player %c will play :\n", player);
	printf("<%i> <%i>\n", position.line + 1, position.column + 1);

	return position;
}

static void
usage(int status)
{
	if (status == EXIT_SUCCESS)
	{
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
	}
	else
	{
		fprintf(stderr, "Try 'mnk-game --help' for more information.\n");
	}
	exit(status);
}

static void
version(void)
{
	printf("mnk-game %i.%i.%i\n"
		"This software is an mnk-game with AI players.\n",
		PROG_VERSION, PROG_SUBVERSION, PROG_REVISION);
}

/* Main */
int
main(int argc, char* argv[])
{
	int optc = 0;
	int contest = false;
	FILE* input_file = NULL;
	
	static struct option long_opts[] =
	{
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
		getopt_long(argc, argv, "m:n:k:012c:vVh", long_opts, NULL)) != -1)
	{
		switch(optc)
		{
			case 'm': /* 'm' option */
				if ((atoi(optarg) > 0) && (atoi(optarg) <= BOARD_MAX))
				{
					m = atoi(optarg);
				}
				else
				{
					fprintf(stderr,
						"Argument invalid, -m must be between 1 and %i\n",
						BOARD_MAX);
					exit(EXIT_FAILURE);
				}
				break;

			case 'n': /* 'n' option */
				if ((atoi(optarg) > 0) && (atoi(optarg) <= BOARD_MAX))
				{
					n = atoi(optarg);
				}
				else
				{
					fprintf(stderr,
						"Argument invalid, -n must be between 1 and %i\n",
						BOARD_MAX);
					exit(EXIT_FAILURE);
				}
				break;

			case 'k': /* 'k' option */
				if ((atoi(optarg) > 0) && (atoi(optarg) <= BOARD_MAX))
				{
					k = atoi(optarg);
				}
				else
				{
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
				input_file = fopen(optarg, "r+");

				if (input_file == NULL)
				{
					fprintf(stderr, "An error occured while opening the file : "
						"'%s\n",
						optarg);
					exit(EXIT_FAILURE);
				}
				contest = true;
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

	/* Validate arguments */
	if ((k > m) && (k > n))
	{
		fprintf(stderr, "Argument invalid : -k can't be greater than m or n.\n"
			"Try 'mnk-game --help' for more information.\n");
		exit(EXIT_FAILURE);
	}

	/* Game */
	char** board;
	if (contest)
	{
		char current_player[BUFFER_SIZE];

		/* Mode contest */
		board = parse_input(current_player, input_file);

		move_t position = play(*current_player, board);

		board_set(*current_player, position.line, position.column, board);

		board_print(board);

		board_free(board);
		fclose(input_file);
	}
	else
	{
		/* Game loop Hmn vs Hmn */
		board = board_alloc();
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
	}

	return EXIT_SUCCESS;
}
