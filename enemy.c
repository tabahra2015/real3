
#include "enemy.h"

#define DEAD 0
#define INJURED 1
#define CAUGHT 2

int select_member_to_target()
{
    int target_id = -1;
    int max_time = 0;

    while (1)
    {
        int random_index = rand() % MAX_MEMBERS;

        if (members[random_index].status != 1)
        {
            time_t current_time = time(NULL);
            int time_in_agency = (int)difftime(current_time, members[random_index].start_time);

            if (time_in_agency >= TIME_EGENY_THRESHOLD && time_in_agency > max_time)
            {
                max_time = time_in_agency;
                target_id = random_index;
                return target_id;
            }
        }
    }
}

void notify_monitor(int member_id, MemberStatus status)
{
    printf("Sending message to monitor\n");

    key_t key;
    int msgid;
    key = ftok("progfile", SEED);

    msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid < 0)
    {
        perror("msgget failed");
        exit(1);
    }
    MonitorMessage msg;
    msg.type = 1;
    msg.member_id = member_id;
    msg.status = status;

    // Send the message
    if (msgsnd(msgid, &msg, sizeof(MonitorMessage) - sizeof(long), 0) < 0)
    {
        perror("msgsnd failed");
        exit(1);
    }
    printf("Message sent: member_id=%d, status=%d\n", msg.member_id, msg.status);
}

// void enemy_function(int enemy_id)
// {
//     srand(time(NULL) + enemy_id);

//     while (1)
//     {
//         sleep(rand() % 40 + 5);
//         int target_id = select_member_to_target();
//         if (target_id != -1)
//         {
//             int event = (rand() +enemy_id ) % 3 ;
//             if (event == 0)
//             {
//                 members[target_id].status = DEAD;
//                // printf("Enemy: Member %d killed\n", target_id);
//             }
//             else if (event == 1)
//             {
//                 members[target_id].status = INJURED;
//               //  printf("Enemy: Member %d injured\n", target_id);
//             }
//             else if (event == 2)
//             {
//                 members[target_id].status = CAUGHT;
//               //  printf("Enemy: Member %d caught\n", target_id);
//             }
//            // printf("Enemy: Member %d removed from the agency.\n", target_id);
//         }
//         notify_monitor(target_id,members[target_id].status);
//         sleep(rand() % 10 + 3);
//     }
// }

void start_enemy_create()
{
    for (int i = 0; i < num_enemies; i++)
    {
        // Create a pipe for communication
        if (pipe(pipes[i]) == -1)
        {
            perror("Pipe creation failed");
            exit(1);
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process: close the write end of the pipe
            close(pipes[i][1]);
            enemy_function(i);
            exit(0);
        }
        else if (pid < 0)
        {
            perror("Fork failed");
            exit(1);
        }
        else
        {
            // Parent process: close the read end of the pipe
            close(pipes[i][0]);
            enemy_pids[i] = pid;
        }
    }
}

// Function for enemy's behavior, reading messages only
void enemy_function(int enemy_id)
{
    int group_data[MAX_GROUPS_define] = {0};
    MessageCitToRes message;

    while (1)
    {
        ssize_t bytes_read = read(pipes[enemy_id][0], &message, sizeof(MessageCitToRes));
        if (bytes_read > 0)
        {
            //printf("Enemy %d received: message_type=%ld, time_to_intercat=%d, id_cit=%d, id_res=%d, id_group=%d\n",enemy_id, message.message_type, message.time_to_intercat, message.id_cit, message.id_res, message.id_group);
            if (message.id_group >= 0 && message.id_group < MAX_GROUPS)
            {
                group_data[message.id_group] += (message.time_to_intercat );
                printf("Group %d total time_to_interact: %d\n", message.id_group, group_data[message.id_group]);
                if (group_data[message.id_group] >= 100)
                {
                    printf("Group %d has reached the target difference of .\n", message.id_group);
                    group_data[message.id_group] = 0;
                }
            }
            else
            {
                printf("Invalid group ID: %d\n", message.id_group);
            }
        }
        else
        {
            sleep(1);
        }
    }
}