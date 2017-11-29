/**
 * @file dictionary.cpp
 * @brief This file uses a trie to implement a dictionary. Constructor takes path to a dictionary file
 *		as a parameter
 *
 * @author Eric Burghardt
 * @date 11/11/2017
 */

#include <string>	//string manipulations
#include <fstream>	//reading dictionary file
#include <iostream>	//I/O
#include <locale>	// locale, tolower

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

//Add a word to the trie if not present
void add(Node * root, string word) {
	Node * current = root;
	locale loc;
	for(int i = 0; i < word.length(); i++) {
		int currChar = tolower(word[i], loc) - 'a'; //Convert current character to index
		if(currChar < 0 || currChar >= ALPHABET_SIZE) {
			cout << "Something happened during insert\n";
			return;
		}
		
		if(!current->children[currChar]) //Need to create a new node
			current->children[currChar] = createNode();
		
		current = current->children[currChar]; //Set current node to index of current character
	}
	//Mark last node as a word. May not be a leaf because may be a prefix.
	current->isWordEnd = true;
}

//Check if a word exists in the trie
//Returns true if word exists
//Returns false if word does not exist
bool contains(Node * root, string word) {
	Node * current = root;
	locale loc;
	for(int i = 0; i < word.length(); i++) {
		int currChar = tolower(word[i], loc) - 'a'; //Convert current character to index
		if(currChar < 0 || currChar >= ALPHABET_SIZE) {
			cout << "Something happened during search\n";
			return false;
		}

		if(!current->children[currChar]) //If node is not present
			return false;

		//node is present, move to next character
		current = current->children[currChar];
	}
	return (current != NULL && current->isWordEnd);
}





class Dictionary {
	private:
		Node * root;
		string fileName;	
	public:
		Dictionary(string fn) {
			fileName = fn;
			root = createNode();

			//Build trie
			
			//First, open file, then, line by line, read the words into the trie
			ifstream infile(fn.c_str());
			string line;
			while(getline(infile, line)) {
				add(root, line);
			}
			
		}

		//Returns true if the word is in the dictionary
		bool isWord(string word) {
			return contains(root, word);
		}		
};


int main(int argc, char ** argv) {
	//For testing purposes
	//Dictionary dictionary("../assets/dictionary.txt");
	Dictionary dictionary("./assets/dictionary.txt");

	for(int i = 0; i < argc; i++) {
		if(i == 0)
			continue;
		cout << "Is word: " << argv[i] << "?\n";
		cout << dictionary.isWord(argv[i]) << "\n";
	}


}
