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
#include <pthread.h>

using namespace std;
int main(int argc, char ** argv) {
	//main player1Name player1port player2Name player2Port
	if(argc < 5) {
		cerr << "Error: not enough arguments. Need at least 2 players" << endl;
		exit(1);		
	}

	int numPlayers = (argc - 1) / 2;
	int ports[numPlayers];
	string playerNames[numPlayers];
	int index = 0;
	for(int i = 1; i < argc; i += 2) {
		playerNames[index] = string(argv[i]);
		ports[index] = atoi(argv[i + 1]);
		index++;
	}

	vector<Player> players;
	for(int i = 0; i < numPlayers; i++) {
		Player player = Player(playerNames[i], ports[i]);
		players.insert(players.end(), player);
		cout << player.toString() << endl;
	}
	

	
}

void createThread(Player player) {
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, (void *(*)(void*))createHub, (void *) player.getPort());
}

void createHub(int port) {
	uWS::Hub h;
	
	string portStr = to_string(port);
	h.onMessage([port](uWS::WebSocket<uWS::SERVER> * ws, char * message, size_t length, uWS::OpCode opCode) {
		//parse message type
		string mess = string(message);
		//connect
		if(mess == "Connected") {
			string scores = "";
			string turn = "";
			string hand = "";
			string numTiles = "";
			
			scores = getScores();
			turn = getTurn();
			hand = getHand(port);
			numTiles = getNumTiles();

			ws->send(scores, sizeof(scores), opCode);
			ws->send(turn, sizeof(turn), opCode);
			ws->send(hand, sizeof(hand), opCode);
			ws->send(numTiles, sizeof(numTiles), opCode);
		} else if(mess.substr(0, 5) == "play:") {
			size_t pos = mess.find(":");
			mess.erase(0, pos + 1);
			pos = mess.find(":");
			string play = mess.substr(0, pos);
			mess.erase(0, pos + 1);
			pos = mess.find(":");
			int x = stoi(mess.substr(0, pos));
			mess.erase(0, pos + 1);
			pos = mess.find(":");
			int y = stoi(mess.substr(0, pos));
			mess.erase(0, pos + 1);
			pos = mess.find(":");
			int z = stoi(mess.substr(0, pos));
			mess.erase(0, pos + 1);
			int dir = stoi(mess);

			
			bool success = game->play(play, x, y, z, dir, player);
			
			if(success) {
				ws->send(message, length, opCode);
			} else {
				ws->send("Denied: Illegal\n", sizeof("Denied: Illegal\n"), opCode);
			}
		}
		
	});

}

string getScores() {
	
}

string getTurn() {
	
}

string getNumTiles() {
	
}

string getHand(int port) {

}
