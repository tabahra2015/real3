#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "agency.h"


pthread_mutex_t lock;

void *agency_member_function(void *arg) {
    AgencyMember *member = (AgencyMember *)arg;
    printf("Member %d: Active\n", member->id);
    sleep(rand() % 5 + 1); // Simulate activity
    if (rand() % 2) {
        member->status = DEAD;
        printf("Member %d: Died\n", member->id);
    } else {
        member->status = CAUGHT;
        printf("Member %d: Caught\n", member->id);
    }
    return NULL;
}

void add_new_member(int id) {
    pthread_mutex_lock(&lock);
    if (active_members < MAX_MEMBERS) {
        members[id].id = id;
        members[id].status = ALIVE;
        pthread_create(&members[id].thread, NULL, agency_member_function, &members[id]);
        active_members++;
        printf("New Member %d added. Active members: %d\n", id, active_members);
    }
    pthread_mutex_unlock(&lock);
}

void *monitor_function(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_MEMBERS; i++) {
            if (members[i].status == DEAD || members[i].status == CAUGHT) {
                pthread_join(members[i].thread, NULL);
                printf("Member %d removed.\n", members[i].id);
                members[i].status = ALIVE;
                add_new_member(i);
            }
        }
        pthread_mutex_unlock(&lock);
        sleep(1);
    }
    return NULL;
}
// first function call 
void agency_process() {
    srand(time(NULL));
    pthread_mutex_init(&lock, NULL);

    // Initialize agency members
    for (int i = 0; i < MAX_MEMBERS; i++) {
        add_new_member(i);
    }

    // Start monitoring thread
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_function, NULL);

    // Wait for monitor thread
    pthread_join(monitor_thread, NULL);

    pthread_mutex_destroy(&lock);
}