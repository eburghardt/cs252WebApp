/**
 * @file board.cpp
 * @brief This class implements the 3d character array we will use to store the board as well as
 * 	some helper functions to aid in placing new characters in and reading the board.
 *
 * @author Eric Burghardt
 * @date 11/22/2017
 */
#include <cmath>
#include <string>
#include <iostream>

#define _BOARD_SIZE 15

using namespace std;

class Board {
	private: 
		//Valid values for each slot are a-z for letters and '2', '3', '4', and '5' for multipliers, '*' for the starting point
		//before the first move
		char *** charBoard;
		int center;
	public:
		//constructor
		Board() {
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
					}			
				}
			}

			//Set center to '*'
			charBoard[center][center][center] = '*';
		}

		//Helper function to determine if location is a valid place to move
		bool isOpen(int x, int y, int z) {
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
		void placeChar(char c, int x, int y, int z) {
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
		char getChar(int x, int y, int z) {
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
		bool canPlaceString(string play, int x, int y, int z, int direction) {
			if(play.length() == 0) {
				return true;
			}

			if(x < 0 || x >= _BOARD_SIZE || y < 0 || y >= _BOARD_SIZE || z < 0 || z >= _BOARD_SIZE) {
				//out of bounds
				return false;
			}
			
			if(play.at(0) == ' ') {
				//if the first character of our string is a space, the position on the board must be occupied
				//by an alphabetic character
				if(isOpen(x, y, z)) {
					//space is not alphabetical
					return false;
				} 
			} else if(play.at(0) >= 'a' && play.at(0) <= 'z') {
				//First character is [a-z], space must be open
				if(!isOpen(x, y, z)) {
					//Space is already occupied by a character [a-z]
					return false;
				}
			} else {
				//invalid character in the string
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
					return false;
					break;
			}
		}

		//String should be of the format "[a-z ]+" where spaces are for letters already on the board 
		void placeString(string play, int x, int y, int z, int direction) {
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

		string printBoard() {
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
};

int main() {
	Board board;

	cout << board.printBoard() << endl;

	board.placeChar('a', 7, 7, 7);
	cout << board.printBoard() << endl;


	cout << board.canPlaceString("test", 7, 7, 7, 0) << endl;

	cout << board.canPlaceString(" test", 7, 7, 7, 0) << endl;

	board.placeString(" test", 7, 7, 7, 0);
	cout << board.printBoard() << endl;

	board.placeString("tes ing", 11, 4, 7, 1);
	cout << board.printBoard() << endl;
}
