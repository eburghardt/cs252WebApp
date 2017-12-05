/**
 * @file bag.cpp
 * @brief This file models a bag of tiles. Players will get tiles from here, and the game will end when the bag is empty.
 *
 * @author Eric Burghardt
 * @date 11/30/2017
 */

#include "../include/bag.hpp"
#include <ctime>
#include <cstdlib>

Bag::Bag() {
	add('a', 9 * 15);
	add('b', 2 * 15);
	add('c', 2 * 15);
	add('d', 4 * 15);
	add('e', 12 * 15);
	add('f', 2 * 15);
	add('g', 3 * 15);
	add('h', 2 * 15);
	add('i', 9 * 15);
	add('j', 1 * 15);
	add('k', 1 * 15);
	add('l', 4 * 15);
	add('m', 2 * 15);
	add('n', 6 * 15);
	add('o', 8 * 15);
	add('p', 2 * 15);
	add('q', 1 * 15);
	add('r', 6 * 15);
	add('s', 4 * 15);
	add('t', 6 * 15);
	add('u', 4 * 15);
	add('v', 2 * 15);
	add('w', 2 * 15);
	add('x', 1 * 15);
	add('y', 2 * 15);
	add('z', 1 * 15);

	//seed random number generator
	srand((unsigned)time(NULL));	
}

void Bag::add(char c, int num) {
	for(int i = 0; i < num; i++) {
		tiles.insert(tiles.end(), c);
	}
}

char Bag::getTile() {
	//randomly select a tile, swap it with the back, pop the back and return it
	int random = rand() % tiles.size();	
	char out = tiles.at(random);
	tiles.erase(tiles.begin() + random);
	return out;
}

int Bag::getNumTilesRemaining() {
	return tiles.size();
}

bool Bag::isEmpty() {
	return tiles.empty();
}
