#ifndef DICTIONARY
#define DICTIONARY

#include <string>	//string manipulation
#include <fstream>	//reading dictionary file
#include <iostream>	//I/O
#include <locale>	//locale, tolower

#define ALPHABET_SIZE 26 //26 characters in the english alphabet

typedef struct Node {
	Node * children[ALPHABET_SIZE]; //1 child node for each letter in the alphabet

	bool isWordEnd; //if true, this node is the last character in a word. Parents are prefixes.
} Node;

Node * createNode();
void add(Node * root, std::string word);
bool contains(Node * root, std::string word);

class Dictionary {
	private:
		Node * root;
		std::string fileName;
	public:
		Dictionary();
		Dictionary(std::string fn);
		bool isWord(std::string word);
};

#endif
