#include "resistance.h"

pthread_mutex_t queue_mutexes[MAX_MEMBERS_define];
MessageCitToRes queues[MAX_GROUPS_define];

void *group_member_function(void *arg) {
        int agency_msgid = msgget(AGENCY_MSG_KEY, IPC_CREAT | 0666);

    MemberInfo *member = (MemberInfo *)arg;
    int member_id = member->member_id;
    SharedMessage message; // Updated type

    while (1) {
        pthread_mutex_lock(&queue_mutexes[member_id]);
        if (queues[member_id].id_res != 0) {
            sleep(queues[member_id].time_to_intercat);
            message.id_res = queues[member_id].id_res;
            message.time_to_intercat = queues[member_id].time_to_intercat;
            message.group_num = queues[member_id].id_group;
            message.id_cit = queues[member_id].id_cit;
            ssize_t bytes_written = msgsnd(agency_msgid, &message, sizeof(SharedMessage) - sizeof(long), 0);
            if (bytes_written == -1) {
                perror("Failed to send message");
            } else {
                printf("tooooooo  agency Group member %d sent message: interact time = %d, group = %d, city id = %d\n", 
                    member_id, message.time_to_intercat, message.group_num, message.id_cit);
            }
            queues[member_id].id_res = 0; 
        }
        pthread_mutex_unlock(&queue_mutexes[member_id]);
        sleep(1);
    }
    return NULL;
}


void *spy_function(void *arg) {
    Spy *spy = (Spy *)arg;

    while (1) {
        // Generate a random time to sleep before sending a message (between 1 and 5 seconds)
        int random_time = rand() % 20 + 1;
        sleep(random_time);

        // Check if the current citizen is a spy and has a valid enemy ID
        if (citizens[spy->spy_id].member_type == 1) {
            MessageCitToRes msg;
            msg.id_res = spy->spy_id;
            msg.time_to_intercat = rand() % 10 + 1;  // random interaction time (1 to 10)
            msg.id_group = spy->group_number;

            ssize_t bytes_written = write(pipes[spy->enemy_id][1], &msg, sizeof(MessageCitToRes));
            // if (bytes_written > 0) {
            //     printf("Spy %d sent message to enemy %d: interact time = %d\n", spy->spy_id, spy->enemy_id, msg.time_to_intercat);
            // } else {
            //     printf("Failed to send message from Spy %d to enemy %d\n", spy->spy_id, spy->enemy_id);
            // }
        }
    }
}
void group_process(ResistanceGroup *group) {
    pthread_t *threads = malloc(group->group_size * sizeof(pthread_t));
    Spy spy_data = {0};
    int spy_index = rand() % group->group_size;

    for (int i = 0; i < group->group_size; i++) {
        group->members[i].member_id = i + 1;
        group->members[i].member_type = (i == spy_index) ? SPY : RESISTANCE_MEMBER;
        group->members[i].interaction_time = 0;
        group->members[i].busy = 0;

        if (i == spy_index) {
            spy_data.spy_id = group->members[i].member_id;
            spy_data.enemy_id = rand() % num_enemies + 1;
            spy_data.group_number = group->group_id;
            spy_data.time_spent_in_group = 0;
            pthread_create(&threads[i], NULL, spy_function, &spy_data);
        } else {
            pthread_create(&threads[i], NULL, group_member_function, &group->members[i]);
        }
    }

      while (1) {
        MessageCitToRes message;
        ssize_t bytes_read = read(pipesgroup[group->group_id][0], &message, sizeof(MessageCitToRes));
        if (bytes_read > 0) {
            message.id_group = group->group_id;
            if (message.id_res >= 0 && message.id_res < group->group_size) {
                pthread_mutex_lock(&queue_mutexes[message.id_res]);
                queues[message.id_res] = message;
                pthread_mutex_unlock(&queue_mutexes[message.id_res]);
            } else {
                printf("Invalid ID.\n");
            }
        } else {
            sleep(1);  
        }
    }

    free(threads);
}


void create_group()
{
    pthread_mutex_lock(&groups_mutex[0]);

    if (groups_created < MAX_GROUPS)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            int group_size = MIN_MEMBERS + (rand() % (MAX_MEMBERS - MIN_MEMBERS + 1));
            GroupType group_type = (rand() % 2 == 0) ? SOCIAL : MILITARY;
            float spy_target_probability = (group_type == MILITARY) ? SPY_TARGET_PROBABILITY : 0.3;
            ResistanceGroup group = {
                .group_id = groups_created,
                .group_size = group_size,
                .group_type = group_type,
                .spy_target_probability = spy_target_probability};

            groups[groups_created] = group;
            group_process(&group);
            exit(0);
        }
        else
        {
            group_pids[groups_created] = pid;
            groups_created++;
        }
    }
    else
    {
        printf("Maximum groups reached, cannot create more.\n");
    }

    pthread_mutex_unlock(&groups_mutex[0]);
}


