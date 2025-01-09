#ifndef ENEMY_H
#define ENEMY_H
#include "local.h"

void select_member_to_target();
// Function to start the enemy's attack on agency members
void start_enemy_create();

// Function for the enemy to perform actions (kill, injure, or capture)
void enemy_function(void *arg);

#endif // ENEMY_H
