#include "agency.h"

pthread_mutex_t lock;

pthread_mutex_t shared_memory_mutex = PTHREAD_MUTEX_INITIALIZER;

int shm_id;
SharedMessage *shared_memory;
void create_shared_memory() {
    shm_id = shmget(IPC_PRIVATE, sizeof(SharedMessage) * MAX_MESSAGES, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }
    shared_memory = (SharedMessage *)shmat(shm_id, NULL, 0);
    if ((long)shared_memory == -1) {
        perror("shmat failed");
        exit(1);
    }
}
void *agency_member_function(void *arg) {
    int agency_msgid = msgget(AGENCY_MSG_KEY, IPC_CREAT | 0666);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    AgencyMember *member = (AgencyMember *)arg;
    create_shared_memory();
    int message_index = 0;
    while (1) {
        MessageCitToRes message;
        ssize_t bytes_read = msgrcv(agency_msgid, &message, sizeof(MessageCitToRes) - sizeof(long), 0, 0);
        if (bytes_read > 0) {
            printf("Agency received info: id_res = %d, interact time = %d\n", message.id_res, message.time_to_intercat);
            if (message_index < MAX_MESSAGES) {
                pthread_mutex_lock(&shared_memory_mutex); // Lock the shared memory mutex
                shared_memory[message_index].id_res = message.id_res;
                shared_memory[message_index].time_to_intercat = message.time_to_intercat;
                message_index++;
                pthread_mutex_unlock(&shared_memory_mutex); // Unlock the shared memory mutex
            } else {
                printf("Shared memory is full, cannot store more messages.\n");
            }
        }
        sleep(1);
    }
    return NULL;
}

void add_new_member(int id)
{
    pthread_mutex_lock(&lock);
    if (active_members < MAX_MEMBERS)
    {
        members[id].id = id;
        members[id].status = ALIVE;
        members[id].start_time = time(NULL);
        pthread_create(&members[id].thread, NULL, agency_member_function, &members[id]);
        active_members++;
    }
    pthread_mutex_unlock(&lock);
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
    printf("Monitor is working\n");

    while (1)
    {
        // Receive the message
        if (msgrcv(msgid, &msg, sizeof(MonitorMessage) - sizeof(long), 0, 0) < 0)
        {
            perror("msgrcv failed");
            continue;
        }

        // Display the received message
        printf("\nData received: member_id=%d, status=%d\n", msg.member_id, msg.status);

        // Validate member_id
        if (msg.member_id < 0 || msg.member_id >= MAX_MEMBERS)
        {
            printf("Invalid member_id: %d. Skipping.\n", msg.member_id);
        }
        else
        {
            if (msg.status == DEAD || msg.status == CAUGHT)
            {
                pthread_mutex_lock(&lock);

                // Cancel the member's thread
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
               // printf("Member %d removed.\n", members[msg.member_id].id);
                members[msg.member_id].status = ALIVE;
                add_new_member(msg.member_id);
            }
        }
    }

    msgctl(msgid, IPC_RMID, NULL);
    return NULL;
}

// first function call
void agency_process()
{
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

    // Initialize agency members
    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        add_new_member(i);
    }

    // Star thread
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_function, NULL);

    // Wait for monitor thread
    pthread_join(monitor_thread, NULL);

    pthread_mutex_destroy(&lock);
}
