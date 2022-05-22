#include "constants.h"

int nChefs;
int nDeliverers;
int semafors_ID, ovenMem, tableMem;

void sigintHandler(int signum)
{
    semctl(semafors_ID, 0, IPC_RMID, NULL);
    shmctl(ovenMem, IPC_RMID, NULL);
    shmctl(tableMem, IPC_RMID, NULL);
}

int main(int argc, char* argv[])
{
    if(argc!=3)
        exit(-1);
    nChefs = atoi(argv[1]);
    nDeliverers = atoi(argv[2]);

    key_t key = ftok(getenv("HOME"), 0);
    printf("%d",key);



}