CXX = g++ -std=c++11

dictionary: ./src/dictionary.cpp
	$(CXX) -o ./bin/$@ ./src/dictionary.cpp 
