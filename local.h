#ifndef __LOCAL_H_
#define __LOCAL_H_

#include <sys/signal.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

// Declare these as const variables
extern int MAX_GROUPS;
extern int MIN_MEMBERS;
extern int MAX_MEMBERS;
extern int AGENCY_MEMBERS;
extern int GROUP_CREATION_INTERVAL;
extern int CIVILIAN_COUNT;
extern float SPY_TARGET_PROBABILITY;

typedef enum {
    SOCIAL,
    MILITARY
} GroupType;

typedef enum {
    EGENCY_MEMBER,
    SPY,
    CIVILIAN
} MemberType;

typedef enum {
    ALIVE,
    DEAD,
    CAUGHT
} MemberStatus;

typedef struct {
    int id;
    MemberStatus status;
    pthread_t thread;
} AgencyMember;

typedef struct {
    int member_id;
    MemberType member_type;
    pthread_t thread_id;
    float interaction_time;
} MemberInfo;

typedef struct {
    int group_id;
    int group_size;
    GroupType group_type;
  //  MemberInfo members[MAX_MEMBERS];
    float spy_target_probability;
} ResistanceGroup;

// Global variables (should be initialized in parent.c)
extern int groups_created;
extern AgencyMember members[100];
extern int active_members;

#endif // __LOCAL_H_
