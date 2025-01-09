#include "agency.h"

pthread_mutex_t lock;

void *agency_member_function(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    AgencyMember *member = (AgencyMember *)arg;

    printf("Member %d: Active\n", member->id);
    while (1)
    {
        /* code */
    }

    // sleep(rand() % 5 + 1); // Simulate activity
    // if (rand() % 2)
    // {
    //     member->status = DEAD;
    //     printf("Member %d: Died\n", member->id);
    // }
    // else
    // {
    //     member->status = CAUGHT;
    //     printf("Member %d: Caught\n", member->id);
    // }
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
        printf("New Member %d added. Active members: %d\n", id, active_members);
    }
    pthread_mutex_unlock(&lock);
}
void *monitor_function()
{
    key_t key;
    int msgid;
    MonitorMessage msg;
    // Generate unique key
    key = ftok("progfile", SEED);
    // Get message queue identifier
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
        printf("Data received: member_id=%d, status=%d\n", msg.member_id, msg.status);

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
                printf("Member %d removed.\n", members[msg.member_id].id);
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

    // Start monitoring thread
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_function, NULL);

    // Wait for monitor thread
    pthread_join(monitor_thread, NULL);

    pthread_mutex_destroy(&lock);
}
