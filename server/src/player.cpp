
#include "../include/player.hpp"
#include <iostream>

using namespace std;

Player::Player(string name, int port) {
	score = 0;
	this->name = name;
	this->port = port;
}

std::vector<char> * Player::getHand() {
	return &hand;
}

string Player::getHandString() {
	string out = "";
	for(int i = 0; i < hand.size(); i++) {
		out += hand.at(i);
	}
	return out;
}

int Player::getHandSize() {
	return hand.size();
}

void Player::addToHand(char c) {
	hand.insert(hand.end(), c);
}

int Player::getScore() {
	return this->score;
}

int Player::getPort() {
	return this->port;
}

void Player::setScore(int score) {
	this->score = score;
}

void Player::addToScore(int points) {
	cout << "Before score: " << this->score << endl;
	this->score += points;
	cout << "After score: " << this->score << endl;
}

string Player::getName() {
	return this->name;
}

string Player::toString() {
	string out = "Name: ";
	out += this->name;
	out += "\nScore: ";
	out += to_string(this->score);
	out += "\nPort: ";
	out += to_string(this->port);
	out += "\nHand: ";
	out += getHandString();
	return out;
}
