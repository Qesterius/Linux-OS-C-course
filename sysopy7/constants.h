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

#define PREPARING_TIME 2
#define COOKING_TIME 5
#define DELIVERY_TIME 5
#define COMEBACK_TIME 5


#define SEM_WORKING_OVEN 0
#define SEM_LEFT_SPACE_OVEN 1
#define SEM_WORKING_TABLE 2
#define SEM_LEFT_SPACE_TABLE 3
#define SEM_TAKEN_SPACE_TABLE 4
#define SEM_DELIVERED_PIZZAS 5

#define CONTEST_TIME 60




struct Entity
{
    int pizza[CAPACITY];
    int toPut;
    int toTake;
    int pizzasInside;
};


#endif