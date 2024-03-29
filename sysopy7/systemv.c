#include "constants.h"

int nChefs;
int nDeliverers;
int semafors_ID, ovenMemID, tableMemID;
struct Entity* oven;
struct Entity* table;


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
    semctl(semafors_ID, 0, IPC_RMID, NULL);
    shmctl(ovenMemID, IPC_RMID, NULL);
    shmctl(tableMemID, IPC_RMID, NULL);
}
int createShm(int projID_KEY)
{
    key_t key = ftok(getenv("HOME"), projID_KEY);
    int shm = shmget(key, ENTITY_SIZE, IPC_CREAT | PERMISSIONS);
    struct Entity* entity = shmat(shm,NULL,0);

    for(int i=0;i<CAPACITY;i++)
        entity->pizza[i] = -1;
    entity->toPut = 0;
    entity->toTake = 0;
    entity->pizzasInside=0;

    return shm;
}
int createSemasphores()
{
    key_t key = ftok(getenv("HOME"), 'S');
    semafors_ID = semget(key, 6 ,IPC_CREAT | PERMISSIONS);

    union semun arg;
    arg.val = 0;
    semctl(semafors_ID,SEM_TAKEN_SPACE_TABLE,SETVAL,arg);
    semctl(semafors_ID,SEM_DELIVERED_PIZZAS,SETVAL,arg);


    arg.val = 1;
    semctl(semafors_ID,SEM_WORKING_OVEN,SETVAL,arg);
    semctl(semafors_ID,SEM_WORKING_TABLE,SETVAL,arg);

    arg.val = CAPACITY;
    semctl(semafors_ID,SEM_LEFT_SPACE_OVEN,SETVAL,arg);
    semctl(semafors_ID,SEM_LEFT_SPACE_TABLE,SETVAL,arg);

    printf("Created semaphores: %d\n", semafors_ID);
}
void closeSEMLOCK(int lock)
{
            struct sembuf buff;
            buff.sem_num = lock;
            buff.sem_op = -1;

            semop(semafors_ID,&buff,1);
            return;
}
void openSEMLOCK(int lock)
{
            struct sembuf buff;
            buff.sem_num = lock;
            buff.sem_op = 1;

            semop(semafors_ID,&buff,1);
            return;
}
int getRandomNumber(int max)
{
    struct timeval currTime;
    gettimeofday(&currTime, NULL);
    return (currTime.tv_sec*getpid())%max;
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
    ovenMemID = createShm('O');
    tableMemID = createShm('T');
    oven  = shmat(ovenMemID,NULL,0);
    table = shmat(tableMemID,NULL,0);

    createSemasphores();

    signal(SIGINT, sigintHandler);
    
    if(fork()==0)
    {
        FILE* raport = fopen("wyniki.txt","a");
        sleep(CONTEST_TIME);
        fprintf(raport,"%d %d in %d seconds delivered %d pizzas. /SYSTEMV\n",nChefs,nDeliverers,CONTEST_TIME,semctl(semafors_ID,SEM_DELIVERED_PIZZAS,GETVAL));
        fclose(raport);
        printf("\n\n\n CONTEST FINISHED\n\n\n");
        exit(0);
    }

    int i;
    for (i = 0; i < nChefs; ++i)
        if (fork() == 0)
            { 
                printf("IM A CHEF %d\n",getpid());

                while(1){

                    int myPizza = getRandomNumber(10); 
                    printf("%d [%s] przygotowuje pszna pitce:%d\n",getpid(),getCurrTime(),myPizza);   
                    sleep(PREPARING_TIME);

                    closeSEMLOCK(SEM_LEFT_SPACE_OVEN);

                        closeSEMLOCK(SEM_WORKING_OVEN);
                            printf("%d [%s] pitca wsadzana do pieca. Liczba picc w piecu: %d.\n",getpid(),getCurrTime(),CAPACITY-semctl(semafors_ID,SEM_LEFT_SPACE_OVEN,GETVAL));
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
                    printf("%d [%s] pitce wyjelem z pieca. Liczba picc w piecu: %d.\n",getpid(),getCurrTime(),CAPACITY-semctl(semafors_ID,SEM_LEFT_SPACE_OVEN,GETVAL));

                    


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
                    printf("%d [%s] pitce polozylem na stole. Liczba picc na stole: %d.\n",getpid(),getCurrTime(),semctl(semafors_ID,SEM_TAKEN_SPACE_TABLE,GETVAL));
                }    
                    
                exit(0);
                
            }
            //execl("./output_chef", "./output_chef", NULL);

    for (i = 0; i < nDeliverers; ++i)
        if (fork() == 0)
            { 
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
                            printf("%d [%s] Wzielem pitce %d ze stolu. Liczba picc na stole: %d.\n",getpid(),getCurrTime(),deliveryPizza,semctl(semafors_ID,SEM_TAKEN_SPACE_TABLE,GETVAL));
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