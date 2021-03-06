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

	game = new Game(&players);

	createThread(&(players.at(0)));		
	while(1) {
		sleep(10000);
	}
}

void createThread(Player * player) {
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, (void *(*)(void*))createHub, (void *) player);
}

void createHub(Player * playerP) {
	uWS::Hub h;
	Player * player = playerP;	

	string portStr = to_string(player->getPort());

	cout << portStr << endl;
	
	h.onConnection([](uWS::WebSocket<uWS::CLIENT> * ws, uWS::HttpRequest req) {
		cout << "Here" << endl;
		ws->send("Connected");
	});	

	h.onMessage([&game, player](uWS::WebSocket<uWS::SERVER> * ws, char * message, size_t length, uWS::OpCode opCode) {
		//parse message type
		string mess = string(message);
		//connect
		if(mess.length() >= 9 && mess.substr(0, 9) == "Connected") {
			cout << "\n\nPlayer connected\n\n" << endl;
			string scores = "";
			string turn = "";
			string hand = "hand:";
			string numTiles = "tiles:";
			
			scores += game->getScores();
			turn += game->getTurn();
			hand += player->getHandString();
			numTiles += game->getNumTiles();

			cout << "Scores: " << scores.c_str() << "\nTurn: " << turn.c_str() << "\nHand: " << hand.c_str() << "\nNumTiles: " << numTiles.c_str() << endl;

			ws->send(scores.c_str(), strlen(scores.c_str()), opCode);
			ws->send(turn.c_str(), strlen(turn.c_str()), opCode);
			ws->send(hand.c_str(), strlen(hand.c_str()), opCode);
			ws->send(numTiles.c_str(), strlen(numTiles.c_str()), opCode);
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

			
			bool success = game->play(play, x, y, z, dir, *player);
			cout << game->getScores() << endl;	
	
			cout << play << x << y << z << dir;
	
			if(success) {
				ws->send(message, length, opCode);
				ws->send(game->getScores().c_str(), game->getScores().length(), opCode);
				string hand = "hand:";
				hand += player->getHandString(); 
				ws->send(hand.c_str(), strlen(hand.c_str()), opCode);

				string tiles = "tiles:";
				tiles += game->getNumTiles();
				ws->send(tiles.c_str(), strlen(tiles.c_str()), opCode);
			} else {
				ws->send("Denied: Illegal\n", strlen("Denied: Illegal\n"), opCode);
			}
		}
		
	});

	h.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char * message, size_t length) {
		exit(0);
		
	});

	if(h.listen(player->getPort()))
		h.run();
	
}

