
#include "../include/player.hpp"

Player::Player() {
	score = 0;
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
