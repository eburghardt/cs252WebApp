/**
 * @file main.h
 *
 * @author Eric Burghardt
 * @date 11/13/2017
 */

#include "../include/game.hpp"
#include <uWS/uWS.h>
#include <iostream>
#include <vector>
#include <pthread.h>

Game * game;

void createThread(Player player);
void createHub(Player * player);
