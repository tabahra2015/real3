#ifndef ENEMY_H
#define ENEMY_H

#include "local.h"

// Constants for enemy status
#define DEAD 0
#define INJURED 1
#define CAUGHT 2

void start_enemy_create();

void enemy_function(int enemy_id);

int select_member_to_target();

#endif // ENEMY_H
