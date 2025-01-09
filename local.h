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
#define QUEUE_NAME "/monitor_queue"
#define MESSAGE_SIZE 256
#define SEED 1234
#define MAX_GROUPS_define 100
#define MAX_MEMBERS_define 100
#define TOTAL_MEMBERS_define 200

// Declare these as const variables
extern int MAX_GROUPS;
extern int MIN_MEMBERS;
extern int MAX_MEMBERS;
extern int AGENCY_MEMBERS;
extern int GROUP_CREATION_INTERVAL;
extern int CIVILIAN_COUNT;
extern float SPY_TARGET_PROBABILITY;
extern int TIME_EGENY_THRESHOLD;
extern int TOTAL_MEMBERS;

typedef enum
{
    SOCIAL,
    MILITARY
} GroupType;

typedef enum
{
    EGENCY_MEMBER,
    SPY,
    RESISTANCE_MEMBER,
    CIVILIAN
} MemberType;


typedef struct {
    long message_type;
    int  time_to_intercat;
    int id_cit;          // Citizen ID
    int id_res;
    int id_group;
    char text[MESSAGE_SIZE];
} MessageCitToRes; 

typedef enum
{
    ALIVE,
    DEAD,
    CAUGHT
} MemberStatus;

typedef struct
{
    int enemy_id;
    int spy_id;
    int group_number;
    float time_spent_in_group;
} Spy;

typedef struct
{
    int member_id;
    MemberType member_type;
    float interaction_time;
    int busy;  // 0 = not busy, 1 = busy
} Citizen;

typedef struct
{
    int id;
    time_t start_time;
    MemberStatus status;
    pthread_t thread;
} AgencyMember;

typedef struct
{
    int member_id;
    MemberType member_type;
    pthread_t thread_id;
    float interaction_time;
       int busy; 
} MemberInfo;

typedef struct
{
    int group_id;
    int group_size;
    GroupType group_type;
    MemberInfo members[MAX_MEMBERS_define];
    float spy_target_probability;
} ResistanceGroup;

// Message Structure
typedef struct
{
    long type;           // Message type (required for SysV IPC)
    int member_id;       // Member ID
    MemberStatus status; // Status of the member
} MonitorMessage;

// Global variables (should be initialized in parent.c)
extern int groups_created;
extern AgencyMember members[100];
extern pid_t enemy_pids[6];
extern pid_t citizen_pids[TOTAL_MEMBERS_define];
extern int active_members;
extern int num_enemies;

extern ResistanceGroup groups[MAX_GROUPS_define];
extern pid_t group_pids[MAX_GROUPS_define];
extern int groups_created;
extern Citizen citizens[TOTAL_MEMBERS_define];
#endif // __LOCAL_H_s