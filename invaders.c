/*
 * Invaders
 * By André Luiz de Oliveira Vasconcelos
 * https://alovasconcelos.com.br
 * alovasconcelos@gmail.com
 * https://github.com/alovasconcelos/Invaders
 * invaders.c
 * 2021
 */

#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "invaders.h"

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

	// Wait 2 seconds
	sleep(2);

	// Clear the screen
	clearScreen();

	// Start level 1
	nextLevel();

    // Game loop
	while(keepRunning == 1) {
		// Keyboard input
		halfdelay(1);
		key = wgetch(mainwin);
		switch(key)
		{	
			case KEY_LEFT:
				moveCannonLeft();
				break;
			case KEY_RIGHT:
				moveCannonRight();
				break;
			case ' ':
				fire();
				break;
			case KEY_END:
				endGame();
				break;
		}
		// Update the game score
		updateScore();
		if (updateEnemies() == KEEP_RUNING) {
			// Draw the bullets on the game field
			drawBullets();

	 		// Check if any bullet hit some target		
			checkTargetHit();

			// Draw the cannon 
			drawCannon();

			// If there's no more enemies and 
			// the level number is less than 9
			// then go to nextlevel
			if (remainingEnemies == 0 && level < 9) {
				// Go to next level
				nextLevel();
			}
		}else{
			// Game over
			gameOver();
		}
	}

	// Destroy the game field
    delwin(mainwin);
    endwin();
    refresh();

	printf("\n\nBye...\n");

    return EXIT_SUCCESS;
}
