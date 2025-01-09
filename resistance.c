
#include "resistance.h"
pthread_mutex_t groups_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex to protect the groups array

// void *group_member_function(void *arg)
// {
//     // MemberInfo *member = (MemberInfo *)arg;
//     // int met_person_id = -1;
//     // float interaction_time = 0;

//     // while (1) {
//     //     met_person_id = rand() % MAX_PEOPLE;
//     //     if (pthread_mutex_trylock(&person_mutex[met_person_id]) == 0) {
//     //         if (person_busy[met_person_id] == 0) {
//     //             person_busy[met_person_id] = 1;
//     //             break;
//     //         } else {
//     //             pthread_mutex_unlock(&person_mutex[met_person_id]);
//     //         }
//     //     }
//     // }

//     // interaction_time = (float)(rand() % 10 + 1);
//     // sleep((int)interaction_time);

//     // pthread_mutex_lock(&person_mutex[met_person_id]);
//     // person_busy[met_person_id] = 0;
//     // pthread_mutex_unlock(&person_mutex[met_person_id]);

//     return NULL;
// }

void *group_member_function(void *arg)
{
    MemberInfo *member = (MemberInfo *)arg;

    while (1)
    {
        // member->interaction_time += 1.0; // Simulate interaction time
        // printf("Member %d (Type: %s) interacting. Total time: %.2f seconds.\n",
        //        member->member_id,
        //        member->member_type == RESISTANCE_MEMBER ? "Resistance Member" : "Spy",
        //        member->interaction_time);
        // sleep(1); // Simulate delay
    }

    return NULL;
}

void *spy_function(void *arg)
{
    Spy *spy = (Spy *)arg;

    while (1)
    {
        // spy->time_spent_in_group += 1.0; // Simulate time spent in group
        // printf("Spy ID %d in Group %d interacting with Enemy ID %d. Time spent: %.2f seconds.\n",
        //        spy->spy_id, spy->group_number, spy->enemy_id, spy->time_spent_in_group);
        // sleep(1); // Simulate delay
    }

    return NULL;
}

// Group process function
void group_process(ResistanceGroup *group)
{
    printf("Group %d process started (PID: %d) with %d members, Type: %s.\n",
           group->group_id, getpid(), group->group_size,
           group->group_type == SOCIAL ? "Social" : "Military");

    // Create threads for members
    pthread_t *threads = malloc(group->group_size * sizeof(pthread_t));
    Spy spy_data = {0};

    int spy_index = rand() % group->group_size;

    // Initialize members and assign roles
    for (int i = 0; i < group->group_size; i++)
    {
        group->members[i].member_id = i + 1;
        group->members[i].member_type = (i == spy_index) ? SPY : RESISTANCE_MEMBER;
        group->members[i].interaction_time = 0;
        group->members[i].busy=0;
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
            // Create thread for a regular member
            pthread_create(&threads[i], NULL, group_member_function, &group->members[i]);
        }
    }

    // Wait for all threads to complete (infinite for this example)
    for (int i = 0; i < group->group_size; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    free(threads);
}

void create_group()
{
    printf("Start new group\n\n\n");

    pthread_mutex_lock(&groups_mutex);

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
            // Child process
            int group_size = MIN_MEMBERS + (rand() % (MAX_MEMBERS - MIN_MEMBERS + 1));
            GroupType group_type = (rand() % 2 == 0) ? SOCIAL : MILITARY;
            float spy_target_probability = (group_type == MILITARY) ? SPY_TARGET_PROBABILITY : 0.3;
            ResistanceGroup group = {
                .group_id = groups_created + 1,
                .group_size = group_size,
                .group_type = group_type,
                .spy_target_probability = spy_target_probability};

            groups[groups_created] = group; // Save the group in the array
            group_process(&group);

            exit(0); // Child process exits after creating the group
        }
        else
        {
            // Parent process
            group_pids[groups_created] = pid; // Save the PID of the process
            groups_created++;                 // Increment group count after creation

            // Unlock mutex after accessing shared array
            pthread_mutex_unlock(&groups_mutex);
        }
    }
    else
    {
        printf("Maximum groups reached, cannot create more.\n");
        pthread_mutex_unlock(&groups_mutex);
    }
}