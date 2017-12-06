CXX = g++ -std=c++11
LIBS = -lpthread -L. -luWS -lssl -lcrypto -lz

all: game server

server: ./src/server.cpp
	$(CXX) -g -o ./bin/$@ ./src/server.cpp -lnsl -lpthread

game: ./src/main.cpp
	$(CXX) -g -o ./bin/$@ ./src/main.cpp ./src/game.cpp ./src/dictionary.cpp ./src/board.cpp ./src/player.cpp ./src/bag.cpp $(LIBS)

clean: 
	rm ./bin/* 
