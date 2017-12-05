
#include "../include/player.hpp"

using namespace std;

Player::Player(int port) {
	score = 0;
	this->port = port;
}

std::vector<char> Player::getHand() {
	return hand;
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

void Player::setScore(int score) {
	this->score = score;
}

void Player::addToScore(int points) {
	score += points;
}

string Player::toString() {
	string out = "Score: ";
	out += to_string(this->score);
	out += "\nPort: ";
	out += to_string(this->port);
	return out;
}
