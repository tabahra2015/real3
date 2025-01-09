#ifndef AGENCY_H
#define AGENCY_H

#include <pthread.h>
#include "local.h"



void agency_process();
void *agency_member_function(void *arg);
int active_members;
#endif
