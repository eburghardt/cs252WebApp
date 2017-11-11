/**
 * @file dictionary.cpp
 * @brief This file uses a trie to implement a dictionary. Constructor takes path to a dictionary file
 *		as a parameter
 *
 * @author Eric Burghardt
 * @date 11/11/2017
 */

using namespace std;

const int ALPHABET_SIZE = 26; //26 characters in the english alphabet

typedef struct Node {
	Node * children[ALPHABET_SIZE]; //1 child node for each letter in the alphabet
	
	bool isWordEnd;	//If true, this node is the last character in a word. Parents are prefixes.
} Node;

Node * createNode() { //Constructor method
	Node * newNode = new Node;

	//initialize values for node
	newNode->isWordEnd = false;
	for(int i = 0; i < ALPHABET_SIZE; i++) {
		newNode->children[i] = NULL;
	}

	return newNode;
}


