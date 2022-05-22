#ifndef CONSTS
#define CONSTS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/time.h>

#define CAPACITY 5
#define ENTITY_SIZE sizeof(struct Entity)
#define PERMISSIONS 0666

#define COOKING_TIME 2

#define SEM_WORKING_OVEN 0
#define SEM_LEFT_SPACE_OVEN 1
#define SEM_WORKING_TABLE 2
#define SEM_LEFT_SPACE_TABLE 3





struct Entity
{
    int pizza[CAPACITY];
};


#endif