/**
 * @file main.cpp
 * @brief Carries out the main server side functions of a game instance using websockets
 * 
 * @author Eric Burghardt
 * @date 11/13/2017
 */

#include "../include/main.hpp"

using namespace std;
int main(int argc, char ** argv) {
	cout << "Argc: " << argc << endl;
	for(int i = 0; i < argc; i++) {
		cout << *(argv + i) << endl;
	}

	//main player1Name player1port player2Name player2Port
	if(argc < 3) {
		cerr << "Error: not enough arguments. Need at least 1 player" << endl;
		exit(1);		
	}

	cout << "Here 1" << endl;

	int numPlayers = (argc - 1) / 2;
	int ports[numPlayers];
	string playerNames[numPlayers];
	int index = 0;
	for(int i = 1; i < argc; i += 2) {
		playerNames[index] = string(argv[i]);
		ports[index] = atoi(argv[i + 1]);
		index++;
	}

	cout << "Here 2" << endl;

	vector<Player> players;
	for(int i = 0; i < numPlayers; i++) {
		Player player = Player(playerNames[i], ports[i]);
		players.insert(players.end(), player);
		cout << player.toString() << endl;
	}

	cout << "Here 3" << endl;	

	game = new Game(players);

	createThread(players.at(0));		
	while(1) {
		sleep(10000);
	}
}

void createThread(Player player) {
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, (void *(*)(void*))createHub, (void *) &player);
}

void createHub(Player * playerP) {
	uWS::Hub h;
	Player player = *playerP;	

	string portStr = to_string(player.getPort());

	cout << portStr << endl;
	
	h.onConnection([](uWS::WebSocket<uWS::CLIENT> * ws, uWS::HttpRequest req) {
		cout << "Here" << endl;
		ws->send("Connected");
	});	

	h.onMessage([&game, &player](uWS::WebSocket<uWS::SERVER> * ws, char * message, size_t length, uWS::OpCode opCode) {
		//parse message type
		string mess = string(message);
		//connect
		if(mess == "Connected") {
			/*
			string scores = "";
			string turn = "";
			string hand = "";
			string numTiles = "";
			
			scores += game->getScores();
			turn += game->getTurn();
			hand += player.getHandString();
			numTiles += game->getNumTiles();

			ws->send(scores.c_str(), sizeof(scores.c_str()), opCode);
			ws->send(turn.c_str(), sizeof(turn.c_str()), opCode);
			ws->send(hand.c_str(), sizeof(hand.c_str()), opCode);
			ws->send(numTiles.c_str(), sizeof(numTiles.c_str()), opCode);
			*/
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
		
			cout << play << x << y << z << dir;
	
			if(success) {
				ws->send(message, length, opCode);
			//	const char * newHand = "hand:newhand\n"; 
			//	ws->send(newHand, sizeof(newHand), opCode);
			} else {
				ws->send("Denied: Illegal\n", sizeof("Denied: Illegal\n"), opCode);
			}
		}
		
	});

	h.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char * message, size_t length) {
		exit(0);
	});

	if(h.listen(player.getPort()))
		h.run();
	
}

