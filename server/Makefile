CXX = g++ -std=c++11

all: game server

server: ./src/server.cpp
	$(CXX) -o ./bin/$@ ./src/server.cpp -lnsl -lpthread

game: ./src/game.cpp
	$(CXX) -g -o ./bin/$@ ./src/game.cpp ./src/dictionary.cpp ./src/board.cpp ./src/player.cpp ./src/bag.cpp

clean: 
	rm ./bin/* 
