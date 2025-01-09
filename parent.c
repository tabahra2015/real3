#include "local.h"
#include "civilian.h"
//#include "enemy.h"

#define LINE_MAX_LENGTH 256
#define DEFAULT_ARGUMENT_FILE "arguments.txt"
#define MAX_PEOPLE 1000
int person_busy[MAX_PEOPLE] = {0};
pthread_mutex_t person_mutex[MAX_PEOPLE];
void read_arguments(char *argument_file);
void *group_member_function(void *arg);
void *agency_member_function(void *arg);
void group_process(ResistanceGroup *group);
void agency_process();
void create_group();
void start_group_creation_timer();
void alarm_handler(int sig);

int MAX_GROUPS = 10;
int MIN_MEMBERS = 3;
int MAX_MEMBERS = 100;
int AGENCY_MEMBERS = 50;
int GROUP_CREATION_INTERVAL = 5;
int CIVILIAN_COUNT = 30;
float SPY_TARGET_PROBABILITY = 0.5f;
int TIME_EGENY_THRESHOLD =10;

AgencyMember members[100];

void initialize_person_locks()
{
    for (int i = 0; i < MAX_PEOPLE; i++)
    {
        pthread_mutex_init(&person_mutex[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    char *argument_file;

    if (argc == 2)
    {
        argument_file = argv[1];
    }
    else
    {
        printf("No argument file provided. Using default: %s\n", DEFAULT_ARGUMENT_FILE);
        argument_file = DEFAULT_ARGUMENT_FILE;
    }

    read_arguments(argument_file);
    initialize_person_locks();
    // Fork the agency process
    pid_t agency_pid = fork();
    if (agency_pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (agency_pid == 0)
    {
        printf("Agency process started (PID: %d).\n", getpid());
        agency_process();
        exit(0);
    }
    else
    {
        printf("Agency process PID: %d\n", agency_pid);
    }
     //this in order to create the enemy 
     start_enemy_create();
    while (1)
    {
        /* code */
    }
    
    //     // Fork the civilian process
    // pid_t civilian_pid = fork();
    // if (civilian_pid < 0)
    // {
    //     perror("Fork failed");
    //     exit(EXIT_FAILURE);
    // }
    // else if (civilian_pid == 0)
    // {
    //     printf("Civilian process started (PID: %d).\n", getpid());
    //     civilian_process();
    //     exit(0);
    // }
    // else
    // {
    //     printf("Civilian process PID: %d\n", civilian_pid);
    // }

    //     // Start the group creation timer
    //     start_group_creation_timer();

    //     // Wait for all child processes
    //     waitpid(agency_pid, NULL, 0);
    //     waitpid(civilian_pid, NULL, 0);
    //     while (1)
    //     {
    //         pause(); // Wait for signals
    //     }

    //     printf("All processes completed.\n");
    //     return 0;
}

void read_arguments(char *argument_file)
{
    FILE *file = fopen(argument_file, "r");
    if (file == NULL)
    {
        printf("Failed to open the file: %s\n", argument_file);
        exit(EXIT_FAILURE);
    }
    char line[LINE_MAX_LENGTH];
    memset(line, 0x0, LINE_MAX_LENGTH);
    while (fgets(line, LINE_MAX_LENGTH, file) != NULL)
    {
        char *token = strtok(line, " ");
        if (strcmp(token, "MAX_GROUPS") == 0)
        {
            token = strtok(NULL, " ");
            MAX_GROUPS = atoi(token);
        }
        else if (strcmp(token, "MIN_MEMBERS") == 0)
        {
            token = strtok(NULL, " ");
            MIN_MEMBERS = atoi(token);
        }
        else if (strcmp(token, "MAX_MEMBERS") == 0)
        {
            token = strtok(NULL, " ");
            MAX_MEMBERS = atoi(token);
        }
        else if (strcmp(token, "AGENCY_MEMBERS") == 0)
        {
            token = strtok(NULL, " ");
            AGENCY_MEMBERS = atoi(token);
        }
        else if (strcmp(token, "GROUP_CREATION_INTERVAL") == 0)
        {
            token = strtok(NULL, " ");
            GROUP_CREATION_INTERVAL = atoi(token);
        }
        else if (strcmp(token, "CIVILIAN_COUNT") == 0)
        {
            token = strtok(NULL, " ");
            CIVILIAN_COUNT = atoi(token);
        }
        else if (strcmp(token, "SPY_TARGET_PROBABILITY") == 0)
        {
            token = strtok(NULL, " ");
            SPY_TARGET_PROBABILITY = atof(token);
        }
         else if (strcmp(token, "TIME_EGENY_THRESHOLD") == 0)
        {
            token = strtok(NULL, " ");
            TIME_EGENY_THRESHOLD = atoi(token);
        }

    }
    fclose(file);
}

void notify_monitor(int member_id, MemberStatus status) {
    printf("Sending message to monitor\n");

    key_t key;
    int msgid;
    key = ftok("progfile", SEED);

    msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid < 0) {
        perror("msgget failed");
        exit(1);
    }
    MonitorMessage msg;
    msg.type = 1; 
    msg.member_id = member_id;
    msg.status = status;

    // Send the message
    if (msgsnd(msgid, &msg, sizeof(MonitorMessage) - sizeof(long), 0) < 0) {
        perror("msgsnd failed");
        exit(1);
    }

    printf("Message sent: member_id=%d, status=%d\n", msg.member_id, msg.status);
}


// void group_process(ResistanceGroup *group)
// {
//     pthread_t threads[group->group_size];
//     printf("Group %d process started (PID: %d) with %d members, Type: %s.\n",  group->group_id, getpid(), group->group_size, group->group_type == SOCIAL ? "Social" : "Military");

//     // Ensure that there is at most one spy in the group
//     int spy_index = rand() % group->group_size;  // Randomly select one index for the spy
//     for (int i = 0; i < group->group_size; i++)
//     {
//         group->members[i].member_type = (i == spy_index) ? SPY : NORMAL_MEMBER;
//     }

//     if (group->group_type == MILITARY)
//     {
//         // Simulate the chance of being targeted by the enemy
//         float random_value = (float)rand() / RAND_MAX;
//         if (random_value < group->spy_target_probability)
//         {
//             printf("Group %d (Military) has been targeted by the enemy.\n", group->group_id);
//             // Handle the consequences of being targeted (e.g., loss, capturing, etc.)
//         }
//         else
//         {
//             printf("Group %d (Military) is safe for now.\n", group->group_id);
//         }
//     }

//     // Continue processing the group
//     for (int i = 0; i < group->group_size; i++)
//     {
//         if (pthread_create(&group->members[i].thread_id, NULL, group_member_function, &group->members[i].member_id) != 0)
//         {
//             perror("Failed to create group thread");
//             exit(EXIT_FAILURE);
//         }
//     }

//     for (int i = 0; i < group->group_size; i++)
//     {
//         pthread_join(group->members[i].thread_id, NULL);
//         printf("Group member %d (Thread ID: %lu) has completed their task.\n", group->members[i].member_id, group->members[i].thread_id);
//     }

//     printf("Group %d process completed.\n", group->group_id);
// }

// void agency_process()
// {
//     pthread_t threads[AGENCY_MEMBERS];
//     int member_ids[AGENCY_MEMBERS];
//     printf("Agency process started (PID: %d).\n", getpid());

//     for (int i = 0; i < AGENCY_MEMBERS; i++)
//     {
//         member_ids[i] = i + 1;
//         if (pthread_create(&threads[i], NULL, agency_member_function, &member_ids[i]) != 0)
//         {
//             perror("Failed to create agency thread");
//             exit(EXIT_FAILURE);
//         }
//     }

//     for (int i = 0; i < AGENCY_MEMBERS; i++)
//     {
//         pthread_join(threads[i], NULL);
//     }

//     printf("Agency process completed.\n");
// }

// void civilian_process()
// {
//     pthread_t threads[CIVILIAN_COUNT];
//     int member_ids[CIVILIAN_COUNT];
//     printf("Civilian process started (PID: %d).\n", getpid());

//     // for (int i = 0; i < CIVILIAN_COUNT; i++)
//     // {
//     //     member_ids[i] = i + 1;
//     //     if (pthread_create(&threads[i], NULL, civilian_function, &member_ids[i]) != 0)
//     //     {
//     //         perror("Failed to create civilian thread");
//     //         exit(EXIT_FAILURE);
//     //     }
//     // }

//     for (int i = 0; i < CIVILIAN_COUNT; i++)
//     {
//         pthread_join(threads[i], NULL);
//     }

//     printf("Civilian process completed.\n");
// }

// void *agency_member_function(void *arg)
// {
//     int member_id = *(int *)arg;
//     printf("Agency member %d working (Thread ID: %lu).\n", member_id, pthread_self());
//     sleep(1);
//     return NULL;
// }

// void *civilian_function(void *arg)
// {
//     int member_id = *(int *)arg;
//     printf("Civilian %d working (Thread ID: %lu).\n", member_id, pthread_self());
//     sleep(1);
//     return NULL;
// }

// void create_group()
// {
//     if (groups_created < MAX_GROUPS)
//     {
//         pid_t pid = fork();
//         if (pid < 0)
//         {
//             perror("Fork failed");
//             exit(EXIT_FAILURE);
//         }
//         else if (pid == 0)
//         {
//             int group_size = MIN_MEMBERS + (rand() % (MAX_MEMBERS - MIN_MEMBERS + 1));
//             ResistanceGroup group = {
//                 .group_id = groups_created + 1,
//                 .group_size = group_size,
//                 .group_type = (rand() % 2 == 0) ? SOCIAL : MILITARY,
//                 .spy_target_probability = ((rand() % 2 == 0) ? SOCIAL : MILITARY) == MILITARY
//                                               ? SPY_TARGET_PROBABILITY // Higher probability for military groups
//                                               : .3                     // Default probability for social groups
//             };

//             for (int i = 0; i < group_size; i++)
//             {
//                 group.members[i].member_id = i + 1;
//                 group.members[i].thread_id = pthread_self();
//             }

//             printf("Group %d process will have %d members, Type: %s.\n",
//                    group.group_id, group.group_size, group.group_type == SOCIAL ? "Social" : "Military");

//             group_process(&group);
//             exit(0);
//         }
//         else
//         {
//             printf("Group %d process PID: %d\n", groups_created + 1, pid);
//             groups_created++;
//         }
//     }
// }

// void alarm_handler(int sig)
// {
//     create_group();
//     if (groups_created < MAX_GROUPS)
//     {
//         int next_interval = 1 + (rand() % 5); // Random interval between 1 and 5 seconds
//         alarm(next_interval);
//         printf("Next group will be created in %d seconds.\n", next_interval);
//     }
// }

// void start_group_creation_timer()
// {
//     struct sigaction sa;
//     sa.sa_handler = alarm_handler;
//     sa.sa_flags = 0;
//     sigemptyset(&sa.sa_mask);

//     if (sigaction(SIGALRM, &sa, NULL) == -1)
//     {
//         perror("sigaction failed");
//         exit(EXIT_FAILURE);
//     }

//     alarm(GROUP_CREATION_INTERVAL);
// }