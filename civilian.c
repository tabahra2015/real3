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
    int key = ftok("msgqueue", 65) + citizen_id;

    int msqid = msgget(key, 0666 | IPC_CREAT);
    MessageCitToRes msg;

    while (1)
    {
        if (msgrcv(msqid, &msg, sizeof(msg) - sizeof(long), 1, 0) != -1)
        {
            printf("Received message for Citizen ID: %d   %d  %d \n", msg.id_cit, citizen_id, citizens[citizen_id].member_type);
            // printf("Interaction time: %d seconds\n", msg.time_to_intercat);
            citizens[msg.id_cit - 1].busy = 1;
            sleep(msg.time_to_intercat);
            citizens[msg.id_cit - 1].busy = 0;
            // printf("Citizen ID: %d is now free.\n", msg.id_cit);

            if (citizens[citizen_id].member_type == 1)
            {
                ssize_t bytes_written = write(pipes[spy[citizen_id].enemy_id][1], &msg, sizeof(MessageCitToRes));
                if (bytes_written > 0)
                {
                   printf("Citizen   to enemy : Sent message to Enemy %d: message_type=%ld, time_to_intercat=%d, id_cit=%d, id_res=%d, id_group=%d\n",
                            spy[citizen_id].enemy_id, msg.message_type, msg.time_to_intercat, msg.pid_group, msg.id_res, msg.id_group);
                }

                else
                {
                    perror("Sender: Error writing to pipe");
                }
            }
        }
        else
        {
            perror("msgrcv failed");
        }
        // updateTablesDataFile(); // Keep file updated
    }

    printf("Handling task for Citizen ID: %d\n", citizen_id);
}

// typedef struct
// {
//     int enemy_id;
//     int spy_id;
//     int group_number;
//     float time_spent_in_group;
// } Spy;
void create_citizens()
{
    int spy_count = 0;
    int civilian_count = 0;

    printf(" start create new new \n");
    for (int i = 1; i < TOTAL_MEMBERS; i++)
    {
        citizens[i].member_id = i;
        if (spy_count < TOTAL_MEMBERS / 2 && rand() % 3 == 0)
        {
            citizens[i].member_type = SPY;
            spy[i].enemy_id = rand() % num_enemies + 1;   
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

            handle_citizen_task(citizens[i].member_id);
            exit(0);
        }
        else
        {
            citizen_pids[i] = pid;
            continue;
        }
    }
}

void make_citizen_busy(int citizen_id)
{
    if (citizen_id >= 1 && citizen_id <= TOTAL_MEMBERS)
    {
        citizens[citizen_id - 1].busy = 1;
        printf("Citizen ID %d is now busy.\n", citizen_id);
    }
    else
    {
        printf("Invalid citizen ID.\n");
    }
}

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
