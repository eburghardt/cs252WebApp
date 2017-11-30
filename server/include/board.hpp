#ifndef BOARD
#define BOARD

#include <cmath>
#include <string>
#include <iostream>
#include <vector>
#include <iterator>

#define _BOARD_SIZE 15

class Board {
	private:
		char *** charBoard; //Valid values for each slot are a-z for letters, 2-5 for multipliers, and * for the starting point
		int center;

		std::vector<std::string> getOffAxisWordList(std::string play, int x, int y, int z, int direction);
		std::string findWordX(char c, int x, int y, int z);
		std::string findWordY(char c, int x, int y, int z);
		std::string findWordZ(char c, int x, int y, int z);
		
	public:
		//constructor
		Board();
		bool isOpen(int x, int y, int z);
		void placeChar(char c, int x, int y, int z);
		char getChar(int x, int y, int z);
		bool canPlaceString(std::string play, int x, int y, int z, int direction);
		int getMultiplier(std::string play, int x, int y, int z, int direction);
		std::vector<std::string> getPlayWordList(std::string play, int x, int y, int z, int direction);
		void placeString(std::string play, int x, int y, int z, int direction);
		std::string printBoard();
};
	

#endif
