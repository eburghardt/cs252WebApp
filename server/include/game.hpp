#ifndef GAME
#define GAME

#include "../include/board.hpp"
#include "../include/dictionary.hpp"
#include "../include/player.hpp"
#include "../include/bag.hpp"

#include <vector>
#include <string>
#include <algorithm>

class Game {
	private:
		std::vector<Player> * players;
		Board * board;
		Dictionary dictionary;
		Bag bag;		

		int getLetterValue(char c);
		int getWordValue(std::string play);	
		void initHand(Player &player);
	public:
		Game(std::vector<Player> * players);
		
		int getPlayValue(std::string play, int x, int y, int z, int direction);
		bool play(std::string play, int x, int y, int z, int direction, Player &player);
		Board getBoard();

		void startGame();

		void refreshHand(Player &player, std::string play);
			
		std::string getScores();
		std::string getTurn();
		std::string getNumTiles();
};

#endif
