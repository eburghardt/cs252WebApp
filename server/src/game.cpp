/*
 * @file game.cpp
 * @brief This class implements the logic behind a 3d scrabble game. 
 *
 * @author Eric Burghardt
 * @date 11/22/2017
 */

#include "../include/game.hpp"

using namespace std;

int Game::getLetterValue(char c) {
	switch(c) {
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'n':
		case 'r':
		case 't':
		case 'l':
		case 's':
		case 'u':
			return 1;
			break;
		case 'd':
		case 'g':
			return 2;
			break;
		case 'b':
		case 'c':
		case 'm':
		case 'p':
			return 3;
			break;
		case 'f':
		case 'h':
		case 'v':
		case 'w':
		case 'y':
			return 4;
			break;
		case 'k':
			return 5;
			break;
		case 'j':
		case 'x':
			return 8;
			break;
		case 'q':
		case 'z':
			return 10;
			break;
		default:
			return 0;
			break;
	}
}

int Game::getWordValue(string play) {
	int value = 0;
	for(int i = 0; i < play.length(); i++) {
		value += getLetterValue(play.at(i));
	}
	return value;
}

/**
 *
 *   Public
 *
 */

//constructor
// @param numPlayers = number of players
// @param players = pointer to array of player pointers
Game::Game(vector<Player> * players) {
	board = new Board;
	dictionary = Dictionary("./assets/dictionary.txt");
	bag = Bag();
	this->players = players;
	for(int i = 0; i < players->size(); i++) {
		initHand(players->at(i));
	}	
}

int Game::getPlayValue(string play, int x, int y, int z, int direction) {
	int multiplier = board->getMultiplier(play, x, y, z, direction);
	int base = getWordValue(play);
	return multiplier * base;
}		
		
/**
 * @method play
 * @brief Plays the word on the board in the given position if possible
 *
 * @param	play = word to be played. Contains only letters from player's hand and spaces
 * @param	x, y, z = position on board
 * @param	direction = axis on which the letters are being played 0 = x, 1 = y, 2 = z
 * @param	player = player who is playing the word
 *
 *
 * @return 	true if play is successful
 *		false if not legal
 */
bool Game::play(string play, int x, int y, int z, int direction, Player &player) {
	//Check legality of play
	if(!(board->canPlaceString(play, x, y, z, direction))) {
		return false;
	}
	
	std::vector<char> handCopy = *(player.getHand());
	//Check that hand has proper letters
	cout << "Player hand: " << player.getHandString() << endl;
	for(char c = 'a'; c <= 'z'; c++) {
		int numPlay = count(play.begin(), play.end(), c);
		int numHand = count(handCopy.begin(), handCopy.end(), c);
		cout << "char: " << c << "\nnumPlay: " << numPlay << "\nNumHand: " << numHand << endl;
		if(numPlay > numHand) {
			return false;
		}
	}

	//Check that all words are legal
	vector<string> wordList = board->getPlayWordList(play, x, y, z, direction);
	vector<string>::iterator it;
	for(it = wordList.begin(); it < wordList.end(); it++) {
		if(!dictionary.isWord(*it)) {
			return false;	
		}
	}

	//Play is legal. Add score and place it on the board
	int multiplier = board->getMultiplier(play, x, y, z, direction);
	int score = 0;
	for(it = wordList.begin(); it < wordList.end(); it++) {
		score += getWordValue(*it);
	}

	player.addToScore(score * multiplier);

	cout << "Calculated score: " << score * multiplier << endl;
	
	board->placeString(play, x, y, z, direction);

	cout << "Placed string" << endl;

	refreshHand(player, play);
	
	cout << "Refreshed hand" << endl;

	return true;	
}

Board Game::getBoard() {
	return *board;
}

void Game::startGame() {
	
}

void Game::refreshHand(Player &player, string play) {
	std::vector<char> * hand = player.getHand();

	for(int i = 0; i < play.length(); i++) {
		char c = play.at(i);
		if(c >= 'a' && c <= 'z')
			hand->erase(find(hand->begin(), hand->end(), c));
	}

	while(hand->size() < 7) {
		hand->insert(hand->end(), bag.getTile());
	}

	cout << player.toString() << endl;
}

void Game::initHand(Player &player) {
	for(int i = 0; i < 7; i++) {
		player.addToHand(bag.getTile());
	}
}

string Game::getScores() {
	string out = "scores;";
	for(int i = 0; i < players->size(); i++) {
		out += players->at(i).getName();
		out += ": ";
		out += to_string(players->at(i).getScore());
		cout << players->at(i).getScore() << endl;
		out += ";";
	}
	return out;
}

string Game::getTurn() {
	return "";
}

string Game::getNumTiles() {
	return to_string(bag.getNumTilesRemaining());
}


