#include "local.h"
#include "civilian.h"
#include "enemy.h"
#include "resistance.h"
#include <pthread.h>

#define LINE_MAX_LENGTH 256
#define DEFAULT_ARGUMENT_FILE "arguments.txt"
#define MAX_PEOPLE 1000
int person_busy[MAX_PEOPLE] = {0};
pthread_mutex_t person_mutex[MAX_PEOPLE];
pthread_t opengl_thread;
void read_arguments(char *argument_file);
void *group_member_function(void *arg);
void *agency_member_function(void *arg);
void group_process(ResistanceGroup *group);
void agency_process();
void start_group_creation_timer();
void alarm_handler();
void send_message_to_random_citizen();
#define KILL_INTERVAL 5
int MAX_GROUPS = 10;
int MIN_MEMBERS = 3;
int MAX_MEMBERS = 100;
int AGENCY_MEMBERS = 50;
int TOTAL_MEMBERS = 100;
int GROUP_CREATION_INTERVAL = 5;
int CIVILIAN_COUNT = 30;
float SPY_TARGET_PROBABILITY = 0.5f;
int TIME_EGENY_THRESHOLD = 10;
pid_t enemy_pids[6];
AgencyMember members[100];
ResistanceGroup *groups;
pid_t group_pids[MAX_GROUPS_define];
pid_t citizen_pids[TOTAL_MEMBERS_define];
int groups_created = 0;
int num_enemies = 6;
int pipes[MAX_ENEMIES][2];
int pipesgroup[MAX_GROUPS_define][2];
int pipe_fd[2];
int pipe_fd2[2];

Spy spy[TOTAL_MEMBERS_define];
pthread_mutex_t *groups_mutex;
Citizen citizens[TOTAL_MEMBERS_define];

pthread_t opengl_thread;

void initialize_person_locks()
{
    for (int i = 0; i < MAX_PEOPLE; i++)
    {
        pthread_mutex_init(&person_mutex[i], NULL);
    }
}
void setup_shared_memory()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }

    size_t size = sizeof(ResistanceGroup) * MAX_GROUPS + sizeof(pthread_mutex_t);
    if (ftruncate(shm_fd, size) == -1)
    {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }
    groups = (ResistanceGroup *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (groups == MAP_FAILED)
    {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }
    groups_mutex = (pthread_mutex_t *)((char *)groups + sizeof(ResistanceGroup) * MAX_GROUPS);
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(groups_mutex, &attr);

    groups_created = 0;
}
// Function for OpenGL rendering thread
void *startOpenGL(void *arg) {
    initGraphics(0, NULL); // Initialize and start OpenGL rendering
    return NULL;
}

void clearTablesDataFile() {
    const char *filename = "tables_data.txt";
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to clear file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    printf("tables_data.txt cleared successfully.\n");
}


int main(int argc, char *argv[])
{
    initialize_semaphore(); // Initialize semaphore

    // Start the OpenGL thread
    if (pthread_create(&opengl_thread, NULL, startOpenGL, NULL) != 0) {
        perror("Failed to create OpenGL thread");
        return EXIT_FAILURE;
    }

    clearTablesDataFile(); // Optional: explicitly clear at the start

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
    setup_shared_memory();
    pid_t agency_pid = fork();
    if (agency_pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (agency_pid == 0)
    {
        agency_process();
        exit(0);
    }
    start_enemy_create();
    start_group_creation_timer();
    create_citizens();
    send_message_to_random_citizen();
    while (1)
    {
    }

    for (int i = 0; i < TOTAL_MEMBERS; i++)
    {
        wait(NULL);
    }
    // Step 8: Clean up OpenGL thread
    printf("Terminating OpenGL thread...\n");
    pthread_cancel(opengl_thread);
    pthread_join(opengl_thread, NULL);

    printf("Program exiting cleanly.\n");

    destroy_semaphore(); // Destroy semaphore    

    return EXIT_SUCCESS;
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
            MAX_GROUPS = 5;
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

    for (int i = 0; i < MAX_GROUPS_define; i++)
    {
        if (pipe(pipesgroup[i]) == -1)
        {
            perror("Failed to create pipe");
            exit(EXIT_FAILURE);
        }
    }
}

void send_message_to_random_citizen()
{
    int msqid;
    key_t key = ftok("msgqueue", 65);
    int selected_citizen = 0;
    int selected_member=0, selected_group=0;
    MemberInfo *target_citizen;
    MessageCitToRes msg;

    fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);

    while (1)
    {
        int sleep_time = rand() % 5 + 1;
        sleep(sleep_time);

        do
        {
            selected_citizen = rand() % TOTAL_MEMBERS + 1;
        } while (citizens[selected_citizen - 1].busy == 1);

        do
        {
            if (groups_created == 0)
                continue;
            selected_group = rand() % groups_created;
            if (groups[selected_group].group_size == 0)
                continue;
            selected_member = rand() % groups[selected_group].group_size;
            target_citizen = &groups[selected_group].members[selected_member];
        } while (groups_created == 0 || target_citizen->busy == 1);

        key = ftok("msgqueue", 65) + (key_t)selected_citizen;
        msqid = msgget(key, 0666 | IPC_CREAT);

        msg.message_type = 1;
        msg.id_cit = selected_citizen;
        msg.id_res = selected_member;
        msg.id_group = selected_group;
        msg.time_to_intercat = rand() % 5 + 6; // Random time to interact between 6 and 10
        msg.pid_group = group_pids[selected_group];

        if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) == -1)
        {
            perror("msgsnd failed");
        }

        ssize_t bytes_written = write(pipesgroup[selected_group][1], &msg, sizeof(MessageCitToRes));
        if (bytes_written == -1)
        {
            perror("write failed");
        }
    }
}

void select_and_kill_group()
{
    int shm_fd = shm_open("/shared_memory", O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }

    size_t shm_size = sizeof(int) + MAX_GROUPS * sizeof(pid_t);
    void *shm_ptr = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }

    int *groups_created = (int *)shm_ptr;
    pid_t *group_pids = (pid_t *)(shm_ptr + sizeof(int));

    pthread_mutex_lock(&groups_mutex[0]);

    if (*groups_created > 0)
    {
        int random_index = rand() % (*groups_created);
        pid_t target_pid = group_pids[random_index];

        if (kill(target_pid, SIGKILL) == 0)
        {
            printf("Group with PID %d has been killed.\n", target_pid);

            // Remove the killed group from the list
            for (int i = random_index; i < *groups_created - 1; i++)
            {
                group_pids[i] = group_pids[i + 1];
            }
            (*groups_created)--;
        }
        else
        {
            perror("Failed to kill group");
        }
    }
    else
    {
        printf("No groups available to kill.\n");
    }

    pthread_mutex_unlock(&groups_mutex[0]);

    munmap(shm_ptr, shm_size);
    close(shm_fd);
}

void alarm_handler()
{
    static int iterations = 0;

    create_group();
    iterations++;
    if (groups_created < MAX_GROUPS)
    {
        int next_interval = 1 + (rand() % 5);
        alarm(0);
        alarm(next_interval);
    }
}

void start_group_creation_timer()
{
    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }

    alarm(GROUP_CREATION_INTERVAL);
}

void cleanup_and_exit()
{
    printf("Cleaning up resources...\n");

    // Cancel and join all agency member threads
    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        if (members[i].status != DEAD && members[i].status != CAUGHT)
        {
            pthread_cancel(members[i].thread);
            pthread_join(members[i].thread, NULL);
        }
    }

    pthread_join(opengl_thread, NULL);
    
    printf("Simulation terminated cleanly.\n");
    exit(0);
}


void updateTablesDataFile() {
    const char *filename = "tables_data.txt";

    // Acquire the semaphore
    sem_wait(file_semaphore);
    // Open the file in write mode to clear it
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    }

    // Write Table 1
    fprintf(file, "# Table 1:\n");
    fprintf(file, "# Format: ResID, Res Destiny, percentage\n");
    for (int i = 0; i < group_count; i++) {
        fprintf(file, "%d,%s,%.2f%%\n", ResistanceGroup[i].members.member_id, //res ID
                .status, // res destuny
                .members.spy_target_probability * 100);// percentage
    }

    // Write Table 2
    fprintf(file, "\n# Table 2:\n");
    fprintf(file, "# Format: ResID, citizen ID, Duration\n");
    for (int i = 0; i < member_count; i++) {
        fprintf(file, "%d,%d,%d minutes\n", 
                .id, //res ID
                .id, //citizen id
                ResistanceGroup.members.interaction_time); // duration
    }

    fclose(file);

    // Release the semaphore
    sem_post(file_semaphore);
    printf("tables_data.txt updated successfully.\n");
    printf("Simulation terminated cleanly.\n");
    exit(0);
}
