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
		Dictionary dictionary;		

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

		int getWordValue(string play) {
			int value = 0;
			for(int i = 0; i < play.length(); i++) {
				value += getLetterValue(play.at(i));
			}
			return value;
		}
	public:
		//constructor
		// @param numPlayers = number of players
		// @param players = pointer to array of player pointers
		Game(int numPlayers, Player ** players) {
			board = new Board(15);
			dictionary = new Dictionary("./assets/dictionary.txt");
			
		}

		int getPlayValue(string play, int x, int y, int z, int direction) {
			int multiplier = board.getMultiplier(play, x, y, z, direction);
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
		 *
		 * @return 	true if play is successful
		 *		false if not legal
		 */
		bool play(string play, int x, int y, int z, int direction) {
			//Check legality of play
			
		}
}
