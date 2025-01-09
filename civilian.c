#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "civilian.h"

float generate_interaction_time()
{
    return (rand() % 100) / 10.0; // Random float between 0.0 and 10.0
}

void handle_citizen_task(int citizen_id)
{
    // int key  = ftok("msgqueue", 65) + 3 ;

    // int msqid = msgget(key, 0666 | IPC_CREAT);
    // MessageCitToRes msg;

    // while (1)
    // {
    //     if (msgrcv(msqid, &msg, sizeof(msg) - sizeof(long), 1, 0) != -1)
    //     {
    //         printf("Received message for Citizen ID: %d   %d \n", msg.id_cit,citizen_id);
    //         printf("Interaction time: %d seconds\n", msg.time_to_intercat);
    //         citizens[msg.id_cit - 1].busy = 1;
    //         sleep(msg.time_to_intercat); 
    //         citizens[msg.id_cit - 1].busy = 0;
    //         printf("Citizen ID: %d is now free.\n", msg.id_cit);
    //     }
    //     else
    //     {
    //         perror("msgrcv failed");
    //     }
    // }

    // printf("Handling task for Citizen ID: %d\n", citizen_id);
}

void create_citizens()
{
    int spy_count = 0;
    int civilian_count = 0;

    for (int i = 0; i < TOTAL_MEMBERS; i++)
    {
        citizens[i].member_id = i + 1;
        if (spy_count < TOTAL_MEMBERS / 5 && rand() % 5 == 0)
        {
            citizens[i].member_type = SPY;
            spy_count++;
        }
        else
        {
            citizens[i].member_type = CIVILIAN;
            civilian_count++;
        }
        citizens[i].interaction_time = 0;
        citizens[i].busy = 0;
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("Fork failed");
            exit(1);
        }
        else if (pid == 0)
        {
            // Child process: Handle citizen-specific logic
            printf("Citizen ID: %d (PID: %d)\n", citizens[i].member_id, getpid());
            printf("Citizen Type: %d\n", citizens[i].member_type);
            printf("Interaction Time: %.2f\n", citizens[i].interaction_time);
            printf("Busy: %d\n", citizens[i].busy);
            handle_citizen_task(citizens[i].member_id);
            exit(0);
        }
        else
        {
            citizen_pids[i] = pid;
            continue; // Parent process keeps iterating over citizens and creating new forks
        }
    }

    printf("Created %d citizens: %d Civilians and %d Spies.\n", TOTAL_MEMBERS, civilian_count, spy_count);
}

// Function to simulate a citizen becoming busy (for example, interacting with another member)
void make_citizen_busy(int citizen_id)
{
    if (citizen_id >= 1 && citizen_id <= TOTAL_MEMBERS)
    {
        citizens[citizen_id - 1].busy = 1; // Mark citizen as busy
        printf("Citizen ID %d is now busy.\n", citizen_id);
    }
    else
    {
        printf("Invalid citizen ID.\n");
    }
}

// Function to simulate a citizen becoming not busy after some time
void make_citizen_not_busy(int citizen_id)
{
    if (citizen_id >= 1 && citizen_id <= TOTAL_MEMBERS)
    {
        citizens[citizen_id - 1].busy = 0; // Mark citizen as not busy
        printf("Citizen ID %d is now not busy.\n", citizen_id);
    }
    else
    {
        printf("Invalid citizen ID.\n");
    }
}
