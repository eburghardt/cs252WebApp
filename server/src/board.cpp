/**
 * @file board.cpp
 * @brief This class implements the 3d character array we will use to store the board as well as
 * 	some helper functions to aid in placing new characters in and reading the board.
 *
 * @author Eric Burghardt
 * @date 11/22/2017
 */

#include "../include/board.hpp"

using namespace std;


/* @method getOffAxisWordList
 * @brief	This function is a helper function for getPlayWordList that finds off axis words. 
 *
 * @param string play	The word to be played. Will have already been verified as board legal by canPlaceString
 *			Characters already on the board will be spaces here. The method is recursive and removes
 *		the first letter with each call.
 * @param int x, y, z	These refer to the current position in the word
 * @param int direction	This tells the direction of the play. 0 is x, 1 is y, and 2 is z.
 */
vector<string> Board::getOffAxisWordList(string play, int x, int y, int z, int direction) {
	if(play.length() == 0) {
		vector<string> emptyVector;
		return emptyVector;
	}

	//for each letter in the word, we need to check the off-axes for words
	//Skip spaces. They represent letters already on the board.
	vector<string> wordList;
	if(play.at(0) != ' ') {
		if(direction != 0) {
			string wordX = findWordX(play.at(0), x, y, z);
			if(wordX.length() > 1) {
				wordList.insert(wordList.end(), wordX);
			}	
		}

		if(direction != 1) {
			string wordY = findWordY(play.at(0), x, y, z);
			if(wordY.length() > 1) {
				wordList.insert(wordList.end(), wordY);
			}
		}

		if(direction != 2) {
			string wordZ = findWordZ(play.at(0), x, y, z);
			if(wordZ.length() > 1) {
				wordList.insert(wordList.end(), wordZ);
			}
		}
	}

	vector<string> restOfWord;
	switch(direction) {
		case 0:
			restOfWord = getOffAxisWordList(play.substr(1, string::npos), x + 1, y, z, direction);
			break;
		case 1:
			restOfWord = getOffAxisWordList(play.substr(1, string::npos), x, y + 1, z, direction);
			break;
		case 2:
			restOfWord = getOffAxisWordList(play.substr(1, string::npos), x, y, z + 1, direction);
			break;
		default:
			//invalid direction
			break;
	}

	//append the rest of the list to our list
	wordList.insert(wordList.end(), restOfWord.begin(), restOfWord.end());
			
	return wordList;	
}

string Board::findWordX(char c, int x, int y, int z) {
	int startX = x;
	//Traverse left
	while(getChar(startX - 1, y, z) >= 'a' && getChar(startX - 1, y, z) <= 'z') {
		startX--;
	} 

	string out = "";
	while(startX == x || (getChar(startX, y, z) >= 'a' && getChar(startX, y, z) <= 'z')) {
		if(startX == x) {
			out += c;
			startX++;
		} else {
			out += getChar(startX++, y, z);
		}
	}

	return out;
}

string Board::findWordY(char c, int x, int y, int z) {
	int startY = y;
	//Traverse left
	while(getChar(x, startY - 1, z) >= 'a' && getChar(x, startY - 1, z) <= 'z') {
		startY--;
	} 

	string out = "";
	while(startY == y || (getChar(x, startY, z) >= 'a' && getChar(x, startY, z) <= 'z')) {
		if(startY == y) {
			out += c;
			startY++;
		} else {
			out += getChar(x, startY++, z);
		}
	}

	return out;
}

string Board::findWordZ(char c, int x, int y, int z) {
	int startZ = z;
	//Traverse left
	while(getChar(x, y, startZ - 1) >= 'a' && getChar(x, y, startZ - 1) <= 'z') {
		startZ--;
	} 

	string out = "";
	while(startZ == z || (getChar(x, y, startZ) >= 'a' && getChar(x, y, startZ) <= 'z')) {
		if(startZ == z) {
			out += c;
			startZ++;	
		} else {
			out += getChar(x, y, startZ++);
		}
	}

	return out;
}


//constructor
Board::Board() {
	//0, 0, 0 will be top left of the box; words will be read in a positive direction
	charBoard = new char**[_BOARD_SIZE];
	for(int i = 0; i < _BOARD_SIZE; i++) {
		charBoard[i] = new char*[_BOARD_SIZE];
				
		for(int j = 0; j < _BOARD_SIZE; j++) {
			charBoard[i][j] = new char[_BOARD_SIZE];
			
			//initialize each entry to '\0' (empty)
			for(int k = 0; k < _BOARD_SIZE; k++) {
				charBoard[i][j][k] = '\0';
			}
		}
	}

	//Calculate the center point
	center = (_BOARD_SIZE - 1) / 2;
			
	//Place the multipliers
	for(int i = 0; i < _BOARD_SIZE; i++) {
		for(int j = 0; j < _BOARD_SIZE; j++) {
			for(int	k = 0; k < _BOARD_SIZE; k++) {
				int distance = abs(center - i) + abs(center - j) + abs(center - k);
			//	if((i * j * k) % 4 == 0 || (i * j * k) % 5 == 0) {
					switch(distance){
						case 3:
							charBoard[i][j][k] = '2';
							break;
						case 7:
							charBoard[i][j][k] = '3';
							break;
						case 12: 
							charBoard[i][j][k] = '4';
							break;
						case 18:
							charBoard[i][j][k] = '5';
							break;
						default:
							break;
					}
			//	}				
			}			
		}
	}

	//Set center to '*'
	charBoard[center][center][center] = '*';
}

//Helper function to determine if location is a valid place to move
bool Board::isOpen(int x, int y, int z) {
	if(x < 0 || x >= _BOARD_SIZE || y < 0 || y >= _BOARD_SIZE || z < 0 || z >= _BOARD_SIZE) {
		//out of bounds
		return false;
	}

	if(charBoard[x][y][z] >= 'a' && charBoard[x][y][z] <= 'z') {
		//space has already been played
		return false;
	}

	return true;
}

//Helper method to place a character on the board
void Board::placeChar(char c, int x, int y, int z) {
	if(!isOpen(x, y, z)) {
		return;
	}			

	if(c < 'a' || c > 'z') {
		//not a valid character
		return;
	}
	charBoard[x][y][z] = c;
}	
		
//helper method to read a character at a position
//returns '!' if an invalid location is detected
char Board::getChar(int x, int y, int z) {
	if(x < 0 || x >= _BOARD_SIZE || y < 0 || y >= _BOARD_SIZE || z < 0 || z >= _BOARD_SIZE) {
		//out of bounds
		return '!';
	}

	return  charBoard[x][y][z];
}
		

//Returns whether a move is valid without consideration for whether the word itself is valid
// direction:
//	0 => x+
//	1 => y+
//	2 => z+
bool Board::canPlaceString(string play, int x, int y, int z, int direction) {
	static bool crossesExisting = false;

	switch(direction) {
		char prevChar;
		case 0:
			prevChar = getChar(x - 1, y, z);
			if(prevChar >= 'a' && prevChar <= 'z')
				crossesExisting = true;
			break;
		case 1:
			prevChar = getChar(x, y - 1, z);
			if(prevChar >= 'a' && prevChar <= 'z')
				crossesExisting = true;
			break;
		case 2:
			prevChar = getChar(x, y, z - 1);
			if(prevChar >= 'a' && prevChar <= 'z')
				crossesExisting = true;
			break;
	}

	if(play.length() == 0) {
		if(!crossesExisting) {
			//Have not crossed an existing word, yet. Character pointed to now must be alphabetical for
			//play to be legal
			char c = getChar(x, y, z);
			if(c >= 'a' && c <= 'z')
				crossesExisting = true;
		}
		if(crossesExisting) {
			//reset for next method call
			crossesExisting = false;
			return true;
		} else {
			return false;
		}
	}

	//If x,y,z is the center point, it crosses an existing word
	if(getChar(x, y, z) == '*') {
		crossesExisting = true;
	}

	if(x < 0 || x >= _BOARD_SIZE || y < 0 || y >= _BOARD_SIZE || z < 0 || z >= _BOARD_SIZE) {
		//out of bounds
		crossesExisting = false;
		return false;
	}
			
	if(play.at(0) == ' ') {
		//if the first character of our string is a space, the position on the board must be occupied
		//by an alphabetic character
		if(isOpen(x, y, z)) {
			//space is not alphabetical
			crossesExisting = false;
			return false;
		} 
	} else if(play.at(0) >= 'a' && play.at(0) <= 'z') {
		//First character is [a-z], space must be open
		if(!isOpen(x, y, z)) {
			//Space is already occupied by a character [a-z]
			crossesExisting = false;
			return false;
		}

		//Check to see if the we contact any letters on the cross axes
		char c;
		if(direction != 0) {
			if(((c = getChar(x - 1, y, z)) >= 'a' && c <= 'z') || ((c = getChar(x + 1, y, z)) >= 'a' && c <= 'z')) {
				crossesExisting = true;
			}
		}
		if(direction != 1) {
			if(((c = getChar(x, y - 1, z)) >= 'a' && c <= 'z') || ((c = getChar(x, y + 1, z)) >= 'a' && c <= 'z')) {
				crossesExisting = true;
			}

		}
		if(direction != 2) {
			if(((c = getChar(x, y, z - 1)) >= 'a' && c <= 'z') || ((c = getChar(x, y, z + 1)) >= 'a' && c <= 'z')) {
				crossesExisting = true;
			}

		}


	} else {
		//invalid character in the string
		crossesExisting = false;
		return false;
	}

	switch(direction) {
		case 0:
			return canPlaceString(play.substr(1, string::npos), x + 1, y, z, direction);
			break;
		case 1:
			return canPlaceString(play.substr(1, string::npos), x, y + 1, z, direction);
			break;
		case 2:
			return canPlaceString(play.substr(1, string::npos), x, y, z + 1, direction);
			break;
		default:
			//invalid direction
			crossesExisting = false;
			return false;
			break;
	}
}

//Given the word to be played and its location, returns the mutiplier the user would get for that move
//When multiple multipliers are encountered, their product is used
int Board::getMultiplier(string play, int x, int y, int z, int direction) {
	if(play.length() == 0) {
		return 1;
	}

	if(x < 0 || x >= _BOARD_SIZE || y < 0 || y >= _BOARD_SIZE || z < 0 || z >= _BOARD_SIZE) {
		//out of bounds
		return 0;
	}

	int multiplier = 1;

	if(play.at(0) >= 'a' && play.at(0) <= 'z') {
		switch(getChar(x, y, z)) {
			case '2':
				multiplier = 2;
				break;
			case '3':
				multiplier = 3;
				break;
			case '4':
				multiplier = 4;
				break;
			case '5':
				multiplier = 5;
				break;
			default:
				multiplier = 1;
		}
	}

	switch(direction) {
		case 0:
			return multiplier * getMultiplier(play.substr(1, string::npos), x + 1, y, z, direction);
			break;
		case 1:
			return multiplier * getMultiplier(play.substr(1, string::npos), x, y + 1, z, direction);
			break;
		case 2:
			return multiplier * getMultiplier(play.substr(1, string::npos), x, y, z + 1, direction);
			break;
		default:
			//invalid direction
			return 0;
			break;

	}
}

/* @method getPlayWordList
 * @brief	This function takes a play as input and returns a list of new words that would result from that play. 
 * 		The first element of the vector will be the word in the direction of the play, followed by words in
 *		the other 2 directions. These will only be present if the play creates more than 1 new word.
 *
 * @param string play	The word to be played. Will have already been verified as board legal by canPlaceString
 *			Characters already on the board will be spaces here. Leading and trailing spaces are not required.
 * @param int x, y, z	These refer to the starting position of the word
 * @param int direction	This tells the direction of the play. 0 is x, 1 is y, and 2 is z.
 */
vector<string> Board::getPlayWordList(string play, int x, int y, int z, int direction) {
	string onAxis = "";
	//read word on axis of play, filling in spaces with characters from the board
	//backtrack
	string prefix;
	switch(direction) {
		case 0:
			prefix = findWordX(play.at(0), x, y, z);
			break;
		case 1:
			prefix = findWordY(play.at(0), x, y, z);
			break;
		case 2:
			prefix = findWordZ(play.at(0), x, y, z);
			break;
		default:
			break;
	}
	
	//Our backtracking methods keep going until they hit a blank space on board. Spaces are blank spaces on the board,
	//so we have to increment past them
	int i = 1;
	while(play.at(i) == ' ') {
		i++;
	}
	//Add our prefix to our word.
	onAxis += prefix;	
	//starting at the index we found, continue to the end of the word
	for(i; i < play.length(); i++) {
		if(play.at(i) != ' ') {
			onAxis += play.at(i);
		} else {
			switch(direction) {
				case 0:
					onAxis += getChar(x + i, y, z);			
					break;
				case 1:
					onAxis += getChar(x, y + i, z);			
					break;
				case 2:
					onAxis += getChar(x, y, z + i);			
					break;
				default:
					//something went wrong
					vector<string> emptyVector;
					return emptyVector;
			}	
		}
	}

	//Now we need to find any suffixes if the exist on the board. 
	char c;
	switch(direction) {
		case 0:
			while((c = getChar(x + i++, y, z)) >= 'a' && c <= 'z') {
				onAxis += c;
			}
			break;
		case 1:
			while((c = getChar(x, y + i++, z)) >= 'a' && c <= 'z') {
				onAxis += c;
			}
			break;
		case 2:
			while((c = getChar(x, y, z + i++)) >= 'a' && c <= 'z') {
				onAxis += c;
			}
			break;
		default:
			break;
	}	

	vector<string> wordList;
	wordList.insert(wordList.begin(), onAxis);
	
	//get off-axis words
	vector<string> offAxis = getOffAxisWordList(play, x, y, z, direction);
	//Append them to our list
	wordList.insert(wordList.end(), offAxis.begin(), offAxis.end());

	/*	Testing		***************	
	cerr << "WordList: " << endl;
	for(i = 0; i < wordList.size(); i++) {
		cerr << wordList.at(i) << endl;
	}
	**************************************/
	cerr << "WordList: " << endl;
	for(i = 0; i < wordList.size(); i++) {
		cerr << wordList.at(i) << endl;
	}

	return wordList;
}

//String should be of the format "[a-z ]+" where spaces are for letters already on the board 
void Board::placeString(string play, int x, int y, int z, int direction) {
	if(play.length() == 0) {
		return;
	}

	if(!canPlaceString(play, x, y, z, direction)) {
		//move invalid
		return;
	}

	if(play.at(0) >= 'a' && play.at(0) <= 'z') {
		charBoard[x][y][z] = play.at(0);
	}

	switch(direction) {
		case 0:
			placeString(play.substr(1, string::npos), x + 1, y, z, direction);
			break;
		case 1:
			placeString(play.substr(1, string::npos), x, y + 1, z, direction);
			break;
		case 2:
			placeString(play.substr(1, string::npos), x, y, z + 1, direction);
			break;
		default:
			//invalid direction
			return;
			break;

	}
}

string Board::printBoard() {
	string out = "";
	for(int i = 0; i < _BOARD_SIZE; i++) {
		out += "Layer: ";
		out += to_string(i);
		out += '\n';
		for(int j = 0; j < _BOARD_SIZE; j++) {
			for(int k = 0; k < _BOARD_SIZE; k++) {
				if(getChar(i, j, k) == '\0') {
					out += '.';
				} else {
					out += getChar(i, j, k);
				}
					
				out += ' ';
			}
			out += '\n';
		}
				
		for(int z = 0; z < _BOARD_SIZE; z++) {
			out += '_';
		}
		out += '\n';
	}
	return out;
}

/*	Testing		*************************
 *int main() {
 *	Board board;
 *
 *	cout << board.printBoard() << endl;
 *
 *	//board.placeChar('a', 7, 7, 7);
 *	cout << board.printBoard() << endl;
 *
 *	cout << board.getMultiplier(" testing", 7, 7, 7, 1) << endl;
 *
 *	cout << board.canPlaceString("test", 7, 7, 7, 0) << endl;
 *
 *	cout << board.canPlaceString(" test", 7, 7, 7, 0) << endl;
 *
 *	board.placeString("test", 7, 7, 7, 0);
 *	board.placeString("est", 10, 8, 7, 1);
 *	board.placeString("est", 7, 8, 7, 1);
 *	
 *	cout << board.printBoard() << endl;
 *	
 *	cout << board.canPlaceString("sets", 7, 11, 7, 0) << endl;
 *	
 *	
 *	
 *	vector<string> wordList = board.getPlayWordList("sets", 7, 11, 7, 0);
 *	
 *	board.placeString("sets", 7, 11, 7, 0);
 *
 *	cout << "here" << endl;
 *	cout << board.printBoard() << endl;
 *
 *		
 *	cout << "WordList: " << endl;	
 *
 *	vector<string>::iterator it;
 *	for(it = wordList.begin(); it < wordList.end(); it++) {
 *		cout << *it << endl;
 *	}
 *
 *	board.placeString("tes ing", 11, 4, 7, 1);
 *
 *}
*/
