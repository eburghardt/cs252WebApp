CXX = g++ -std=c++11

all: game

game: ./src/game.cpp
	$(CXX) -g -o ./bin/$@ ./src/game.cpp ./src/dictionary.cpp ./src/board.cpp ./src/player.cpp

clean: 
	rm ./bin/* 
