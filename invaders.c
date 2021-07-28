/*
 * Invaders
 * By André Luiz de Oliveira Vasconcelos
 * https://alovasconcelos.com.br
 * alovasconcelos@gmail.com
 * 2021
 */

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#define BULLET_COLOR 1
#define ENEMY_COLOR 2
#define CANNON_COLOR 3
#define SCORE_COLOR 4
#define LEVEL_COLOR 5
#define SHOT_NOT_FIRED 0
#define SHOT_FIRED 1
#define ENEMY_KILLED 0
#define ENEMY_ALIVE 1
#define CANNON_LINE 19
#define GAME_OVER 0
#define KEEP_RUNING 1

// Cannon
char *cannon = "^-^";

// Enemy
char *enemy = ">o<";

// Bullet
char *bullet = "^";

// Explosion
char *explosionParts[5] = {	"***", ".*.", " * ", " . ", "   " };

WINDOW * mainwin;
int key;

// Level number
int level = 0;

// Flag to control the game loop
int keepRunning = 1;

// Cannon horizontal position
int cannonCol;

// Enemies horizontal position
int enemiesCol;

// Enemies vertical position
int enemiesRow;

// Enemies direction
int enemiesDirection;

// Flags indicating fired bullets
int shots[3];

// Bullets positions
int bulletRow[3];
int bulletCol[3];

// Score
int score = 0;

// Counter used to control enemies moves
int tick;


// Array of enemies status - 1:alive 0:killed
int enemiesStatus[28];

// Number of killed enemies
int killedEnemies;

/**
 * 	Print string at specified coordinate
 */
void printAtRowCol(int row, int col, char *str) {
	mvaddstr(row, col, str);
}

/**
 * Update the score
 */
void updateScore() {	
	char buffer[12];
	snprintf(buffer, 12,"%d",score);
	attron(COLOR_PAIR(SCORE_COLOR));
	printAtRowCol(1, 8, buffer);
	attroff(COLOR_PAIR(SCORE_COLOR));
}

/**
 * Draw the opening screen
 */
void openingScreen() {
	for(int line=5; line<12; line++) {
		printAtRowCol(line, 1,"                                                          ");
		printAtRowCol(line + 1, 1,"###                                                       ");
		printAtRowCol(line + 2, 1," #   #    #  #    #    ##    #####   ######  #####   #### ");
		printAtRowCol(line + 3, 1," #   ##   #  #    #   #  #   #    #  #       #    # #     ");
		printAtRowCol(line + 4, 1," #   # #  #  #    #  #    #  #    #  #####   #    #  #### ");
		printAtRowCol(line + 5, 1," #   #  # #  #    #  ######  #    #  #       #####       #");
		printAtRowCol(line + 6, 1," #   #   ##   #  #   #    #  #    #  #       #   #  #    #");
		printAtRowCol(line + 7, 1,"###  #    #    ##    #    #  #####   ######  #    #  #### ");
		refresh();
		usleep(100000);
	}
}

/**
 * Clear the game board 
 */
void clearScreen() {
	for (int line = 3; line < 20; line++) {
		printAtRowCol(line,  1, "                                                          ");	
	}
	refresh();
}

/**
 * Draw the game board
 */
void drawScreen() {
	printAtRowCol(0 ,  0, "+----------------------------------------------------------+");
	printAtRowCol(1 ,  0, "|SCORE:              I N V A D E R S               LEVEL:  |");
	printAtRowCol(2 ,  0, "+----------------------------------------------------------+");
	printAtRowCol(3 ,  0, "|                                                          |");
	printAtRowCol(4 ,  0, "|                                                          |");
	printAtRowCol(5 ,  0, "|                                                          |");
	printAtRowCol(6 ,  0, "|                                                          |");
	printAtRowCol(7 ,  0, "|                                                          |");
	printAtRowCol(8 ,  0, "|                                                          |");
	printAtRowCol(9 ,  0, "|                                                          |");
	printAtRowCol(10 , 0, "|                                                          |");
	printAtRowCol(11 , 0, "|                                                          |");
	printAtRowCol(12 , 0, "|                                                          |");
	printAtRowCol(13 , 0, "|                                                          |");
	printAtRowCol(14 , 0, "|                                                          |");
	printAtRowCol(15 , 0, "|                                                          |");
	printAtRowCol(16 , 0, "|                                                          |");
	printAtRowCol(17 , 0, "|                                                          |");
	printAtRowCol(18 , 0, "|                                                          |");
	printAtRowCol(19 , 0, "|                                                          |");
	printAtRowCol(20 , 0, "+----------------------------------------------------------+");
	printAtRowCol(21 , 0, "|                                                          |");
	printAtRowCol(22 , 0, "|                                                          |");
	printAtRowCol(23 , 0, "|                                                          |");
	printAtRowCol(24 , 0, "+----------------------------------------------------------+");
}

/**
 * Show help bar
 */
void showHelp() {
	printAtRowCol(21 , 1, "Use left and right arrows keys to move your cannon");
	printAtRowCol(22 , 1, "Press space to shoot");
	printAtRowCol(23 , 1, "Press 'End' key to quit the game");
	refresh();
}

/**
 * Get character at the specified coordinate
*/
char getCharRowCol(int row, int col) {
	return mvinch(row, col) & A_CHARTEXT;
}

/**
 * Draw the cannon
 */
void drawCannon() {
	attron(COLOR_PAIR(CANNON_COLOR));
	printAtRowCol(CANNON_LINE, cannonCol, cannon);
	printAtRowCol(CANNON_LINE, cannonCol + 1, "");
	attroff(COLOR_PAIR(CANNON_COLOR));
}

/**
 * Clear the cannon
 */
void clearCannon() {
	printAtRowCol(CANNON_LINE, cannonCol, "   ");
}

/**
 * Fire bullets
 */
void fire() {
	for (int bulletNumber = 0; bulletNumber < 3; bulletNumber++) {
		if (shots[bulletNumber] == SHOT_NOT_FIRED) {
			// fire bullet
			bulletCol[bulletNumber] = cannonCol + 1;
			shots[bulletNumber] = SHOT_FIRED;		
			return;
		}
	}
}

/**
 * Game over
 */
void gameOver() {
 	printAtRowCol(5 ,  1, "               ####     ##    #    #  ######              ");
	printAtRowCol(6 ,  1, "              #    #   #  #   ##  ##  #                   ");
	printAtRowCol(7 ,  1, "              #       #    #  # ## #  #####               ");
	printAtRowCol(8 ,  1, "              #  ###  ######  #    #  #                   ");
	printAtRowCol(9 ,  1, "              #    #  #    #  #    #  #                   ");
	printAtRowCol(10 , 1, "               ####   #    #  #    #  ######              ");
	printAtRowCol(13 , 1, "               ####   #    #  ######  #####               ");
	printAtRowCol(14 , 1, "              #    #  #    #  #       #    #              ");
	printAtRowCol(15 , 1, "              #    #  #    #  #####   #    #              ");
	printAtRowCol(16 , 1, "              #    #  #    #  #       #####               ");
	printAtRowCol(17 , 1, "              #    #   #  #   #       #   #               ");
	printAtRowCol(18 , 1, "               ####     ##    ######  #    #              ");
	printAtRowCol(23 , 1, "By: André Vasconcelos        https://alovasconcelos.com.br");
	refresh();
}


/**
 * Draw enemies
 */
int drawEnemies() {
	int enemyNumber = 0;
	for(int row = enemiesRow; row < enemiesRow + 4; row++) {
		for(int col = enemiesCol; col < enemiesCol + 37; col += 6) {
			if (enemiesStatus[enemyNumber]) {
				attron(COLOR_PAIR(ENEMY_COLOR));
				printAtRowCol(row, col, enemy);
				attron(COLOR_PAIR(ENEMY_COLOR));
				if (row == CANNON_LINE) {
					// Game over
					return GAME_OVER;
				}
			}else{
				printAtRowCol(row, col, "   ");
			}
			enemyNumber++;
			refresh();
		}
	}
	return KEEP_RUNING;
}

/**
 * Reset enemies status
 */
void resetEnemiesStatus() {
	for(int enemyNumber=0; enemyNumber<28; enemyNumber++) {
		enemiesStatus[enemyNumber] = ENEMY_ALIVE;
	}
}

/**
 * Reset the bullets status
 */
void resetBulletsStatus() {
	for(int bulletNumber=0; bulletNumber<3; bulletNumber++) {
		shots[bulletNumber] = SHOT_NOT_FIRED;
		bulletRow[bulletNumber] = CANNON_LINE -1;
		bulletCol[bulletNumber] = CANNON_LINE -1;
	}
}

/**
 * Show level information
 */
void showLevelOpening() {
 	printAtRowCol(3 ,  1, "          #       ######  #    #  ######  #               ");
	printAtRowCol(4 ,  1, "          #       #       #    #  #       #               ");
	printAtRowCol(5 ,  1, "          #       #####   #    #  #####   #               ");
	printAtRowCol(6 ,  1, "          #       #       #    #  #       #               ");
	printAtRowCol(7 ,  1, "          #       #        #  #   #       #               ");
	printAtRowCol(8 ,  1, "          ######  ######    ##    ######  ######          ");	

	switch (level)
	{
		case 1:
			printAtRowCol(10 , 1, "                            #                             ");	
			printAtRowCol(11 , 1, "                           ##                             ");	
			printAtRowCol(12 , 1, "                          # #                             ");	
			printAtRowCol(13 , 1, "                            #                             ");	
			printAtRowCol(14 , 1, "                            #                             ");	
			printAtRowCol(15 , 1, "                            #                             ");	
			printAtRowCol(16 , 1, "                          #####                           ");	
			break;
		case 2:
			printAtRowCol(10 , 1, "                          #####                           ");	
			printAtRowCol(11 , 1, "                         #     #                          ");	
			printAtRowCol(12 , 1, "                               #                          ");	
			printAtRowCol(13 , 1, "                          #####                           ");	
			printAtRowCol(14 , 1, "                         #                                ");	
			printAtRowCol(15 , 1, "                         #                                ");	
			printAtRowCol(16 , 1, "                         #######                          ");	
			break;
		case 3:
			printAtRowCol(10 , 1, "                          #####                           ");	
			printAtRowCol(11 , 1, "                         #     #                          ");	
			printAtRowCol(12 , 1, "                               #                          ");	
			printAtRowCol(13 , 1, "                          #####                           ");	
			printAtRowCol(14 , 1, "                               #                          ");	
			printAtRowCol(15 , 1, "                         #     #                          ");	
			printAtRowCol(16 , 1, "                          #####                           ");	
			break;
		case 4:
			printAtRowCol(10 , 1, "                         #                                ");	
			printAtRowCol(11 , 1, "                         #    #                           ");	
			printAtRowCol(12 , 1, "                         #    #                           ");	
			printAtRowCol(13 , 1, "                         #######                          ");	
			printAtRowCol(14 , 1, "                              #                           ");	
			printAtRowCol(15 , 1, "                              #                           ");	
			printAtRowCol(16 , 1, "                              #                           ");	
			break;
		case 5:
			printAtRowCol(10 , 1, "                        #######                           ");	
			printAtRowCol(11 , 1, "                        #                                 ");	
			printAtRowCol(12 , 1, "                        #                                 ");	
			printAtRowCol(13 , 1, "                         #####                            ");	
			printAtRowCol(14 , 1, "                              #                           ");	
			printAtRowCol(15 , 1, "                        #     #                           ");	
			printAtRowCol(16 , 1, "                         #####                            ");	
			break;
		case 6:
			printAtRowCol(10 , 1, "                         #####                            ");	
			printAtRowCol(11 , 1, "                        #     #                           ");	
			printAtRowCol(12 , 1, "                        #                                 ");	
			printAtRowCol(13 , 1, "                        ######                            ");	
			printAtRowCol(14 , 1, "                        #     #                           ");	
			printAtRowCol(15 , 1, "                        #     #                           ");	
			printAtRowCol(16 , 1, "                         #####                            ");	
			break;
		case 7:
			printAtRowCol(10 , 1, "                        #######                           ");	
			printAtRowCol(11 , 1, "                        #    #                            ");	
			printAtRowCol(12 , 1, "                            #                             ");	
			printAtRowCol(13 , 1, "                           #                              ");	
			printAtRowCol(14 , 1, "                          #                               ");	
			printAtRowCol(15 , 1, "                          #                               ");	
			printAtRowCol(16 , 1, "                          #                               ");	
			break;
		case 8:
			printAtRowCol(10 , 1, "                         #####                            ");	
			printAtRowCol(11 , 1, "                        #     #                           ");	
			printAtRowCol(12 , 1, "                        #     #                           ");	
			printAtRowCol(13 , 1, "                         #####                            ");	
			printAtRowCol(14 , 1, "                        #     #                           ");	
			printAtRowCol(15 , 1, "                        #     #                           ");	
			printAtRowCol(16 , 1, "                         #####                            ");	
			break;
		case 9:
			printAtRowCol(10 , 1, "                         #####                            ");	
			printAtRowCol(11 , 1, "                        #     #                           ");	
			printAtRowCol(12 , 1, "                        #     #                           ");	
			printAtRowCol(13 , 1, "                         ######                           ");	
			printAtRowCol(14 , 1, "                              #                           ");	
			printAtRowCol(15 , 1, "                        #     #                           ");	
			printAtRowCol(16 , 1, "                         #####                            ");	
			break;
	}

	refresh();
}

/**
 * Start the next level
 */
void nextLevel() {
	level++;
	cannonCol = 28;	
	enemiesCol = 10;
	enemiesRow = 5;
	enemiesDirection = 1;

	tick = 0;
	killedEnemies = 0;
	resetEnemiesStatus();
	resetBulletsStatus();

	// Show help bar
	showHelp();

	// Show level
	showLevelOpening();

	// Wait 2 seconds
	sleep(2);

	// Hide level information
	clearScreen();

	// Show level
	char buffer[12];
	snprintf(buffer, 12,"%d",level);
	attron(COLOR_PAIR(LEVEL_COLOR));
	printAtRowCol(1, 58, buffer);
	attroff(COLOR_PAIR(LEVEL_COLOR));
}

/**
 * Restart the game
 */
void restartGame() {
	level = 0;
	score = 0;
}

/**
 * Clear enemies
 */
void clearEnemies() {
	for(int row = enemiesRow; row < enemiesRow + 4; row++) {
		for(int col = enemiesCol; col < enemiesCol + 37; col+= 6) {
			printAtRowCol(row, col, "   ");
			refresh();
		}
	}
}

/**
 * Draw the fired bullets
 */
void drawBullets() {
	attron(COLOR_PAIR(BULLET_COLOR));
	for (int bulletNumber = 0; bulletNumber < 3; bulletNumber++) {
		if (shots[bulletNumber] == SHOT_FIRED) {
			if (bulletRow[bulletNumber] < CANNON_LINE - 1) {
				printAtRowCol(bulletRow[bulletNumber] + 1, bulletCol[bulletNumber], " ");
			}
			printAtRowCol(bulletRow[bulletNumber], bulletCol[bulletNumber], bullet);
			bulletRow[bulletNumber]--;	
			if (bulletRow[bulletNumber] == 2) {
				printAtRowCol(bulletRow[bulletNumber] + 1, bulletCol[bulletNumber], " ");
				shots[bulletNumber] = SHOT_NOT_FIRED;
				bulletRow[bulletNumber] = CANNON_LINE - 1;
			}
		}
	}
	attroff(COLOR_PAIR(BULLET_COLOR));
}

/**
 * Finish the game
 */
void endGame() {
	printAtRowCol(21 , 1, "Do you really want to quit the game?                      ");
	printAtRowCol(22 , 1, "    Press Y to confirm                                    ");
	printAtRowCol(23 , 1, "    Or N to keep playing                                  ");
    int ch;
	while (ch != 'Y' && ch != 'y' && ch != 'N' && ch != 'n') {
		ch = wgetch(mainwin);
	}
              
    if(ch == 'y' || ch == 'Y') {
		keepRunning = GAME_OVER;
    }else{
		showHelp();
	}
}

/**
 * ExplodeEnemy
 */
void explodeEnemy(int row, int col) {
	for(int i = 0 ; i < 5; i++) {
		printAtRowCol(row, col, explosionParts[i]);
		usleep(8000);
		refresh();
	}
}

/**
 * Check if theres an enemy at a given position
 */
int isEnemy(int row, int col) {
	int enemyNumber = 0;
	for(int enemyRow = enemiesRow; enemyRow < enemiesRow + 4; enemyRow++) {
		for(int enemyCol = enemiesCol; enemyCol < enemiesCol + 37; enemyCol += 6) {
			if (enemiesStatus[enemyNumber]) {
				if (row == enemyRow && col >= enemyCol && col <= enemyCol + 2) {
					// Enemy has been hit
					enemiesStatus[enemyNumber] = ENEMY_KILLED;
					explodeEnemy(enemyRow, enemyCol);
					killedEnemies++;
					score += enemyRow;
					return 1;
				}
			}
			enemyNumber++;
		}
	}
	return 0;
}
 
/**
 * Update enemies
 */
int updateEnemies() {

	int r = rand() % 100;

	if (tick++ % (8 - level) == 0) {
		if (r > 50) {
			clearEnemies();
			enemiesCol += enemiesDirection;
		}
		if (r < 10) {		
			clearEnemies();
			enemiesRow++;
		}
	}
	if(drawEnemies() == GAME_OVER){
		return GAME_OVER;
	}

	if (enemiesCol == CANNON_LINE) {
		enemiesDirection = -1;
	}
	if (enemiesCol == 1) {
		enemiesDirection = 1;
	}
	return KEEP_RUNING;
}

/**
 * Destroy enemy
 */
void destroyEnemy(int row, int col) {
	if (getCharRowCol(row, col) == '>') {
		printAtRowCol(row, col, "   ");		
	}
	if (getCharRowCol(row, col) == 'o') {
		printAtRowCol(row, col-1, "   ");		
	}
	if (getCharRowCol(row, col) == '<') {
		printAtRowCol(row, col-2, "   ");		
	}
}

/**
 * Destroy bullet
 */
void destroyBullet(int row, int col) {
	printAtRowCol(row, col, " ");
}

/**
 * Reset fire
 */
void resetFire(int bulletNumber) {	
	destroyBullet(bulletRow[bulletNumber] + 1, bulletCol[bulletNumber]);
	shots[bulletNumber] = SHOT_NOT_FIRED;
	bulletRow[bulletNumber] = CANNON_LINE -1;
}

/**
 * Check if the target has been hit
 */
void checkTargetHit() {	
	for(int bulletNumber = 0; bulletNumber < 3; bulletNumber++) {
		if (shots[bulletNumber]) {
			// bullet is fired
			if (isEnemy(bulletRow[bulletNumber], bulletCol[bulletNumber])) {
				destroyEnemy(bulletRow[bulletNumber], bulletCol[bulletNumber]);
				resetFire(bulletNumber);
			}
		}
	}
}

/**
 * Main function
 */
int main(void) {
	
    //  Initialize NCurses  
    if ( (mainwin = initscr()) == NULL ) {
    	fprintf(stderr, "Error initializing NCurses.\n");
    	exit(EXIT_FAILURE);
    }

	// Keyboard configuration
	keypad(mainwin, TRUE);
	cbreak();
	noecho();

	// Draw game screen
	drawScreen();

	// Initialize colors
	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal doesn't support colors\n\n");
		exit(1);
	}
	
	// Starts color mode
	start_color();

	// Define the colors
	init_pair(BULLET_COLOR, COLOR_RED, COLOR_BLACK);
	init_pair(ENEMY_COLOR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(CANNON_COLOR, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(SCORE_COLOR, COLOR_BLUE, COLOR_BLACK);
	init_pair(LEVEL_COLOR, COLOR_BLUE, COLOR_BLACK);

	// Initialize random seed
	srand(time(NULL));	

	// Show opening screen
	openingScreen();
	sleep(2);
	clearScreen();

	// Start level 1
	nextLevel();

    // Game loop
	while(keepRunning == 1) {
		halfdelay(1);
		key = wgetch(mainwin);
		switch(key)
		{	
			case KEY_LEFT:
				if(cannonCol > 1) {
					clearCannon();
					cannonCol--;
				}
				break;
			case KEY_RIGHT:
				if(cannonCol < 56) {
					clearCannon();
					cannonCol++;
				}
				break;
			case ' ':
				fire();
				break;
			case KEY_END:
				endGame();
				break;
		}
		updateScore();
		if (updateEnemies() == KEEP_RUNING) {
			drawBullets();
			checkTargetHit();
			drawCannon();
			if (killedEnemies == 28) {
				if (level < 9) {
					// Go to next level
					nextLevel();
				}
			}
		}else{
			clearScreen();
			gameOver();
			sleep(2);
			restartGame();
			nextLevel();
		}
	}

    delwin(mainwin);
    endwin();
    refresh();

	printf("\n\nBye...\n");

    return EXIT_SUCCESS;
}
