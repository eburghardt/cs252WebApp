/**
 * @file bag.cpp
 * @brief This file models a bag of tiles. Players will get tiles from here, and the game will end when the bag is empty.
 *
 * @author Eric Burghardt
 * @date 11/30/2017
 */

#include "../include/bag.hpp"

Bag::Bag() {
	tiles.add('a', 9 * 15);
	tiles.add('b', 2 * 15);
	tiles.add('c', 2 * 15);
	tiles.add('d', 4 * 15);
	tiles.add('e', 12 * 15);
	tiles.add('f', 2 * 15);
	tiles.add('g', 3 * 15);
	tiles.add('h', 2 * 15);
	tiles.add('i', 9 * 15);
	tiles.add('j', 1 * 15);
	tiles.add('k', 1 * 15);
	tiles.add('l', 4 * 15);
	tiles.add('m', 2 * 15);
	tiles.add('n', 6 * 15);
	tiles.add('o', 8 * 15);
	tiles.add('p', 2 * 15);
	tiles.add('q', 1 * 15);
	tiles.add('r', 6 * 15);
	tiles.add('s', 4 * 15);
	tiles.add('t', 6 * 15);
	tiles.add('u', 4 * 15);
	tiles.add('v', 2 * 15);
	tiles.add('w', 2 * 15);
	tiles.add('x', 1 * 15);
	tiles.add('y', 2 * 15);
	tiles.add('z', 1 * 15);
}

void Bag::add(char c, int num) {
	for(int i = 0; i < num; i++) {
		tiles.insert(tiles.end, c);
	}
}

char Bag::getTile() {
	//randomly select a tile, swap it with the back, pop the back and return it
		
}

int Bag::getNumTilesRemaining() {
	return tiles.size();
}

bool Bag::isEmpty() {
	return tiles.empty();
}
