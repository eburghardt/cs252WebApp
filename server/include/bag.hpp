/**
 * @file bag.hpp
 * @brief This file models a bag of tiles. Players will get tiles from here,
 *		and the game will end when the bag is empty.
 *
 * @author Eric Burghardt
 * @date 11/30/2017
 */

class Bag {
	private:
		
	public:
		Bag();
		char getTile();
		int getNumTiles();
};
