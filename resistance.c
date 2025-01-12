#include "resistance.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

pthread_mutex_t queue_mutexes[MAX_MEMBERS_define];
MessageCitToRes queues[MAX_GROUPS_define];

void handle_signal(int sig, siginfo_t *si, void *uc)
{
    ResistanceGroup *current_group = (ResistanceGroup *)si->si_value.sival_ptr;

    for (int i = 0; i < current_group->group_size; i++)
    {
        MemberInfo *target = &current_group->members[i];

        if (!target->active)
            continue;

        int outcome = rand() % 4 + 1;

        pthread_mutex_lock(&target->lock);

        switch (outcome)
        {
        case LIGHT_INJURY:
            target->injury_status = LIGHT_INJURY;
            target->active = 0;
            pthread_mutex_unlock(&target->lock);
            sleep(20);
            target->injury_status = 0;
            target->active = 1;
            break;

        case SEVERE_INJURY:
            target->injury_status = SEVERE_INJURY;
            target->active = 0;
            pthread_mutex_unlock(&target->lock);
            break;

        case MEMBER_CAUGHT:
            target->active = 0;
            pthread_mutex_unlock(&target->lock);
            break;

        case KILLED:
            target->active = 0;
            pthread_mutex_unlock(&target->lock);
            break;

        default:
            pthread_mutex_unlock(&target->lock);
            break;
        }

        for (int j = 0; j < current_group->group_size; j++)
        {
            pthread_mutex_lock(&current_group->members[j].lock);
            if (!current_group->members[j].active)
            {
                printf("New member joins as replacement for member %d.\n", target->member_id);
                current_group->members[j].member_id = target->member_id;
                current_group->members[j].injury_status = 0;
                current_group->members[j].active = 1;
                pthread_mutex_unlock(&current_group->members[j].lock);
                break;
            }
            pthread_mutex_unlock(&current_group->members[j].lock);
        }
    }
}

void *group_member_function(void *arg)
{
    int agency_msgid = msgget(AGENCY_MSG_KEY, IPC_CREAT | 0666);
    if (agency_msgid == -1)
    {
        perror("msgget failed");
        return NULL;
    }

    MemberInfo *member = (MemberInfo *)arg;
    int member_id = member->member_id;
    SharedMessage message;

    while (1)
    {
        pthread_mutex_lock(&queue_mutexes[member_id]);
        if (queues[member_id].id_res != 0)
        {
            sleep(queues[member_id].time_to_intercat);
            message.id_res = queues[member_id].id_res;
            message.time_to_intercat = queues[member_id].time_to_intercat;
            message.group_num = queues[member_id].id_group;
            message.id_cit = queues[member_id].id_cit;
            ssize_t bytes_written = msgsnd(agency_msgid, &queues[member_id], sizeof(MessageCitToRes) - sizeof(long), 0);
            if (bytes_written == -1)
            {
                perror("Message send failed");
            }
            else
            {
                // printf("Agency: Group member %d sent message: Interaction time = %d, Group = %d, City ID = %d\n",
                    //   member_id, message.time_to_intercat, message.group_num, message.id_cit);
            }
            queues[member_id].id_res = 0;
        }
        pthread_mutex_unlock(&queue_mutexes[member_id]);
        sleep(1);
    }
    return NULL;
}

void *spy_function(void *arg)
{
    Spy *spy = (Spy *)arg;
    while (1)
    {
        int random_time = rand() % 20 + 1;
        sleep(random_time);

        if (citizens[spy->spy_id].member_type == 1)
        {
            MessageCitToRes msg;
            msg.id_res = spy->spy_id;
            msg.time_to_intercat = rand() % 10 + 1;
            msg.id_group = spy->group_number;
            //msg.pid_group=getpid();
           // ssize_t bytes_written = write(pipes[spy->enemy_id][1], &msg, sizeof(MessageCitToRes));
            // if (bytes_written == -1)
            // {
            //     perror("Spy message send failed");
            // }
        }
    }
}

void group_process(ResistanceGroup *group)
{
    pthread_t *threads = malloc(group->group_size * sizeof(pthread_t));
    if (threads == NULL)
    {
        perror("Memory allocation failed");
        return;
    }

    Spy spy_data = {0};
    int spy_index = rand() % group->group_size;

    for (int i = 0; i < group->group_size; i++)
    {
        group->members[i].member_id = i + 1;
        group->members[i].member_type = (i == spy_index) ? SPY : RESISTANCE_MEMBER;
        group->members[i].interaction_time = 0;
        group->members[i].busy = 0;
        group->members[i].active = 1;

        if (i == spy_index)
        {
            spy_data.spy_id = group->members[i].member_id;
            spy_data.enemy_id = rand() % num_enemies + 1;
            spy_data.group_number = group->group_id;
            spy_data.time_spent_in_group = 0;
            pthread_create(&threads[i], NULL, spy_function, &spy_data);
        }
        else
        {
            pthread_create(&threads[i], NULL, group_member_function, &group->members[i]);
        }
    }

    struct sigaction sa;
    sa.sa_sigaction = handle_signal; // Your signal handler function
    sa.sa_flags = SA_SIGINFO;        // Ensure the handler gets siginfo (info about the signal)
    sigaction(SIGUSR1, &sa, NULL);   // Register the handler for SIGUSR1

    while (1)
    {
        MessageCitToRes message;
        ssize_t bytes_read = read(pipesgroup[group->group_id][0], &message, sizeof(MessageCitToRes));
        if (bytes_read > 0)
        {
            message.id_group = group->group_id;
            if (message.id_res >= 0 && message.id_res < group->group_size)
            {
                pthread_mutex_lock(&queue_mutexes[message.id_res]);
                queues[message.id_res] = message;
                pthread_mutex_unlock(&queue_mutexes[message.id_res]);
            }
            else
            {
                printf("Invalid ID.\n");
            }
        }
        else
        {
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
            printf("  the pid_=%d",pid);
            groups_created++;
        }
    }
    else
    {
        printf("Maximum number of groups reached, cannot create more.\n");
    }

    pthread_mutex_unlock(&groups_mutex[0]);
}
