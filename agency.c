#include "agency.h"

pthread_mutex_t lock;
pthread_mutex_t shared_memory_mutex = PTHREAD_MUTEX_INITIALIZER;
int shm_id;
SharedMessage *shared_memory;
int message_index;

void create_shared_memory()
{
    shm_id = shmget(IPC_PRIVATE, sizeof(SharedMessage) * MAX_MESSAGES, IPC_CREAT | 0666);
    if (shm_id < 0)
    {
        perror("shmget failed");
        exit(1);
    }
    shared_memory = (SharedMessage *)shmat(shm_id, NULL, 0);
    if ((long)shared_memory == -1)
    {
        perror("shmat failed");
        exit(1);
    }
}

void *agency_member_function(void *arg)
{
    int agency_msgid = msgget(AGENCY_MSG_KEY, IPC_CREAT | 0666);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    create_shared_memory();

    while (1)
    {
        MessageCitToRes message;
        ssize_t bytes_read = msgrcv(agency_msgid, &message, sizeof(MessageCitToRes) - sizeof(long), 0, 0);
        if (bytes_read > 0)
        {
            printf("Agency received info: id_res = %d, interact time = %d group num = %d  memeber   =%d civilian = %d \n", message.id_res, message.time_to_intercat, message.id_group, message.id_res, message.id_cit);
            if (message_index < MAX_MESSAGES)
            {
                pthread_mutex_lock(&shared_memory_mutex);
                shared_memory[message_index].id_res = message.id_res;
                shared_memory[message_index].time_to_intercat = message.time_to_intercat;
                shared_memory[message_index].group_num = message.id_group;
                shared_memory[message_index].id_cit = message.id_cit;
                message_index++;
                if (message.time_to_intercat > SUSPICIOUS_TIME_THRESHOLD)
                {
                    printf("Suspicious interaction detected: Civ %d with Group %d\n",
                           message.id_cit, message.id_group);
                }

                pthread_mutex_unlock(&shared_memory_mutex);
            }
            else
            {
                printf("Shared memory is full, cannot store more messages.\n");
            }
        }
        sleep(1);
        updateTablesDataFile(); // Keep file updated
    }

    return NULL;
}

void analyze_group_interactions(int group_num)
{

    int citizen_time[TOTAL_MEMBERS_define] = {0};
    int employee_time[MAX_MEMBERS_define] = {0};
    int total_interactions = 0;

    for (int i = 0; i < message_index; i++)
    {
        //   printf(" to know Message %d: Group = %d, Citizen = %d, Employee = %d, Time = %d | ",
        //    i,
        //    shared_memory[i].group_num,
        //    shared_memory[i].id_cit,
        //    shared_memory[i].id_res,
        //    shared_memory[i].time_to_intercat);

        if (shared_memory[i].group_num == group_num)
        {
            citizen_time[shared_memory[i].id_cit] += shared_memory[i].time_to_intercat;
            employee_time[shared_memory[i].id_res] += shared_memory[i].time_to_intercat;
            total_interactions++;
        }
    }

    if (total_interactions < 10)
    {
        return;
    }

    int potential_spies[TOTAL_MEMBERS_define];
    int spy_count = 0;
    int highest_employee = 0;
    int highest_employee_id = -1;

    for (int i = 0; i < TOTAL_MEMBERS_define; i++)
    {
        if (citizen_time[i] > SUSPICIOUS_TIME_THRESHOLD)
        {
            potential_spies[spy_count++] = i;
        }
    }

    for (int i = 0; i < MAX_MEMBERS_define; i++)
    {
        if (employee_time[i] > highest_employee)
        {
            highest_employee = employee_time[i];
            highest_employee_id = i;
        }
    }

    const char *spy_status = (highest_employee > SUSPICIOUS_TIME_THRESHOLD) ? "The spy in employee is" : "Employee with the Most Time";
    const char *spy_status = (highest_employee > SUSPICIOUS_TIME_THRESHOLD) ? "The spy in employee is" : "Employee with the Most Time";

    // Print a line of underscores before the output
    printf("_____________________________________________________________\n");

    // Print the main output
    printf("Group %d | %s: %d, Time Spent: %d | Total Interactions: %d | Potential Spies: %d\n",
           group_num, spy_status, highest_employee_id, highest_employee, total_interactions, spy_count);

    // Print a line of underscores after the output
    printf("_____________________________________________________________\n");

    generate_chart(group_num, citizen_time, employee_time, spy_count, total_interactions);
}

void generate_chart(int group_num, int citizen_time[], int employee_time[], int spy_count, int total_interactions)
{
    int total_citizen_time = 0;
    for (int i = 0; i < TOTAL_MEMBERS_define; i++)
    {
        total_citizen_time += citizen_time[i];
    }

    int total_employee_time = 0;
    for (int i = 0; i < TOTAL_MEMBERS_define; i++)
    {
        total_employee_time += employee_time[i];
    }

    if (total_citizen_time + total_employee_time == 0)
    {
        return;
    }

    double citizen_percentage = (double)total_citizen_time / (total_citizen_time + total_employee_time) * 100;
    double employee_percentage = (double)total_employee_time / (total_citizen_time + total_employee_time) * 100;

    printf("Group %d | Citizens: %.2f%% | Employees: %.2f%% | Total Interactions: %d | Potential Spies: %d\n",
           group_num, citizen_percentage, employee_percentage, total_interactions, spy_count);
}

void add_new_member(int id)
{
    pthread_mutex_lock(&lock);

    if (active_members < MAX_MEMBERS)
    {
        members[id].id = id;
        members[id].status = ALIVE;
        members[id].start_time = time(NULL);

        if (active_members == 1)
        {
            pthread_create(&members[id].thread, NULL, analyze_group_interactions_thread, &members[id]);
        }
        else
        {
            pthread_create(&members[id].thread, NULL, agency_member_function, &members[id]);
        }

        active_members++;
    }

    pthread_mutex_unlock(&lock);
}

void *analyze_group_interactions_thread()
{
    int group_num;
    sleep(2);
    int sleep_time = rand() % 4 + 2;

    while (1)
    {
        sleep_time = rand() % 4 + 2;
        sleep(sleep_time);
        group_num = rand() % MAX_GROUPS_define;
        analyze_group_interactions(group_num);
    }

    return NULL;
}

void *monitor_function()
{
    key_t key;
    int msgid;
    MonitorMessage msg;
    key = ftok("progfile", SEED);
    msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid < 0)
    {
        perror("msgget failed");
        pthread_exit(NULL);
    }

    while (1)
    {
        if (msgrcv(msgid, &msg, sizeof(MonitorMessage) - sizeof(long), 0, 0) < 0)
        {
            perror("msgrcv failed");
            continue;
        }
        if (msg.member_id < 0 || msg.member_id >= MAX_MEMBERS)
        {
            printf("Invalid member_id: %d. Skipping.\n", msg.member_id);
        }
        else
        {
            if (msg.status == DEAD || msg.status == CAUGHT)
            {
                pthread_mutex_lock(&lock);

                if (pthread_cancel(members[msg.member_id].thread) == 0)
                {
                    printf("Cancellation request sent to member %d's thread.\n", msg.member_id);
                }
                else
                {
                    printf("Failed to send cancellation request to member %d's thread.\n", msg.member_id);
                }
                active_members--;

                // Wait for the thread to be canceled
                pthread_join(members[msg.member_id].thread, NULL);
                pthread_mutex_unlock(&lock);

                members[msg.member_id].status = ALIVE;
                add_new_member(msg.member_id);
            }
        }

        pthread_mutex_lock(&shared_memory_mutex);
        for (int i = 0; i < MAX_MESSAGES; i++)
        {
            if (shared_memory[i].time_to_intercat > SUSPICIOUS_TIME_THRESHOLD)
            {
                printf("Agency Monitor: Suspicious activity - Civ %d, Group %d\n",
                       shared_memory[i].id_cit, shared_memory[i].group_num);
            }
        }
        pthread_mutex_unlock(&shared_memory_mutex);

        sleep(5);
    }

    msgctl(msgid, IPC_RMID, NULL);
    return NULL;
}
void agency_process()
{
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        add_new_member(i);
    }

    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        pthread_join(members[i].thread, NULL);
    }
    pthread_mutex_destroy(&lock);
}
