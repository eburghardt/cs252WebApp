#ifndef PLAYER
#define PLAYER

#include <vector>
#include <string>

class Player {
	private:
		std::vector<char> hand;
		int score;
		int port;
		std::string name;	
	public:
		Player(std::string name, int port);
		
		std::vector<char> getHand();
		std::string getHandString();
		int getHandSize();
		void addToHand(char c);

		std::string getName();
		int getScore();
		int getPort();
		void setScore(int score);
		void addToScore(int points);
		std::string toString();
};

#endif
