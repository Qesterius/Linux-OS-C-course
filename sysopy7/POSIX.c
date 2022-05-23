#include "constants.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/errno.h>
#ifndef NULL
#define NULL 0
#endif
int nChefs;
int nDeliverers;
sem_t** semafors;

int semafors_ID, ovenMemID, tableMemID;
struct Entity* oven;
struct Entity* table;

int getSemVal();
union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void sigintHandler(int signum)
{
    for(int i=0;i<CAPACITY;i++)
    {
        sem_close(semafors[i]);
    }
    sem_unlink("w_oven");
    sem_unlink("w_table");
    sem_unlink("taken_table");
    sem_unlink("delivered_pizzas");
    sem_unlink("left_oven");
    sem_unlink("left_table");

    munmap(oven,sizeof(struct Entity));
    munmap(table,sizeof(struct Entity));
    shm_unlink("/oven");
    shm_unlink("/table");

}
int createShm(char* name)
{
    int shm = shm_open(name, O_CREAT|O_RDWR,PERMISSIONS);
    if(shm==-1)
        {
            puts("cant create shm");
            exit(-1);
        }
    puts("shm_open ... correct");
    if(ftruncate(shm,sizeof(struct Entity))==-1)
        {puts("CANT TRUNCATE POSIX SHM");exit(-1);}
    
    struct Entity* entity =(struct Entity*) mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,shm,0);
    puts("created entity");
//c,d,e
    printf(" Value of errno: %d\n ", errno);
    printf("%d, %d\n",entity,shm);
    fflush(stdout);
    for(int i=0;i<CAPACITY;i++)
        entity->pizza[i] = -1;
    entity->toPut = 0;
    entity->toTake = 0;
    entity->pizzasInside=0;

    
    puts("initialized");
    fflush(stdout);
    return shm;
}
int createSemasphores()
{
    semafors = malloc(sizeof(sem_t*)*6);
    semafors[SEM_WORKING_OVEN] = sem_open("w_oven",O_CREAT,PERMISSIONS,1);
    semafors[SEM_WORKING_TABLE] = sem_open("w_table",O_CREAT,PERMISSIONS,1);

    semafors[SEM_TAKEN_SPACE_TABLE] = sem_open("taken_table",O_CREAT,PERMISSIONS,0);
    semafors[SEM_DELIVERED_PIZZAS] = sem_open("delivered_pizzas",O_CREAT,PERMISSIONS,0);

    semafors[SEM_LEFT_SPACE_OVEN] = sem_open("left_oven",O_CREAT,PERMISSIONS,CAPACITY);
    semafors[SEM_LEFT_SPACE_TABLE] = sem_open("left_table",O_CREAT,PERMISSIONS,CAPACITY);

    for(int i=0;i<CAPACITY;i++)
        printf("value of %d: %d\n",i,getSemVal(i));
    printf("Created semaphores\n");
    fflush(stdout);
}
void closeSEMLOCK(int lock)
{
            sem_wait(semafors[lock]);
            return;
}
void openSEMLOCK(int lock)
{
            sem_post(semafors[lock]);
            return;
}
int getRandomNumber(int max)
{
    struct timeval currTime;
    gettimeofday(&currTime, NULL);
    return (currTime.tv_sec*getpid())%max;
}
int getSemVal(int lock)
{
    int val =0;
    sem_getvalue(semafors[lock],&val);
    return val;
}
char* getCurrTime()
{
    struct timeval currTime;
    gettimeofday(&currTime, NULL);

    const int BUFFER_SIZE = 64;
    char buffer[BUFFER_SIZE];
    char* bufferPtr = &buffer[0];
    strftime(bufferPtr, BUFFER_SIZE, "%H:%M:%S", localtime(&currTime.tv_sec));

    char* currTimeStr = calloc(BUFFER_SIZE, sizeof(char));
    int msec = currTime.tv_usec / 1000;
    sprintf(currTimeStr, "%s:%03d", bufferPtr, msec);

    return currTimeStr;
}

int main(int argc, char* argv[])
{
    if(argc!=3)
        exit(-1);

    nChefs = atoi(argv[1]);
    nDeliverers = atoi(argv[2]);

    
    printf("WITAMY W LOS PAPITOS\nDzisiaj obsluga liczy %d kucharzy i %d dostawcow.\n",nChefs,nDeliverers);   
    ovenMemID = createShm("/oven");
    tableMemID = createShm("/table");
    oven  = mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,ovenMemID,0);
shmat(ovenMemID,NULL,0);
    table = mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,tableMemID,0);

    printf("%d %d\n",oven,table);
    for(int i=0;i<CAPACITY;i++)
        printf("value of %d: %d\n",i,oven->pizza[i]);
    createSemasphores();

    signal(SIGINT, sigintHandler);
    
    if(fork()==0)
    {
        FILE* raport = fopen("wyniki.txt","a");
        sleep(CONTEST_TIME);
        fprintf(raport,"%d %d in %d seconds %d delivered %d pizzas. /POSIX\n",nChefs,nDeliverers,CONTEST_TIME,getSemVal(SEM_DELIVERED_PIZZAS));
        fclose(raport);
        printf("\n\n\n CONTEST FINISHED\n\n\n");
        exit(0);
    }

    int i;
    for (i = 0; i < nChefs; ++i)
        if (fork() == 0)
            { 
                printf("IM A CHEF %d\n",getpid());
                oven  = mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,ovenMemID,0);
shmat(ovenMemID,NULL,0);
    table = mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,tableMemID,0);


                while(1){

                    int myPizza = getRandomNumber(10); 
                    printf("%d [%s] przygotowuje pszna pitce:%d\n",getpid(),getCurrTime(),myPizza);   
                    sleep(PREPARING_TIME);

                    closeSEMLOCK(SEM_LEFT_SPACE_OVEN);

                        closeSEMLOCK(SEM_WORKING_OVEN);
                            printf("%d [%s] pitca wsadzana do pieca. Liczba picc w piecu: %d.\n",getpid(),getCurrTime(),CAPACITY-getSemVal(SEM_LEFT_SPACE_OVEN));
                            int freeOven= oven->toPut;
                            oven->toPut = (oven->toPut+1)%CAPACITY;
            
                                if(oven->pizza[freeOven]!=-1)
                                {
                                    printf("THIS PIZZA PLACE SHOULD BE EMPTY\n\n\n SUICIDE OF CHEF:%d\n",getpid());
                                    exit(-1);
                                } 
                            
                            oven->pizza[freeOven]= myPizza;
                        
                        openSEMLOCK(SEM_WORKING_OVEN);
                        sleep(COOKING_TIME);
                        //taking out    
                        closeSEMLOCK(SEM_WORKING_OVEN);
                            oven->pizza[freeOven]=-1;
                        openSEMLOCK(SEM_WORKING_OVEN);
                    openSEMLOCK(SEM_LEFT_SPACE_OVEN);
                    printf("%d [%s] pitce wyjelem z pieca. Liczba picc w piecu: %d.\n",getpid(),getCurrTime(),CAPACITY-getSemVal(SEM_LEFT_SPACE_OVEN));

                    


                    closeSEMLOCK(SEM_LEFT_SPACE_TABLE);
                        closeSEMLOCK(SEM_WORKING_TABLE);
                        int freeTable=table->toPut;
                        table->toPut = (table->toPut+1)%CAPACITY;

                            if(table->pizza[freeTable]!=-1)
                            {
                                    printf("THIS PIZZA TABLE SHOULD BE EMPTY\n\n\n SUICIDE OF CHEF:%d\n",getpid());
                                    exit(-1);
                            }

                        table->pizza[freeTable] = myPizza;
                        openSEMLOCK(SEM_WORKING_TABLE);
                    openSEMLOCK(SEM_TAKEN_SPACE_TABLE);            
                    printf("%d [%s] pitce polozylem na stole. Liczba picc na stole: %d.\n",getpid(),getCurrTime(),getSemVal(SEM_TAKEN_SPACE_TABLE));
                }    
                    
                exit(0);
                
            }
            //execl("./output_chef", "./output_chef", NULL);

    for (i = 0; i < nDeliverers; ++i)
        if (fork() == 0)
            { 
                oven  = mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,ovenMemID,0);
shmat(ovenMemID,NULL,0);
    table = mmap(NULL,sizeof(struct Entity),PROT_READ|PROT_WRITE,MAP_SHARED,tableMemID,0);

                printf("IM A DELIVERER %d\n",getpid()); 

                while(1)
                {
                    closeSEMLOCK(SEM_TAKEN_SPACE_TABLE);
                        closeSEMLOCK(SEM_WORKING_TABLE);
                            int takingID = table->toTake;
                            table->toTake = (table->toTake+1)%CAPACITY;
                            if(table->pizza[takingID]== -1)
                            {
                                printf("%d [%s] GDZIE JEST PICCA, POWINNA TU BYC...\n       SAIONARA CHEF :(( \n",getpid(),getCurrTime());
                                exit(-1);
                            }
                            int deliveryPizza = table->pizza[takingID];
                            table->pizza[takingID] =-1;
                            printf("%d [%s] Wzielem pitce %d ze stolu. Liczba picc na stole: %d.\n",getpid(),getCurrTime(),deliveryPizza,getSemVal(SEM_TAKEN_SPACE_TABLE));
                        openSEMLOCK(SEM_WORKING_TABLE);
                    openSEMLOCK(SEM_LEFT_SPACE_TABLE);
                    sleep(DELIVERY_TIME);
                    printf("%d [%s] Dostarczylem pitce %d.\n",getpid(),getCurrTime(),deliveryPizza);
                    openSEMLOCK(SEM_DELIVERED_PIZZAS);
                    sleep(COMEBACK_TIME);
                    printf("%d [%s] Wrocilem.\n",getpid(),getCurrTime());

                }      
                exit(0);
                
            }

            //execl("./output_carrier", "./output_carrier", NULL);
    while(wait(NULL)>0)
        ;
    sigintHandler(SIGINT);
    return 0;
}