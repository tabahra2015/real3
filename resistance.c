
#include "resistance.h"


void *group_member_function(void *arg)
{
    // MemberInfo *member = (MemberInfo *)arg;
    // int met_person_id = -1;
    // float interaction_time = 0;

    // while (1) {
    //     met_person_id = rand() % MAX_PEOPLE;
    //     if (pthread_mutex_trylock(&person_mutex[met_person_id]) == 0) {
    //         if (person_busy[met_person_id] == 0) {
    //             person_busy[met_person_id] = 1;
    //             break;
    //         } else {
    //             pthread_mutex_unlock(&person_mutex[met_person_id]);
    //         }
    //     }
    // }

    // interaction_time = (float)(rand() % 10 + 1);
    // sleep((int)interaction_time);

    // pthread_mutex_lock(&person_mutex[met_person_id]);
    // person_busy[met_person_id] = 0;
    // pthread_mutex_unlock(&person_mutex[met_person_id]);

    return NULL;
}


void group_process(ResistanceGroup *group)
{
    pthread_t threads[group->group_size];
    printf("Group %d process started (PID: %d) with %d members, Type: %s.\n", group->group_id, getpid(), group->group_size, group->group_type == SOCIAL ? "Social" : "Military");

    // Ensure that there is at most one spy in the group
    int spy_index = rand() % group->group_size;
    for (int i = 0; i < group->group_size; i++)
    {
        group->members[i].member_type = (i == spy_index) ? SPY : RESISTEANCE_MEMBER;
    }

    if (group->group_type == MILITARY)
    {
        float random_value = (float)rand() / RAND_MAX;
        if (random_value < group->spy_target_probability)
        {
            printf("Group %d (Military) has been targeted by the enemy.\n", group->group_id);
        }
        else
        {
            printf("Group %d (Military) is safe for now.\n", group->group_id);
        }
    }

    for (int i = 0; i < group->group_size; i++)
    {
        pthread_create(&group->members[i].thread_id, NULL, group_member_function, &group->members[i].member_id);
    }

    for (int i = 0; i < group->group_size; i++)
    {
        pthread_join(group->members[i].thread_id, NULL);
    }

    printf("Group %d process completed.\n", group->group_id);
}



void create_group()
{

    printf(" start new group \n\n\n");
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
                .group_id = groups_created + 1,
                .group_size = group_size,
                .group_type = group_type,
                .spy_target_probability = spy_target_probability
            };
            groups[groups_created] = group;  // Save the group in the array
            group_process(&group);
            exit(0);
        }
        else
        {
            group_pids[groups_created] = pid; // Save the PID of the process
            groups_created++;
        }
    }
}
