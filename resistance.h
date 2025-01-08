#ifndef RESISTANCE_H
#define RESISTANCE_H

#include <pthread.h>

typedef enum {
    SOCIAL,
    MILITARY
} GroupType;

typedef enum {
    NORMAL_MEMBER,
    SPY,
    CIVILIAN
} MemberType;

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
    MemberInfo members[100];
    float spy_target_probability;
} ResistanceGroup;

void group_process(ResistanceGroup *group);
void create_group();
//void group_member_function();

#endif
