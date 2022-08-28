#include "mnk-game.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <getopt.h>

/* Set default values */
static bool verbose = false;
static int m = 3;
static int n = 3;
static int k = 3;

static void usage(int status) {
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

static void version(void) {
	printf("mnk-game %i.%i.%i\n"
		"This software is an mnk-game with AI players.\n",
		PROG_VERSION, PROG_SUBVERSION, PROG_REVISION);
}

int main(int argc, char* argv[]) {
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
	return EXIT_SUCCESS;
}
