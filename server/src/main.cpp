/**
 * @file main.cpp
 * @brief Carries out the main server side functions of a game instance using websockets
 * 
 * @author Eric Burghardt
 * @date 11/13/2017
 */

#include "../include/main.hpp"
#include "../include/game.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <vector>

using namespace std;
int main(int argc, char ** argv) {
	if(argc < 3) {
		cerr << "Error: not enough arguments. Need at least 2 players" << endl;
		exit(1);		
	}

	int numPlayers = argc - 1;
	int ports[numPlayers];
	for(int i = 1; i < argc; i++) {
		ports[i - 1] = atoi(argv[i]);
	}

	vector<Player> players;
	for(int i = 0; i < numPlayers; i++) {
		Player player = Player(ports[i]);
		players.insert(players.end(), player);
		cout << player.toString() << endl;
	}
	
	uWS::Hub h;

	
}
