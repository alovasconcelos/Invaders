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

// Counter used to sincronize enemies moves
int tick;

// Cannon
char *cannon = "^-^";

// Enemy
char *enemy = ">o<";

// Array of enemies status
int enemiesStatus[28] = {1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,
					1,1,1,1,1,1,1};

// Killed enemies
int killedEnemies;

/*
 	Print string at specified coordinate
*/
void printAtRowCol(int row, int col, char *str) {
	mvaddstr(row, col, str);
}

/*
	Update score
*/
void updateScore() {	
	char buffer[12];
	snprintf(buffer, 12,"%d",score);
	printAtRowCol(1, 8, buffer);
}

/*
	Draw the game board
*/
void drawScreen() {
	printAtRowCol(0 ,  0, "+----------------------------------------------------------+");
	printAtRowCol(1 ,  0, "|SCORE:                                            LEVEL:  |");
	printAtRowCol(2 ,  0, "|                                                          |");
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
}

/*
	Get character at the specified coordinate
*/
char getCharRowCol(int row, int col) {
	return mvinch(row, col);
}

/*
	Draw the cannon
*/
void drawCannon() {
	printAtRowCol(19, cannonCol, cannon);
	printAtRowCol(19, cannonCol + 1, "");
}

/*
	Clear the cannon
*/
void clearCannon() {
	printAtRowCol(19, cannonCol, "   ");
}

/*
	Fire bullets
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

/*
	Game over
*/
void gameOver() {
	printAtRowCol(0 ,  0, "+----------------------------------------------------------+");
	printAtRowCol(1 ,  0, "|SCORE:                                                    |");
	printAtRowCol(2 ,  0, "|                                                          |");
 	printAtRowCol(3 ,  0, "|               ####     ##    #    #  ######              |");
	printAtRowCol(4 ,  0, "|              #    #   #  #   ##  ##  #                   |");
	printAtRowCol(5 ,  0, "|              #       #    #  # ## #  #####               |");
	printAtRowCol(6 ,  0, "|              #  ###  ######  #    #  #                   |");
	printAtRowCol(7 ,  0, "|              #    #  #    #  #    #  #                   |");
	printAtRowCol(8 ,  0, "|               ####   #    #  #    #  ######              |");
	printAtRowCol(9 ,  0, "|                                                          |");
	printAtRowCol(10 , 0, "|                                                          |");
	printAtRowCol(11 , 0, "|               ####   #    #  ######  #####               |");
	printAtRowCol(12 , 0, "|              #    #  #    #  #       #    #              |");
	printAtRowCol(13 , 0, "|              #    #  #    #  #####   #    #              |");
	printAtRowCol(14 , 0, "|              #    #  #    #  #       #####               |");
	printAtRowCol(15 , 0, "|              #    #   #  #   #       #   #               |");
	printAtRowCol(16 , 0, "|               ####     ##    ######  #    #              |");
	printAtRowCol(17 , 0, "|                                                          |");
	printAtRowCol(18 , 0, "|                                                          |");
	printAtRowCol(19 , 0, "|                                                          |");
	printAtRowCol(20 , 0, "+----------------------------------------------------------+");	
}

/*
 	Dray enemies
*/
void drawEnemies() {
	int enemyNumber = 0;
	for(int row = enemiesRow; row < enemiesRow + 4; row++) {
		for(int col = enemiesCol; col < enemiesCol + 37; col += 6) {
			if (enemiesStatus[enemyNumber]) {
				printAtRowCol(row, col, enemy);
				if (row == 19) {
					// Game over
					gameOver();
					updateScore();
					refresh();
					sleep(5);					
				}
			}else{
				printAtRowCol(row, col, "   ");
			}
			enemyNumber++;
			refresh();
		}
	}
}

/*
	Clear enemies
*/
void clearEnemies() {
	for(int row = enemiesRow; row < enemiesRow + 4; row++) {
		for(int col = enemiesCol; col < enemiesCol + 37; col+= 6) {
			printAtRowCol(row, col, "   ");
			refresh();
		}
	}
}

/*
	Draw the fired bullets
*/
void drawBullets() {
	if (fire1 == 1) {
		if (bullet1Row < 18) {
			printAtRowCol(bullet1Row + 1, bullet1Col, " ");
		}
		printAtRowCol(bullet1Row, bullet1Col, "^");
		bullet1Row--;	
		if (bullet1Row == 0) {
			printAtRowCol(bullet1Row + 1, bullet1Col, " ");
			fire1 = 0;
			bullet1Row = 18;
		}
	}
	if (fire2 == 1) {
		if (bullet2Row < 18) {
			printAtRowCol(bullet2Row + 1, bullet2Col, " ");
		}
		printAtRowCol(bullet2Row, bullet2Col, "^");
		bullet2Row--;	
		if (bullet2Row == 0) {
			printAtRowCol(bullet2Row + 1, bullet2Col, " ");
			fire2 = 0;
			bullet2Row = 18;
		}
	}
	if (fire3 == 1) {
		if (bullet3Row < 18) {
			printAtRowCol(bullet3Row + 1, bullet3Col, " ");
		}
		printAtRowCol(bullet3Row, bullet3Col, "^");
		bullet3Row--;	
		if (bullet3Row == 0) {
			printAtRowCol(bullet3Row + 1, bullet3Col, " ");
			fire3 = 0;
			bullet3Row = 18;
		}		
	}
}

/*
	Finish the game
*/
void endGame() {
	keepRunning = 0;
}

/*
 	Check if theres an enemy at a given position
*/
int isEnemy(int row, int col) {
	int enemyNumber = 0;
	for(int enemyRow = enemiesRow; enemyRow < enemiesRow + 4; enemyRow++) {
		for(int enemyCol = enemiesCol; enemyCol < enemiesCol + 37; enemyCol += 6) {
			if (enemiesStatus[enemyNumber]) {
				if (row == enemyRow && col >= enemyCol && col <= enemyCol + 2) {
					// Enemy has been hit
					enemiesStatus[enemyNumber] = 0;
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
 
/*
	Update enemies
*/
void updateEnemies() {

	int r = rand() % 100;

	if (tick++ % 5 == 0) {
		if (r > 50) {
			clearEnemies();
			enemiesCol += enemiesDirection;
		}
		if (r < 10) {		
			clearEnemies();
			enemiesRow++;
		}
	}
	drawEnemies();

	if (enemiesCol == 19) {
		enemiesDirection = -1;
	}
	if (enemiesCol == 1) {
		enemiesDirection = 1;
	}
}

/*
	Destroy enemy
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

/*
	Destroy bullet
*/
void destroyBullet(int row, int col) {
	printAtRowCol(row, col, " ");
}

/*
	Reset fire
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

/*
	Check if the target has been hit
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

/*
	Reset enemies status
*/
void resetEnemiesStatus() {
	for(int i=0; i<28; i++) {
		enemiesStatus[i]=1;
	}
}

/*
	Start level
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

	// Draw enemies
	drawEnemies();

	// Show level
	char buffer[12];
	snprintf(buffer, 12,"%d",level);
	printAtRowCol(1, 58, buffer);
}

/*
	Main function
*/
int main(void) {
    WINDOW * mainwin;
	int key;
	
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
				clearCannon();
				cannonCol--;
				break;
			case KEY_RIGHT:
				clearCannon();
				cannonCol++;
				break;
			case ' ':
				fire();
				break;
			case KEY_HOME:
				endGame();
				break;
		}
		updateEnemies();
		drawBullets();
		checkTargetHit();
		updateScore();
		drawCannon();
    	refresh();
		if (killedEnemies == 28) {
			if (level < 9) {
				// Go to nextLevel
				nextLevel();
			}
		}
	}

    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}
