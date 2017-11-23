CXX = g++ -std=c++11

all: board dictionary

dictionary: ./src/dictionary.cpp
	$(CXX) -o ./bin/$@ ./src/dictionary.cpp

board: ./src/board.cpp
	$(CXX) -o ./bin/$@ ./src/board.cpp

clean: 
	rm ./bin/* 
