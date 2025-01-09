#ifndef ENEMY_H
#define ENEMY_H

#include "local.h"

// Constants for enemy status
#define DEAD 0
#define INJURED 1
#define CAUGHT 2

// Function to start the enemy's attack on agency members
void start_enemy_create();

// Function for the enemy to perform actions (kill, injure, or capture)
void enemy_function(int enemy_id);

// Function to select a member to target based on the longest time spent in the agency
int select_member_to_target();

#endif // ENEMY_H
