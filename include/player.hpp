#ifndef PLAYER
#define PLAYER

#include <vector>


class Player {
	private:
		std::vector<char> hand;
		int score;
	
	public:
		Player();
		
		std::vector<char> getHand();
		int getHandSize();
		void addToHand(char c);

		int getScore();
		void setScore(int score);
		void addToScore(int points);
};

#endif
