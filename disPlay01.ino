#include <SPI.h>
#include "Ucglib.h"
#include "Keypad.h"
    
/*
  Hardware SPI Pins:
    Arduino Uno    sclk=13, data=11
    Arduino Due   sclk=76, data=75
    Arduino Mega  sclk=52, data=51
    
  >>> Please uncomment (and update) one of the following constructors. <<<  
*/

//Ucglib_SSD1331_18x96x64_UNIVISION_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);
Ucglib_SSD1331_18x96x64_UNIVISION_HWSPI ucg(/*cd=*/ 9 , /*cs=*/ 10, /*reset=*/ 8);

//keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'#','0','*'}
};
byte rowPins[ROWS] = {14, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup(void) {
	//Serial.begin(9600);
	randomSeed(analogRead(5));
	ucg.begin(UCG_FONT_MODE_TRANSPARENT);
	keypad.setDebounceTime(65);
}

void loop(void) {
	ucg.setFont(ucg_font_helvB08_tr);
	ucg.clearScreen();
	
	// CHOOSE MODE
	ucg.setColor(255, 255, 255); // white
	ucg.setPrintPos(25, 10);
	ucg.print("XO Game");
	ucg.setPrintPos(2, 24);
	ucg.print("(1) vs AI");
	ucg.setPrintPos(2, 38);
	ucg.print("(2) vs Player");
	
	char keypadInput;
	unsigned int mode = 0;
	do {
		keypadInput = keypad.getKey();
	} while ( keypadInput == NO_KEY || keypadInput != '1' && keypadInput != '2');
	if (keypadInput == '1') mode = 1;
	if (keypadInput == '2') mode = 2;
	
	//game start here
	ucg.clearScreen();
	int board[9] = { 0,0,0,0,0,0,0,0,0 };
	
	if (mode == 1) {
		//choose who play first
		ucg.setColor(255, 255, 255);
		ucg.setPrintPos(2, 10);
		ucg.print("You play");
		ucg.setPrintPos(2, 24);
		ucg.print("(1)st or (2)nd");

		char keypadInput;
		do {
			keypadInput = keypad.getKey();
		} while ( keypadInput == NO_KEY || keypadInput != '1' && keypadInput != '2');
		
		//define board and player turn
		unsigned player = 1; // 1 player first | 2 ai first

		if (keypadInput == '2') player = 2;
		else player = 1;

		unsigned turn = 0;
		String turnText[2] = { "Ai Thinking", "Your Turn" };

		for (turn = 0; turn < 9 && win(board) == 0; ++turn) {
			
			draw(board);
			
			if ((turn + player) % 2 == 0) { // ai
				drawTurnText(0, turnText);
				
				
				if (turn == 0 && player == 2) { // ai first move - random
					randomComputerMove(board);
				}
				else{
					computerMove(board);
				}
			} else { // Human
				drawTurnText(1, turnText);
				playerMove(board);
			}
		}
		String resultString[3] = { "Draw.", "You lose.", "You win!!"};
		drawResult(win(board), board, resultString);
	}
	
	if (mode == 2){
		unsigned turn;
		int player = 2;
		String turnText[2] = { "Player 1", "Player 2" };
		
		for (turn = 0; turn < 9 && win(board) == 0; ++turn) {
			draw(board);
			if ((turn + player) % 2 == 0) { 
				// Player 1st, RED (-1)
				drawTurnText(0, turnText);
				pvpMove(1,board);
			} 
			else { 
				// Player 2nd, BLUE (1)
				drawTurnText(1, turnText);
				pvpMove(-1,board);
			}
		}
		
		String resultString[3] = { "Draw.", "Player 1 Win", "Player 2 Win"}; // 0 1 -1
		drawResult(win(board), board, resultString);
	}
	
	delay(2500);
}

void drawBoardAndBG() {
	ucg.clearScreen();
	
	//big box
	ucg.setColor(255, 255, 255);
	ucg.drawFrame(31, 12, 37, 37);

	//fill box with line
	ucg_SetColor(ucg.getUcg(), 0, 255, 255, 255);
	ucg_DrawVLine(ucg.getUcg(), 43, 13, 35);
	ucg_DrawVLine(ucg.getUcg(), 55, 13, 35);
	ucg_DrawHLine(ucg.getUcg(), 32, 24, 35);
	ucg_DrawHLine(ucg.getUcg(), 32, 36, 35);
}

void fillBoard(int board[9]){
	unsigned int boxLocationWH = 11;
	unsigned int boxLocation[9][2] = { {32,13}, {44,13}, {56,13},
                               {32,25}, {44,25}, {56,25},
                               {32,37}, {44,37}, {56,37}};

	for (int i = 0; i < 9; i++) {
		if (board[i] == 1) {
			ucg.setColor(0, 255, 0, 0); // red box  
			ucg.drawBox(boxLocation[i][0], boxLocation[i][1], boxLocationWH, boxLocationWH);
		} else if (board[i] == -1) {
			ucg.setColor(0, 0, 0, 255); // blue
			ucg.drawBox(boxLocation[i][0], boxLocation[i][1], boxLocationWH, boxLocationWH);
		}
	}// end for
}

void draw(int board[9]) {
	drawBoardAndBG();
	fillBoard(board);
}

void drawTurnText(int input, String turnText[2]) {
	unsigned int boxTurnX = 10;
	unsigned int boxTurnY = 56;
	unsigned int boxTurnSize = 6;
	
	if (input == 0) { // 0 ai
		ucg.setColor(0, 255, 0, 0); // red box
	}
	else {
		ucg.setColor(0, 0, 0, 255); // blue
	}

	// draw Turn color box
	ucg.drawBox(boxTurnX, boxTurnY, boxTurnSize, boxTurnSize); 

	//draw text
	ucg.setColor(255, 255, 255);
	ucg.setPrintPos(boxTurnX + boxTurnSize + 4, boxTurnY + boxTurnSize);
	ucg.setPrintDir(0);
	ucg.print(turnText[input]);
}

void drawResult(int input, int board[9], String resultString[3]) {
	//set text attrb
	draw(board);
	ucg.setPrintPos(18, 60);
	ucg.setPrintDir(0);
	switch (input) {
	case 0:
		ucg.setColor(0, 255, 0);
		ucg.print(resultString[0]);
		break;
	case 1:
		ucg.setColor(255, 0, 0);
		ucg.print(resultString[1]);
		break;
	case -1:
		ucg.setColor(0, 0, 255);
		ucg.print(resultString[2]);
		break;
	}
}

int win(const int board[9]) {
	// -1 hu | 1 ai | 0 none (draw/gameNotEnd)
	unsigned int wins[8][3] = { { 0,1,2 },{ 3,4,5 },{ 6,7,8 },{ 0,3,6 },{ 1,4,7 },{ 2,5,8 },{ 0,4,8 },{ 2,4,6 } };

	for (int i = 0; i < 8; ++i) {
		if (board[wins[i][0]] != 0 &&
			board[wins[i][0]] == board[wins[i][1]] &&
			board[wins[i][0]] == board[wins[i][2]])
		{
			return board[wins[i][2]];
		}
	}
	return 0;
}

int minimax(int board[9], int player, int& depth) {
	int winner = win(board);
	if (winner != 0) {
		depth++;
		if (winner == 1) return 10;
		else if (winner == -1) return -10;
	}

	int saveDepth = 0;
	int newDepth = depth;

	if (player == 1) { // ai
		int move = -1;
		int score = -1000;

		for (int i = 0; i < 9; ++i, newDepth=depth) {
			if (board[i] == 0) {
				board[i] = player;
				int thisScore = minimax(board, player*-1, newDepth);

				if (thisScore > score) {
					score = thisScore;
					move = i;
					saveDepth = newDepth;
				}
				if (thisScore == score && newDepth<saveDepth) {
					score = thisScore;
					move = i;
					saveDepth = newDepth;
				}
				board[i] = 0;
			}
		}

		if (move == -1) return 0;
		saveDepth++;
		depth = saveDepth;
		return score;
	}
	else {
		int move = -1;
		int score = 1000;

		for (int i = 0; i < 9; ++i, newDepth=depth) {
			if (board[i] == 0) {
				board[i] = player;
				int thisScore = minimax(board, player*-1, newDepth);
				if (thisScore < score) {
					score = thisScore;
					move = i;
					saveDepth = newDepth;
				}
				if (thisScore == score && newDepth<saveDepth) {
					score = thisScore;
					move = i;
					saveDepth = newDepth;
				}
				board[i] = 0;
			}
		}

		if (move == -1) return 0;
		saveDepth++;
		depth = saveDepth;
		return score;
	}

}

void computerFastOpenMove(int board[9]){
	if (board[4] == -1)  {
		unsigned int border[4] = {0,2,6,8};
		int tmpRnd = random(0, 4);
		int myNum = border[tmpRnd];
		board[border[myNum]] = 1; 
	}
	else { board[4] = 1; }
}

void computerMove(int board[9]) {
	int move = -1;
	int score = -1000;
	int depth = 0;
	int newDepth = depth;
	int saveDepth = 0;

	for (int i = 0; i < 9; ++i, newDepth = depth) {
		if (board[i] == 0) {
			board[i] = 1;
			int tempScore = minimax(board, -1, newDepth);
			board[i] = 0;
			if (tempScore > score) {
				score = tempScore;
				move = i;
				saveDepth = newDepth;
			}
			if (tempScore == score && newDepth<saveDepth) {
				score = tempScore;
				move = i;
				saveDepth = newDepth;
			}
		}
	}
	
	board[move] = 1;
}

void randomComputerMove(int board[9]) {
	int randomnum = random(0, 9);
	board[randomnum] = 1;
	//board[0] = 1;
}

void playerMove(int board[9]) {
	int move = -1;
	char keypadInput;
	do {
		// read clean key
		do {
			keypadInput = keypad.getKey();
			if (keypadInput >= '1' && keypadInput <= '9') {
				move = keypadInput - '1';// compare ascii char
			}
		} while (keypadInput == NO_KEY || keypadInput == '*' || keypadInput == '#');
	} while (move > 8 || move < 0 || board[move] != 0);
	board[move] = -1;
}

void pvpMove(int input,int board[9]){
	int move = -1;
	char keypadInput;
	do {
		// read clean key
		do {
			keypadInput = keypad.getKey();
			if (keypadInput >= '1' && keypadInput <= '9') {
				move = keypadInput - '1';// compare ascii char
			}
		} while (keypadInput == NO_KEY || keypadInput == '*' || keypadInput == '#');
	} while (move > 8 || move < 0 || board[move] != 0);
	board[move] = input; //****
}
