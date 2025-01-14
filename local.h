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
#define LIGHT_INJURY 1
#define SEVERE_INJURY 2
#define MEMBER_CAUGHT 3
#define KILLED 4

#define AGENCY_MSG_KEY 12345
#define QUEUE_NAME "/monitor_queue"
#define SHM_NAME "/my_shared_memory"
#define MESSAGE_SIZE 256
#define SEED 1234
#define MAX_GROUPS_define 5
#define MAX_MEMBERS_define 80
#define TOTAL_MEMBERS_define 100
#define MAX_ENEMIES 6
// Declare these as const vagroups_createdriables
extern int MAX_GROUPS;
extern int MIN_MEMBERS;
extern int MAX_MEMBERS;
extern int AGENCY_MEMBERS;
extern int GROUP_CREATION_INTERVAL;
extern int CIVILIAN_COUNT;
extern float SPY_TARGET_PROBABILITY;
extern int TIME_EGENY_THRESHOLD;
extern int TOTAL_MEMBERS;
void setup_shared_memory();
#define SUSPICIOUS_TIME_THRESHOLD 10 // Example: Interaction > 10 seconds is suspicious


typedef struct {
    long message_type; // Message type (e.g., 1 for normal data)
    int group_id;      // ID of the resistance group
    int civilian_id;   // ID of the civilian
    int interaction_time; // Time spent with the group
} AgencyMessage;

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

typedef struct
{
    int id_res;
    int time_to_intercat;
    int group_num;
    int id_cit;
} SharedMessage;

typedef struct
{
    long message_type;
    int time_to_intercat;
    int id_cit; // Citizen ID
    int id_res;
    int id_group;
    pid_t pid_group;
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
    int busy; // 0 = not busy, 1 = busy
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
    int active;
     int injury_status; 
    pthread_mutex_t lock;
} MemberInfo;

typedef struct
{
    int group_id;
    int group_size;
    GroupType group_type;
    MemberInfo members[MAX_MEMBERS_define];
    float spy_target_probability;
} ResistanceGroup;

typedef struct
{
    long type;
    int member_id;
    MemberStatus status;
} MonitorMessage;

extern int groups_created;
extern AgencyMember members[100];

extern pid_t citizen_pids[TOTAL_MEMBERS_define];
extern int active_members;
extern int num_enemies;

extern ResistanceGroup *groups;
extern pid_t group_pids[MAX_GROUPS_define];
extern int groups_created;
extern Citizen citizens[TOTAL_MEMBERS_define];

extern pid_t enemy_pids[MAX_ENEMIES];
extern int pipes[MAX_ENEMIES][2];
extern int pipe_fd[2]; 
extern int pipe_fd2[2];

extern int pipesgroup[MAX_GROUPS_define][2];

extern Spy spy[TOTAL_MEMBERS_define];
extern pthread_mutex_t *groups_mutex;

#endif // __LOCAL_H_s