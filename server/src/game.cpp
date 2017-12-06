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
Game::Game(vector<Player> players) {
	board = new Board;
	dictionary = Dictionary("./assets/dictionary.txt");
	bag = Bag();
	this->players = players;	
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

	return true;	
}

Board Game::getBoard() {
	return *board;
}

void Game::startGame() {
	
}

string Game::getScores() {
	string out = "scores;";
	for(int i = 0; i < players.size(); i++) {
		out += players.at(i).getName();
		out += ": ";
		out += players.at(i).getScore();
		out += ";";
	}
	return out;
}

string Game::getTurn() {
	return "";
}

string Game::getNumTiles() {
	return "" + bag.getNumTilesRemaining();
}

/*
int main() {
	Board board = Board();
	board.placeString("test", 7, 7, 7, 1);
	cout << board.printBoard() << endl;

//	Player player1 = Player();
//	Player player2 = Player();

//	player1.setScore(100);
	cout << player1.getScore() << endl;
	vector<Player> playerList = {player1, player2};
	//players.insert(players.end(), player1);
	//players.insert(players.end(), player2);
	
	Game game(playerList);
	
	Board gameBoard = game.getBoard();
	gameBoard.placeChar('i', 7, 11, 7);
	gameBoard.placeChar('n', 7, 12, 7);
	gameBoard.placeChar('g', 7, 13, 7);

	cout << "Board:\n" << game.getBoard().printBoard() << endl;

	game.play("test", 7, 7, 7, 1, playerList.front());
	cout << "Board: " << endl;
	cout << game.getBoard().printBoard() << endl;

	cout << "Score: " << playerList.front().getScore() << endl;


	//testBag
	Bag bag = Bag();
	while(!bag.isEmpty()) {
		cout << bag.getTile() << endl;
	}	

	exit(0);
}
*/
