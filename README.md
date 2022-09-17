# m,n,k-game

### Description
An m,n,k-game is an abstract board game in which two players take turns in placing a stone of their color on an m-by-n board, the winner being the player who first gets k stones of their own color in a row, horizontally, vertically, or diagonally. Thus, tic-tac-toe is the 3,3,3-game and free-style gomoku is the 15,15,5-game. An m,n,k-game is also called a k-in-a-row game on an m-by-n board.\
source : [Wikipedia](https://en.wikipedia.org/wiki/M,n,k-game)

### Project structure
`src/mnk-game.c` contains the main function. How each turns work, the AI for the computer.\
`src/board.c` contains the logic of the board (i.e. coordinates of each stones for each players).

## Compiling & running
From the project's root folder, run:\
```
make clean && make
./mnk-game
```

## Usage
```
./mnk-game --help
Usage: mnk-game [OPTION] [-c FILE]
Allow to play to mnk-games with chosen values for m, n or k.

 -m, --set-m M       set the width m to M
 -n, --set-n N       set the height n to N
 -k, --set-k K       set the value of k to K
 -0, --all-ai        set both player to be an AI
 -1, --player1-ai    set first player as an AI
 -2, --player2-ai    set second player as an AI
 -c, --contest FILE  enable 'contest mode'
 -v, --verbose       verbose output
 -V, --version       display version and exit
 -h, --help          display this help
```
