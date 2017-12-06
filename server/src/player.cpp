
#include "../include/player.hpp"

using namespace std;

Player::Player(string name, int port) {
	score = 0;
	this->name = name;
	this->port = port;
}

std::vector<char> Player::getHand() {
	return hand;
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
	return score;
}

int Player::getPort() {
	return port;
}

void Player::setScore(int score) {
	this->score = score;
}

void Player::addToScore(int points) {
	score += points;
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
	return out;
}
