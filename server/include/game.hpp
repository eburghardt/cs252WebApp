#ifndef GAME
#define GAME

#include "../include/board.hpp"
#include "../include/dictionary.hpp"
#include "../include/player.hpp"
#include "../include/bag.hpp"

#include <vector>
#include <string>

class Game {
	private:
		std::vector<Player> players;
		Board board;
		Dictionary dictionary;
		
		int getLetterValue(char c);
		int getWordValue(std::string play);	

	public:
		Game(std::vector<Player>);
		
		int getPlayValue(std::string play, int x, int y, int z, int direction);
		bool play(std::string play, int x, int y, int z, int direction, Player &player);
		Board getBoard();

		void startGame();
};

#endif
