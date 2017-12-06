#ifndef PLAYER
#define PLAYER

#include <vector>
#include <string>

class Player {
	private:
		std::vector<char> hand;
		int score;
		int port;	
	public:
		Player(int port);
		
		std::vector<char> getHand();
		int getHandSize();
		void addToHand(char c);

		int getScore();
		void setScore(int score);
		void addToScore(int points);
		std::string toString();
};

#endif
