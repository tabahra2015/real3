
#include "enemy.h"

#define DEAD 0
#define INJURED 1
#define CAUGHT 2


int select_member_to_target()
{
    int target_id = -1;
    int max_time = 0;

    // Find the member with the longest time in the agency
    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        if (members[i].status != DEAD)
        {
            // Calculate the time spent in the agency by comparing current time with start_time
            time_t current_time = time(NULL);
            int time_in_agency = (int)difftime(current_time, members[i].start_time);

            // Select the member with the longest time in the agency that exceeds the user-defined threshold
            if (time_in_agency >= TIME_EGENY_THRESHOLD && time_in_agency > max_time)
            {
                max_time = time_in_agency;
                target_id = i;
            }
        }
    }

    return target_id;
}


void enemy_function()
{
    srand(time(NULL));

    while (1)
    {
        // Choose a random time to trigger an attack on a member
        sleep(rand() % 5 + 1);  // Random delay between 1 to 5 seconds

        // Select the member to attack based on the time spent in the agency
        int target_id = select_member_to_target();

        if (target_id != -1)
        {
            // Randomly determine the attack event (0 = kill, 1 = injure, 2 = capture)
            int event = rand() % 3;

            if (event == 0)
            {
                members[target_id].status = DEAD;
                printf("Enemy: Member %d killed\n", target_id);
            }
            else if (event == 1)
            {
                members[target_id].status = INJURED;
                printf("Enemy: Member %d injured\n", target_id);
            }
            else if (event == 2)
            {
                members[target_id].status = CAUGHT;
                printf("Enemy: Member %d caught\n", target_id);
            }

            // After the attack, the member is removed from active members
            printf("Enemy: Member %d removed from the agency.\n", target_id);

            // Simulate the enemy resting for a short period before attacking again
            sleep(rand() % 7+ 3);  // Random delay between attacks (1 to 3 seconds)
        }
    }
}

void start_enemy_create()
{
    int num_enemies = 6; 
    

    for (int i = 0; i < num_enemies; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            enemy_function();
            exit(0); 
        }
        else if (pid < 0)
        {
            perror("Fork failed");
            exit(1);
        }
        else
        {
            enemy_pids[i] = pid;
        }
    }

    for (int i = 0; i < num_enemies; i++)
    {
        waitpid(enemy_pids[i], NULL, 0); // Wait for each child process by its PID
    }
}

