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

		
		
}
