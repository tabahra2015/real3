
#include "resistance.h"
pthread_mutex_t groups_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex to protect the groups array


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


void group_process(ResistanceGroup *group) {
    printf("Group %d process started (PID: %d) with %d members, Type: %s.\n", 
           group->group_id, getpid(), group->group_size, 
           group->group_type == SOCIAL ? "Social" : "Military");

    int spy_index = rand() % group->group_size;

    for (int i = 0; i < group->group_size; i++) {
        group->members[i].member_id = i + 1;
        group->members[i].member_type = (i == spy_index) ? SPY : RESISTANCE_MEMBER;
        group->members[i].interaction_time = 0;
    }

    for (int i = 0; i < group->group_size; i++) {
        printf("Member %d: Type: %s, Interaction Time: %.2f seconds.\n", 
               group->members[i].member_id, 
               group->members[i].member_type == SPY ? "Spy" : "Resistance Member", 
               group->members[i].interaction_time);
    }



    while(1){

    }
}

  




void create_group() {
    printf("Start new group\n\n\n");

    pthread_mutex_lock(&groups_mutex);

    if (groups_created < MAX_GROUPS) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
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

            exit(0);  // Child process exits after creating the group
        } else {
            // Parent process
            group_pids[groups_created] = pid;  // Save the PID of the process
            groups_created++;  // Increment group count after creation

            // Unlock mutex after accessing shared array
            pthread_mutex_unlock(&groups_mutex);
        }
    } else {
        printf("Maximum groups reached, cannot create more.\n");
        pthread_mutex_unlock(&groups_mutex);
    }
}