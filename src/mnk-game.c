#define _POSIX_C_SOURCE 200809L
#include "mnk-game.h"
#include "board.h"

#include <stdlib.h>
#include <stdbool.h>

#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

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
	   decimal2index("002") = 2
	   decimal2index("101") = 101  */
	return atoi(str);
}

static int
game(board_t* board)
{
	/* Mechanic of the game for a turn. */
	bool valid = false;

	printf("\nNumber of stones to align to win (k) = %i)\n\n", k);
	board_display(board, stdout);
	printf("\n");

	while (valid == false)
	{
		int player;

		/* Get player to play */
		int stone_X = 0;
		int stone_O = 0;

		for (int i = 1; i <= m; i++)
		{
			for (int j = 1; j <= n; j++)
			{
				if (board_get(board, i, j) == 1)
					stone_X++;
				else if (board_get(board, i, j) == 2)
					stone_O++;
			}
		}

		player = (stone_X <= stone_O) ? 1 : 2;

		/* Print state of board and prompt */
		printf("Player '%c' give your move 'line column' (eg: 1 2), "
			"press 'Q' to quit: ", (stone_X <= stone_O) ? PLAYER1 : PLAYER2);

		/* Read the input */
		char buffer[BUFFER_SIZE];
		char* arg;
		int arg_i = 0;

		fgets(buffer, sizeof(buffer), stdin);
		printf("\n");

		arg = strtok(buffer, " \t\n");

		/* Get arguments */
		int x, y;

		while(arg != NULL)
		{
			if ((str2index(arg) != 0) && (arg_i == 0))
				x = str2index(arg);
			else if ((str2index(arg) != 0) && (arg_i == 1))
			{
				y = str2index(arg);
				valid = true;
			}
			else if ((*arg == 'Q') || (*arg == 'q'))
			{
				board_delete(board);
				exit(EXIT_SUCCESS);
			}

			arg_i++;
			arg = strtok(NULL, " \n\t");

		}

		if (valid)
		{
			if (board_set(board, player, x, y) != 1)
			{
				printf("Invalid move\n");
			}
		}
		else
		{
			printf("Invalid syntaxe\n");
		}
	}

	return board_win(board, k);

}

board_t*
parse_input(int* player, FILE* input_file)
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
	board_t* board_tmp = board_new(BOARD_MAX, BOARD_MAX);
	int m_value = 0;
	int n_value = 0;
	int i = 1;
	int j = 1;
	int stones_p1 = 0;
	int stones_p2 = 0;

	char current_char;
	int ignore = false; /* in case of comment, ignore the rest of the line */
	int board_line = false; /* true when the line read had data from board */
	int scan_first_row = false; /* true if first row had been scanned */

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
						n_value = j - 1;
					}
					else
					{
						if (n_value != j - 1)
						{
							fprintf(stderr, "Invalid number of column at line "
								"%i\n", i);
							fclose(input_file);
							exit(EXIT_FAILURE);
						}
					}

					j = 1;
					i++;
					m_value++;

				}

				ignore = false;
				board_line = false;

			}
			else if ((current_char == stone_p1) && (!ignore))
			{
				stones_p1++;
				board_set(board_tmp, 1, i, j);
				j++;
				board_line = true;
			}
			else if ((current_char == stone_p2) && (!ignore))
			{
				stones_p2++;
				board_set(board_tmp, 2, i, j);
				j++;
				board_line = true;
			}
			else if ((current_char == '_') && (!ignore))
			{
				j++;
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
			n_value = j - 1;
		}
		else
		{
			if (n_value != j - 1)
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
	board_t* board = board_new(m, n);

	for (int i = 1; i <= board->m; i++)
	{
		for (int j = 1; j <= board->n; j++)
		{
			if (board_get(board_tmp, i, j) != 0)
			{
				board_set(board, board_get(board_tmp, i, j), i, j);
			}
		}
	}

	board_delete(board_tmp);

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

	if (board_win(board, k) != 0)
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

	*player = (stones_p1 == stones_p2) ? 1 : 2;

	board_display(board, stdout);

	return board;
}

move_t
play(int* player, board_t* board)
{
	/* Return a random free position to play */
	move_t position;
	bool valid = false;

	srand(time(NULL));
	while (!valid)
	{
		position.line = (rand() % m) + 1;
		position.column = (rand() % n) + 1;

		if (board_get(board, position.line, position.column) == 0)
		{
			valid = true;
		}
	}

	printf("Player %i will play :\n", *player);
	printf("<%i> <%i>\n", position.line, position.column);

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
			" -m, --set-m M       set the width m to M\n"
			" -n, --set-n N       set the height n to N\n"
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
				exit(EXIT_SUCCESS);
				break;

			case '1': /* player 1 as an AI option */
				printf("Unimplemented yet\n");
				exit(EXIT_SUCCESS);
				break;

			case '2': /* player 2 as an AI option */
				printf("Unimplemented yet\n");
				exit(EXIT_SUCCESS);
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
	if (contest)
	{
		/* Mode contest */
		int* current_player, p = 0;
		current_player = &p;

		board_t* board = parse_input(current_player, input_file);

		move_t position = play(current_player, board);

		board_set(board, *current_player, position.line, position.column);

		board_display(board, stdout);

		board_delete(board);
		fclose(input_file);
	}
	else
	{
		/* Game loop Hmn vs Hmn */
		board_t* board = board_new(m, n);

		while (board_count_empty_cells(board) > 0)
		{
			int winner = game(board);

			if (winner > 0)
			{
				board_display(board, stdout);
				printf("The winner is '%c' !\n", (winner == 1) ? PLAYER1 : PLAYER2);
				board_delete(board);
				exit(EXIT_SUCCESS);
			}
		}

		board_display(board, stdout);
		printf("No winner, this is a draw.\n");
		board_delete(board);
	}

	return EXIT_SUCCESS;
}
