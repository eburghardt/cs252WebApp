/**
 * @file main.h
 *
 * @author Eric Burghardt
 * @date 11/13/2017
 */

#include "../include/game.hpp"

Game * game;

void createThread(Player player);
void createHub(int port);

std::string getScores();
std::string getTurn();
std::string getNumTiles(); 
