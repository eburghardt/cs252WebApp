/**
 * @file game.cpp
 * @brief This class implements the logic behind a 3d scrabble game. 
 *
 * @author Eric Burghardt
 * @date 11/22/2017
 */



class Game {
	private:
		Player ** players;
		Board board;
	public:
		//constructor
		// @param numPlayers = number of players
		// @param players = pointer to array of player pointers
		Game(int numPlayers, Player ** players) {
			board = new Board(15);
				
		}

		int getLetterValue(char c) {
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

		
		
}
