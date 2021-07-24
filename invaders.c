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

// Cannon
char *cannon = "^-^";

// Enemy
char *enemy = ">o<";

// Bullet
char *bullet = "^";

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
int fire1;
int fire2;
int fire3;

// Bullets positions
int bullet1Row;
int bullet2Row;
int bullet3Row;
int bullet1Col;
int bullet2Col;
int bullet3Col;

// Score
int score = 0;

// Counter used to control enemies moves
int tick;


// Array of enemies status - 1:alive 0:killed
int enemiesStatus[28] = {1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,
					1,1,1,1,1,1,1};

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
	printAtRowCol(1, 8, buffer);
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
	printAtRowCol(21 , 0, "|Use left and right arrows keys to move your cannon        |");
	printAtRowCol(22 , 0, "|Press space to shoot                                      |");
	printAtRowCol(23 , 0, "|Press 'End' key to quit the game                          |");
	refresh();
}

/**
 * Get character at the specified coordinate
*/
char getCharRowCol(int row, int col) {
	return mvinch(row, col);
}

/**
 * Draw the cannon
 */
void drawCannon() {
	printAtRowCol(19, cannonCol, cannon);
	printAtRowCol(19, cannonCol + 1, "");
}

/**
 * Clear the cannon
 */
void clearCannon() {
	printAtRowCol(19, cannonCol, "   ");
}

/**
 * Fire bullets
 */
void fire() {
	if (fire1 == 0) {
		// fire bullet 1
		bullet1Col = cannonCol + 1;
		fire1 = 1;		
		return;
	}
	if (fire2 == 0) {
		// fire bullet 1
		bullet2Col = cannonCol + 1;
		fire2 = 1;
		return;
	}
	if (fire3 == 0) {
		// fire bullet 1
		bullet3Col = cannonCol + 1;
		fire3 = 1;
		return;
	}
}

/**
 * Game over
 */
void gameOver() {
	printAtRowCol(0 ,  0, "+----------------------------------------------------------+");
	printAtRowCol(1 ,  0, "|SCORE:              I N V A D E R S                       |");
	printAtRowCol(2 ,  0, "|                                                          |");
	printAtRowCol(3 ,  0, "|                                                          |");
	printAtRowCol(4 ,  0, "|                                                          |");
 	printAtRowCol(5 ,  0, "|               ####     ##    #    #  ######              |");
	printAtRowCol(6 ,  0, "|              #    #   #  #   ##  ##  #                   |");
	printAtRowCol(7 ,  0, "|              #       #    #  # ## #  #####               |");
	printAtRowCol(8 ,  0, "|              #  ###  ######  #    #  #                   |");
	printAtRowCol(9 ,  0, "|              #    #  #    #  #    #  #                   |");
	printAtRowCol(10 , 0, "|               ####   #    #  #    #  ######              |");
	printAtRowCol(11 , 0, "|                                                          |");
	printAtRowCol(12 , 0, "|                                                          |");
	printAtRowCol(13 , 0, "|               ####   #    #  ######  #####               |");
	printAtRowCol(14 , 0, "|              #    #  #    #  #       #    #              |");
	printAtRowCol(15 , 0, "|              #    #  #    #  #####   #    #              |");
	printAtRowCol(16 , 0, "|              #    #  #    #  #       #####               |");
	printAtRowCol(17 , 0, "|              #    #   #  #   #       #   #               |");
	printAtRowCol(18 , 0, "|               ####     ##    ######  #    #              |");
	printAtRowCol(19 , 0, "|                                                          |");
	printAtRowCol(20 , 0, "|                                                          |");
	printAtRowCol(21 , 0, "|                                                          |");
	printAtRowCol(22 , 0, "|                                                          |");
	printAtRowCol(23 , 0, "|By: André Vasconcelos        https://alovasconcelos.com.br|");
	printAtRowCol(24 , 0, "+----------------------------------------------------------+");	
}


/**
 * Dray enemies
 */
int drawEnemies() {
	int enemyNumber = 0;
	for(int row = enemiesRow; row < enemiesRow + 4; row++) {
		for(int col = enemiesCol; col < enemiesCol + 37; col += 6) {
			if (enemiesStatus[enemyNumber]) {
				printAtRowCol(row, col, enemy);
				if (row == 19) {
					// Game over
					return 0;
				}
			}else{
				printAtRowCol(row, col, "   ");
			}
			enemyNumber++;
			refresh();
		}
	}
	return 1;
}

/**
 * Reset enemies status
*/
void resetEnemiesStatus() {
	for(int i=0; i<28; i++) {
		enemiesStatus[i]=1;
	}
}

/**
 * Show level information
 */
void showLevel() {
 	printAtRowCol(3 ,  0, "|          #       ######  #    #  ######  #               |");
	printAtRowCol(4 ,  0, "|          #       #       #    #  #       #               |");
	printAtRowCol(5 ,  0, "|          #       #####   #    #  #####   #               |");
	printAtRowCol(6 ,  0, "|          #       #       #    #  #       #               |");
	printAtRowCol(7 ,  0, "|          #       #        #  #   #       #               |");
	printAtRowCol(8 ,  0, "|          ######  ######    ##    ######  ######          |");	

	switch (level)
	{
		case 1:
			printAtRowCol(10 , 0, "|                            #                             |");	
			printAtRowCol(11 , 0, "|                           ##                             |");	
			printAtRowCol(12 , 0, "|                          # #                             |");	
			printAtRowCol(13 , 0, "|                            #                             |");	
			printAtRowCol(14 , 0, "|                            #                             |");	
			printAtRowCol(15 , 0, "|                            #                             |");	
			printAtRowCol(16 , 0, "|                          #####                           |");	
			break;
		case 2:
			printAtRowCol(10 , 0, "|                          #####                           |");	
			printAtRowCol(11 , 0, "|                         #     #                          |");	
			printAtRowCol(12 , 0, "|                               #                          |");	
			printAtRowCol(13 , 0, "|                          #####                           |");	
			printAtRowCol(14 , 0, "|                         #                                |");	
			printAtRowCol(15 , 0, "|                         #                                |");	
			printAtRowCol(16 , 0, "|                         #######                          |");	
			break;
		case 3:
			printAtRowCol(10 , 0, "|                          #####                           |");	
			printAtRowCol(11 , 0, "|                         #     #                          |");	
			printAtRowCol(12 , 0, "|                               #                          |");	
			printAtRowCol(13 , 0, "|                          #####                           |");	
			printAtRowCol(14 , 0, "|                               #                          |");	
			printAtRowCol(15 , 0, "|                         #     #                          |");	
			printAtRowCol(16 , 0, "|                          #####                           |");	
			break;
		case 4:
			printAtRowCol(10 , 0, "|                         #                                |");	
			printAtRowCol(11 , 0, "|                         #    #                           |");	
			printAtRowCol(12 , 0, "|                         #    #                           |");	
			printAtRowCol(13 , 0, "|                         #######                          |");	
			printAtRowCol(14 , 0, "|                              #                           |");	
			printAtRowCol(15 , 0, "|                              #                           |");	
			printAtRowCol(16 , 0, "|                              #                           |");	
			break;
		case 5:
			printAtRowCol(10 , 0, "|                        #######                           |");	
			printAtRowCol(11 , 0, "|                        #                                 |");	
			printAtRowCol(12 , 0, "|                        #                                 |");	
			printAtRowCol(13 , 0, "|                         #####                            |");	
			printAtRowCol(14 , 0, "|                              #                           |");	
			printAtRowCol(15 , 0, "|                        #     #                           |");	
			printAtRowCol(16 , 0, "|                         #####                            |");	
			break;
		case 6:
			printAtRowCol(10 , 0, "|                         #####                            |");	
			printAtRowCol(11 , 0, "|                        #     #                           |");	
			printAtRowCol(12 , 0, "|                        #                                 |");	
			printAtRowCol(13 , 0, "|                        ######                            |");	
			printAtRowCol(14 , 0, "|                        #     #                           |");	
			printAtRowCol(15 , 0, "|                        #     #                           |");	
			printAtRowCol(16 , 0, "|                         #####                            |");	
			break;
		case 7:
			printAtRowCol(10 , 0, "|                        #######                           |");	
			printAtRowCol(11 , 0, "|                        #    #                            |");	
			printAtRowCol(12 , 0, "|                            #                             |");	
			printAtRowCol(13 , 0, "|                           #                              |");	
			printAtRowCol(14 , 0, "|                          #                               |");	
			printAtRowCol(15 , 0, "|                          #                               |");	
			printAtRowCol(16 , 0, "|                          #                               |");	
			break;
		case 8:
			printAtRowCol(10 , 0, "|                         #####                            |");	
			printAtRowCol(11 , 0, "|                        #     #                           |");	
			printAtRowCol(12 , 0, "|                        #     #                           |");	
			printAtRowCol(13 , 0, "|                         #####                            |");	
			printAtRowCol(14 , 0, "|                        #     #                           |");	
			printAtRowCol(15 , 0, "|                        #     #                           |");	
			printAtRowCol(16 , 0, "|                         #####                            |");	
			break;
		case 9:
			printAtRowCol(10 , 0, "|                         #####                            |");	
			printAtRowCol(11 , 0, "|                        #     #                           |");	
			printAtRowCol(12 , 0, "|                        #     #                           |");	
			printAtRowCol(13 , 0, "|                         ######                           |");	
			printAtRowCol(14 , 0, "|                              #                           |");	
			printAtRowCol(15 , 0, "|                        #     #                           |");	
			printAtRowCol(16 , 0, "|                         #####                            |");	
			break;
	}

	refresh();
}

/**
 * Hide level information
 */
void hideLevel() {
 	printAtRowCol(3 ,  0, "|                                                          |");
	printAtRowCol(4 ,  0, "|                                                          |");
	printAtRowCol(5 ,  0, "|                                                          |");
	printAtRowCol(6 ,  0, "|                                                          |");
	printAtRowCol(7 ,  0, "|                                                          |");
	printAtRowCol(8 ,  0, "|                                                          |");	
	printAtRowCol(10 ,  0, "|                                                          |");	
	printAtRowCol(11 ,  0, "|                                                          |");	
	printAtRowCol(12 ,  0, "|                                                          |");	
	printAtRowCol(13 ,  0, "|                                                          |");	
	printAtRowCol(14,  0, "|                                                          |");	
	printAtRowCol(15 ,  0, "|                                                          |");	
	printAtRowCol(16 ,  0, "|                                                          |");	
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
	fire1 = 0;
	fire2 = 0;
	fire3 = 0;
	bullet1Row = 18;
	bullet2Row = 18;
	bullet3Row = 18;
	bullet1Col = 0;
	bullet2Col = 0;
	bullet3Col = 0;
	tick = 0;
	killedEnemies = 0;
	resetEnemiesStatus();

	// Draw game screen
	drawScreen();

	// Show help bar
	showHelp();

	// Show level
	showLevel();

	// Wait 2 seconds
	sleep(2);

	// Hide level information
	hideLevel();

	// Draw enemies
	drawEnemies();

	// Show level
	char buffer[12];
	snprintf(buffer, 12,"%d",level);
	printAtRowCol(1, 58, buffer);
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
	if (fire1 == 1) {
		if (bullet1Row < 18) {
			printAtRowCol(bullet1Row + 1, bullet1Col, " ");
		}
		printAtRowCol(bullet1Row, bullet1Col, bullet);
		bullet1Row--;	
		if (bullet1Row == 2) {
			printAtRowCol(bullet1Row + 1, bullet1Col, " ");
			fire1 = 0;
			bullet1Row = 18;
		}
	}
	if (fire2 == 1) {
		if (bullet2Row < 18) {
			printAtRowCol(bullet2Row + 1, bullet2Col, " ");
		}
		printAtRowCol(bullet2Row, bullet2Col, bullet);
		bullet2Row--;	
		if (bullet2Row == 2) {
			printAtRowCol(bullet2Row + 1, bullet2Col, " ");
			fire2 = 0;
			bullet2Row = 18;
		}
	}
	if (fire3 == 1) {
		if (bullet3Row < 18) {
			printAtRowCol(bullet3Row + 1, bullet3Col, " ");
		}
		printAtRowCol(bullet3Row, bullet3Col, bullet);
		bullet3Row--;	
		if (bullet3Row == 2) {
			printAtRowCol(bullet3Row + 1, bullet3Col, " ");
			fire3 = 0;
			bullet3Row = 18;
		}		
	}
}

/**
 * Finish the game
 */
void endGame() {
	printAtRowCol(21 , 0, "|Do you really want to quit the game?                      |");
	printAtRowCol(22 , 0, "|    Press Y to confirm                                    |");
	printAtRowCol(23 , 0, "|    Or N to keep playing                                  |");
    refresh();
    int ch;
	while (ch != 'Y' && ch != 'y' && ch != 'N' && ch != 'n') {
		ch = wgetch(mainwin);
	}
              
    if(ch == 'y' || ch == 'Y') {
		keepRunning = 0;
    }
	showHelp();
}

/**
 * ExplodeEnemy
 */
void explodeEnemy(int row, int col) {
	printAtRowCol(row, col, "***");
	refresh();
	usleep(50000);
	printAtRowCol(row, col, ".*.");
	refresh();
	usleep(50000);
	printAtRowCol(row, col, " * ");
	refresh();
	usleep(50000);
	printAtRowCol(row, col, " . ");
	refresh();
	usleep(50000);
	printAtRowCol(row, col, "   ");
	refresh();
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
					enemiesStatus[enemyNumber] = 0;
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
	if(drawEnemies() == 0){
		return 0;
	}

	if (enemiesCol == 19) {
		enemiesDirection = -1;
	}
	if (enemiesCol == 1) {
		enemiesDirection = 1;
	}
	return 1;
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
	switch(bulletNumber) {
		case 1:			
			destroyBullet(bullet1Row + 1, bullet1Col);
			fire1 = 0;
			bullet1Row = 18;
			break;
		case 2:
			destroyBullet(bullet2Row + 1, bullet2Col);
			fire2 = 0;
			bullet2Row = 18;
			break;
		case 3:
			destroyBullet(bullet3Row + 1, bullet3Col);
			fire3 = 0;
			bullet3Row = 18;
			break;
	}
}

/**
 * Check if the target has been hit
 */
void checkTargetHit() {	
	if (fire1) {
		// bullet 1 is fired
		if (isEnemy(bullet1Row, bullet1Col)) {
			destroyEnemy(bullet1Row, bullet1Col);
			resetFire(1);
		}
	}
	if (fire2) {
		// bullet 2 is fired
		if (isEnemy(bullet2Row, bullet2Col)) {
			destroyEnemy(bullet2Row, bullet2Col);
			resetFire(2);
		}
	}
	if (fire3) {
		// bullet 3 is fired
		if (isEnemy(bullet3Row, bullet3Col)) {
			destroyEnemy(bullet3Row, bullet3Col);
			resetFire(3);
		}
	}
}

/**
 * Main function
 */
int main(void) {
	
    //  Initialize NCurses  
    if ( (mainwin = initscr()) == NULL ) {
    	fprintf(stderr, "Erro inicializando NCurses.\n");
    	exit(EXIT_FAILURE);
    }

	// Keyboard configuration
	keypad(mainwin, TRUE);
	noecho();

	// Initialize random seed
	srand(time(NULL));	

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
		if (updateEnemies() == 1) {
			drawBullets();
			checkTargetHit();
			updateScore();
			drawCannon();
			refresh();
			if (killedEnemies == 28) {
				if (level < 9) {
					// Go to next level
					nextLevel();
				}
			}
		}else{
			gameOver();
			updateScore();
			refresh();
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
