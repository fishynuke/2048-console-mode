/*2048 Game V1.0
* Designed by Fishynuke*/
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<Windows.h>
#include<time.h>
#include<string.h>
#include "lang.h"

#define LOSE 1
#define WIN 2
#define NOT_FINISHED 3

FILE *ldrboard, *saved;
unsigned int board[4][4];
unsigned int chance[100];
unsigned long highscore[5];
unsigned long highest, current, finished, combo, merge, changed, maxtile, maxupdated;
void output_status();
void level1();
void level2();
void level3();
void level4();
void savegame();
void up();
void down();
void left();
void right();
void score_cal();

void main() {
	unsigned int i, j, flag, blanknum, operated;
	unsigned int board_copy[4][4];
	char mode, mode2;

	//Files
	if (fopen_s(&ldrboard, "lb.sav", "r+")) {
		MessageBox(NULL, TEXT(ERROR_1), TEXT(ERROR_0), MB_OK);
		exit(-1);
	}
	if (fopen_s(&saved, "sg.sav", "r+")) {
		MessageBox(NULL, TEXT(ERROR_2), TEXT(ERROR_0), MB_OK);
		exit(-2);
	}

	//Leaderboard initializing
	if (fgetc(ldrboard) != '$') {
		MessageBox(NULL, TEXT(ERROR_3), TEXT(ERROR_0), MB_OK);
		exit(-3);
	}
	for (i = 0; i < 5; i++) {
		fscanf(ldrboard, "%u", &highscore[i]);
	}
	highest = highscore[0];

	//Saved game initializing
	if (fgetc(saved) != '#') {
		MessageBox(NULL, TEXT(ERROR_4), TEXT(ERROR_0), MB_OK);
		exit(-4);
	}
	fscanf(saved, "%u", &finished);
	fscanf(saved, "%u", &current);
	fscanf(saved, "%u", &combo);
	maxtile = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			fscanf(saved, "%u", &board[i][j]);
			board_copy[i][j] = board[i][j];
			if (board[i][j] > maxtile) {
				maxtile = board[i][j];
			}
		}
	}

	//Game title
	flag = 1;
	while (flag) {
		printf(WELCOME);
		printf(MODE_SELECT);
		printf(MODES);
		mode = _getch();
		system("cls");
		switch (mode) {
		//Quit game
		case 'q': {
			printf(EXIT_GAME);
			Sleep(1000);
			exit(0);
		}
		//Leaderboard
		case 'l': {
			printf(LEADERBOARD_TITLE);
			for (i = 0; i < 5; i++) {
				printf("%u |", i + 1);
				printf("%10u\n", highscore[i]);
			}
			printf(LEADERBOARD_INS);
			mode = _getch();
			//Back
			if (mode == 'b') {
				system("cls");
				continue;
			}
			//Reset Leaderboard
			if (mode == 'r') {
				fclose(ldrboard);
				fopen_s(&ldrboard, "lb.sav", "r+");
				fprintf(ldrboard, "$\n");
				for (i = 0; i < 5; i++) {
					highscore[i] = 5730;
					fprintf(ldrboard, "%u\n", highscore[i]);
				}
				fclose(ldrboard);
				fopen_s(&ldrboard, "lb.sav", "r+");
				MessageBox(NULL, TEXT(LEADERBOARD_CLEAR), TEXT(HINT), MB_OK);
				system("cls");
			}
			break;
		}
		//New Game
		case 'n': {
			if (finished) {
				for (i = 0; i < 4; i++) {
					for (j = 0; j < 4; j++) {
						board[i][j] = 0;
						board_copy[i][j] = 0;
					}
				}
				finished = 0;
				current = 0;
				combo = 0;
				maxtile = 0;
				flag = 0;
			}
			else { //Saved game detected
				printf(CONFIRM_NEWGAME);
				mode = _getch();
				if (mode == 'n') {
					for (i = 0; i < 4; i++) {
						for (j = 0; j < 4; j++) {
							board[i][j] = 0;
						}
					}
					current = 0;
					combo = 0;
					maxtile = 0;
					flag = 0;
				}
				if (mode == 'c') {
					flag = 0;
				}
			}
			break;
		}
		//Continue saved game
		case 'c': {
			if (finished) { //No saved game detected
				MessageBox(NULL, TEXT(FINISHED_GAME), TEXT(HINT), MB_OK);
				system("cls");
			}
			else {
				flag = 0;
			}
			break;
		}
		}
	}
	system("cls");
	if (mode == 'c') {
		printf(CONTINUE_LASTGAME);
		operated = 0;
	}
	if (mode == 'n') {
		printf(START_NEWGAME);
		srand((int)time(NULL));
		blanknum = rand() % 16;
		board[blanknum / 4][blanknum % 4] = 2;
		operated = 1;
	}
	Sleep(1000);
	system("cls");

	//Game start
	flag = 0;
	mode2 = 0;
	maxupdated = 1;
	while (flag == 0) {
		if (operated) {
			//Number of blank tiles and compare copy board
			blanknum = 0;
			changed = 0;
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					if (board[i][j] == 0) {
						blanknum++;
					}
					if (board_copy[i][j] != board[i][j]) {
						changed = 1;
					}
				}
			}
			//No blank tile -> lose
			if (!blanknum) {
				flag = LOSE;
				finished = 1;
				break;
			}
			//See if last move is effected
			if (changed) {
				//Calculate scores and combos
				if (merge) {
					combo++;
				}
				else {
					if (changed) {
						combo = 0;
					}
				}
				score_cal();
				//Automatic difficulty setting
				if (maxupdated) {
					if (maxtile < 128) {
						level1();
					}
					else if (maxtile < 512) {
						level2();
					}
					else if (maxtile < 2048) {
						level3();
					}
					else {
						level4();
					}
					maxupdated = 0;
				}
				//Randomly generates new number in blank tile
				srand((int)time(NULL));
				blanknum = rand() % blanknum + 1;
				for (i = 0; i < 4; i++) {
					for (j = 0; j < 4; j++) {
						if (board[i][j] == 0) {
							blanknum--;
							if (blanknum == 0) {
								board[i][j] = chance[rand() % 100 + 1];
							}
						}
					}
				}
				for (i = 0; i < 4; i++) {
					for (j = 0; j < 4; j++) {
						board_copy[i][j] = board[i][j];
					}
				}
			}
			operated = 0;
		}
		//Output and waiting for input
		output_status();
		mode = _getch();
		system("cls");
		//Q for quit
		if (mode == 'q') {
			while (1) {
				//Save game?
				printf(SAVE_GAME_HINT);
				mode = _getch();
				if (mode == 'y') {
					savegame();
					flag = NOT_FINISHED;
					finished = 0;
					break;
				}
				if (mode == 'n') {
					flag = NOT_FINISHED;
					finished = 0;
					MessageBox(NULL, TEXT(GAME_UNSAVED), TEXT("HINT"), MB_OK);
					break;
				}
			}
		}
		//S for saving game
		if (mode == 's') {
			savegame();
		}
		//Direction key detected
		if (mode < 0) {
			mode2 = _getch();
		}
		if (mode2) {
			merge = 0;
			switch (mode2) {
			case 72: {
				up();
				break;
			}
			case 80: {
				down();
				break;
			}
			case 75: {
				left();
				break;
			}
			case 77: {
				right();
				break;
			}
			}
			mode2 = 0;
			operated = 1;
		}
	}

	//Results
	if (finished) {
		//Finished game (win or lose)
		system("cls");
		printf(CURRENT_SCORE);
		printf("%u\n\n", current);
		int k;
		for (i = 0; i < 4; i++) {
			k = 0;
			while (k++ < 25) {
				printf("-");
			}
			printf("\n");
			k = 0;
			while (k++ < 25) {
				if ((k - 1) % 6 == 0) {
					printf("|");
				}
				else {
					printf(" ");
				}
			}
			printf("\n|");
			for (j = 0; j < 4; j++) {
				if (board[i][j] == 0) {
					printf("     |");
				}
				else if (board[i][j] < 10) {
					printf("%3u  |", board[i][j]);
				}
				else if (board[i][j] < 1000) {
					printf("%4u |", board[i][j]);
				}
				else {
					printf("%5u|", board[i][j]);
				}
			}
			printf("\n");
			k = 0;
			while (k++ < 25) {
				if ((k - 1) % 6 == 0) {
					printf("|");
				}
				else {
					printf(" ");
				}
			}
			printf("\n");
		}
		k = 0;
		while (k++ < 25) {
			printf("-");
		}
		printf("\n");
		//Judge
		if (flag == LOSE) {
			MessageBox(NULL, TEXT(LOSE_TITLE), TEXT(HINT), MB_OK);
		}
		if (flag == WIN) {
			MessageBox(NULL, TEXT(WIN_TITLE), TEXT(HINT), MB_OK);
		}
		savegame();
		fclose(saved);
	}
	else { //Game not finished
		fclose(saved);
	}

	//Highscore / Leaderboard update
	for (i = 0; i < 5; i++) {
		if (current >= highscore[i]) {
			if (i == 0) {
				MessageBox(NULL, TEXT(NEW_HIGHSCORE), TEXT(HINT), MB_OK);
			}
			else {
				MessageBox(NULL, TEXT(GOTON_LEADERBOARD), TEXT(HINT), MB_OK);
			}
			for (j = 4; j > i; j--) {
				highscore[j] = highscore[j - 1];
			}
			highscore[i] = current;
			break;
		}
	}
	//Rewrite Leaderboard file
	fclose(ldrboard);
	fopen_s(&ldrboard, "lb.sav", "r+");
	fprintf(ldrboard, "$\n");
	for (i = 0; i < 5; i++) {
		fprintf(ldrboard, "%u\n", highscore[i]);
	}
	fclose(ldrboard);
	
	//Continue game or not
	while (1) {
		system("cls");
		printf(END_OF_GAME);
		mode = _getch();
		if (mode == 'y') {
			Sleep(500);
			system("cls");
			main();
		}
		if (mode == 'n') {
			printf(EXIT_GAME);
			Sleep(1000);
			exit(0);
		}
	}
}//End of main()

void output_status() {
	printf(DURING_GAME_TITLE);
	printf(GAME_INSTRUCTION);
	printf(GAME_OPERATION);
	printf(HIGHEST_SCORE);
	printf("%u\n", highest);
	printf(CURRENT_SCORE);
	printf("%u\t", current);
	if (combo > 1) {
		printf("%u ", combo);
		printf(COMBOS);
	}
	printf("\n");
	int i, j, k;
	for (i = 0; i < 4; i++) {
		k = 0;
		while (k++ < 25) {
			printf("-");
		}
		printf("\n");
		k = 0;
		while (k++ < 25) {
			if ((k - 1) % 6 == 0) {
				printf("|");
			}
			else {
				printf(" ");
			}
		}
		printf("\n|");
		for (j = 0; j < 4; j++) {
			if (board[i][j] == 0) {
				printf("     |");
			}
			else if (board[i][j] < 10) {
				printf("%3u  |", board[i][j]);
			}
			else if (board[i][j] < 1000) {
				printf("%4u |", board[i][j]);
			}
			else {
				printf("%5u|", board[i][j]);
			}
		}
		printf("\n");
		k = 0;
		while (k++ < 25) {
			if ((k - 1) % 6 == 0) {
				printf("|");
			}
			else {
				printf(" ");
			}
		}
		printf("\n");
	}
	k = 0;
	while (k++ < 25) {
		printf("-");
	}
	printf("\n\n");
	printf(INPUT_TEXT);
}//End of output_status()

void level1() {
	int i;
	for (i = 0; i < 100; i++) {
		chance[i] = 2;
	}
}
void level2() {
	int i;
	for (i = 0; i < 90; i++) {
		chance[i] = 2;
	}
	for (i = 90; i < 100; i++) {
		chance[i] = 4;
	}
}
void level3() {
	int i;
	for (i = 0; i < 75; i++) {
		chance[i] = 2;
	}
	for (i = 75; i < 100; i++) {
		chance[i] = 4;
	}
}
void level4() {
	int i;
	for (i = 0; i < 50; i++) {
		chance[i] = 2;
	}
	for (i = 50; i < 90; i++) {
		chance[i] = 4;
	}
	for (i = 90; i < 100; i++) {
		chance[i] = 8;
	}
}//End of levels()

void savegame() {
	unsigned int i, j;
	//Rewrite saved game file
	fclose(saved);
	fopen_s(&saved, "sg.sav", "r+");
	fprintf(saved, "#\n");
	fprintf(saved, "%u\n", finished);
	fprintf(saved, "%u\n", current);
	fprintf(saved, "%u\n", combo);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (j == 3) {
				fprintf(saved, "%5u", board[i][j]);
			}
			else {
				fprintf(saved, "%5u ", board[i][j]);
			}
		}
		fprintf(saved, "\n");
	}
	fclose(saved);
	fopen_s(&saved, "sg.sav", "r+");
	MessageBox(NULL, TEXT(GAME_SAVED), TEXT(HINT), MB_OK);
}//End of savegame()

//i is row j is column
void up() {
	int i, j, k, merged, temp;
	for (j = 0; j < 4; j++) {
		merged = 1;
		k = 0;
		for (i = 0; i < 4; i++) {
			if (board[i][j] > 0) {
				temp = board[i][j];
				board[i][j] = 0;
				if (merged) {
					while (k < 4) {
						if (board[k][j] == 0) {
							board[k][j] = temp;
							break;
						}
						k++;
					}
					merged = 0;
				}
				else {
					if (board[k][j] == temp) {
						board[k][j] += temp;
						merge++;
						merged = 1;
						if (board[k][j] > maxtile) {
							maxtile = board[k][j];
							maxupdated = 1;
						}
					}
					else {
						board[k + 1][j] = temp;
					}
					k++;
				}
			}
		}
	}
}
void down() {
	int i, j, k, merged, temp;
	for (j = 0; j < 4; j++) {
		merged = 1;
		k = 3;
		for (i = 3; i > -1; i--) {
			if (board[i][j] > 0) {
				temp = board[i][j];
				board[i][j] = 0;
				if (merged) {
					while (k > -1) {
						if (board[k][j] == 0) {
							board[k][j] = temp;
							break;
						}
						k--;
					}
					merged = 0;
				}
				else {
					if (board[k][j] == temp) {
						board[k][j] += temp;
						merge++;
						merged = 1;
						if (board[k][j] > maxtile) {
							maxtile = board[k][j];
							maxupdated = 1;
						}
					}
					else {
						board[k - 1][j] = temp;
					}
					k--;
				}
			}
		}
	}
}
void left() {
	int i, j, k, merged, temp;
	for (i = 0; i < 4; i++) {
		merged = 1;
		k = 0;
		for (j = 0; j < 4; j++) {
			if (board[i][j] > 0) {
				temp = board[i][j];
				board[i][j] = 0;
				if (merged) {
					while (k < 4) {
						if (board[i][k] == 0) {
							board[i][k] = temp;
							break;
						}
						k++;
					}
					merged = 0;
				}
				else {
					if (board[i][k] == temp) {
						board[i][k] += temp;
						merge++;
						merged = 1;
						if (board[i][k] > maxtile) {
							maxtile = board[i][k];
							maxupdated = 1;
						}
					}
					else {
						board[i][k + 1] = temp;
					}
					k++;
				}
			}
		}
	}
}
void right() {
	int i, j, k, merged, temp;
	for (i = 0; i < 4; i++) {
		merged = 1;
		k = 3;
		for (j = 3; j > -1; j--) {
			if (board[i][j] > 0) {
				temp = board[i][j];
				board[i][j] = 0;
				if (merged) {
					while (k > -1) {
						if (board[i][k] == 0) {
							board[i][k] = temp;
							break;
						}
						k--;
					}
					merged = 0;
				}
				else {
					if (board[i][k] == temp) {
						board[i][k] += temp;
						merge++;
						merged = 1;
						if (board[i][k] > maxtile) {
							maxtile = board[i][k];
							maxupdated = 1;
						}
					}
					else {
						board[i][k - 1] = temp;
					}
					k--;
				}
			}
		}
	}
}//End of gameoperations()

void score_cal() {
	int score;
	if (merge) {
		score = (merge + 1) * 100 + (combo - 1) * 10;
		if (maxupdated) {
			if (maxtile < 1024) {
				score += 500;
				MessageBox(NULL, TEXT(NEW_TILE_BONUS), TEXT(BONUS_500), MB_OK);
			}
			else if (maxtile < 4096) {
				score += 1000;
				MessageBox(NULL, TEXT(NEW_TILE_BONUS), TEXT(BONUS_1000), MB_OK);
			}
			else {
				score += 5000;
				MessageBox(NULL, TEXT(NEW_TILE_BONUS), TEXT(BONUS_5000), MB_OK);
			}
		}
	}
	else {
		score = 0;
	}
	current += score;
	if (current > highest) {
		highest = current;
	}
}//End of score_cal()