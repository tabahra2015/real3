#ifndef AGENCY_H
#define AGENCY_H

#include <pthread.h>
#include "local.h"
#define MAX_MESSAGES 1000



void agency_process();
void *agency_member_function(void *arg);
void generate_chart(int group_num, int citizen_time[], int employee_time[], int spy_count, int total_interactions);
void analyze_group_interactions(int group_num) ;
void *analyze_group_interactions_thread() ;
int active_members;
#endif
