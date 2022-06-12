#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int* trashELFES;
int* trashREINDEERS;
pthread_t* elfThrd;
pthread_t* reindThrd;
pthread_t mikolajThrd;


int queueElf[3];
int solvedProblem=0;

int free_q_id=0;
pthread_mutex_t elf_fullq_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_q_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sleeping_santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t renifer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;





pthread_cond_t elf_fullq_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_q_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t sleeping_santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t renifer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t exit_cond = PTHREAD_COND_INITIALIZER;




int getRandomNumber(int max)
{
    struct timeval currTime;
    gettimeofday(&currTime, 0);
    return (currTime.tv_sec*getpid())%max;
}

int waitingRenifers=0;
int traveling=0; //1 travel; 2 dostarczono
void renifer(void* arg){
    int ID = *((int*)(arg));
    printf("BEEE BEEE: %d\n",ID);

    while(1)
    {    
        /*Są na wakacjach w ciepłych krajach losowy okres czasu (5-10s)
        Wracaja na biegun północny (Komunikat: Renifer: czeka _ reniferów na Mikołaja, ID), jeśli jest dziewiątym reniferem to wybudza Mikołaja (Komunikat: Renifer: wybudzam Mikołaja, ID).
        Dostarczają zabawki grzecznym dzieciom (i studentom którzy nie spóźniają się z dostarczaniem zestawów) przez (2-4s).   
        Lecą na wakacje.*/
        int wait_t = 5+getRandomNumber(5);
        sleep(wait_t);
        pthread_mutex_lock(&renifer_mutex);
        traveling=0;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n",waitingRenifers,ID);
        waitingRenifers++;
        while(traveling<2)
        {
            if(traveling==1)
            {
                puts("MEEEEEE, GOING WITH SANTAS");
            }
            if(waitingRenifers==9)
            {
                pthread_cond_broadcast(&sleeping_santa_cond);
                pthread_cond_broadcast(&renifer_cond);
            }            
            pthread_cond_wait(&renifer_cond,&renifer_mutex);    
        }
        pthread_mutex_unlock(&renifer_mutex);





    }
}

void elf(void* arg)
{
    int ID = *((int*)(arg));
    printf("Powstalem z popiolow %d\n",ID);
    while(1)
    {
        /*
        Pracują przez losowy okres czasu (2-5s).
        Chcą zgłosić problem - jeśli liczba oczekujących elfów przed warsztatem Mikołaja jest mniejsza od 3 to idzie przed warsztat 
            (Komunikat: Elf: czeka _ elfów na Mikołaja, ID), 
        w przeciwnym wypadku czeka na powrót pierwszej trójki i dopiero wtedy idzie przed warsztat 
            (Komunikat: Elf: czeka na powrót elfów, ID)
        Jeśli jest trzecim elfem przed warsztatem to wybudza Mikołaja. (Komunikat: Elf: wybudzam Mikołaja, ID)
        Mikołaj się z nimi spotyka. (Komunikat: Elf: Mikołaj rozwiązuje problem, ID) (1-2s)
        Wraca do pracy
        */
        int wait_t = 2+getRandomNumber(3);
        sleep(wait_t);

        pthread_mutex_lock(&elf_fullq_mutex);
        while(free_q_id>=3)
        {
            printf("Elf: czeka na powrot elfow,%d\n",ID);
            pthread_cond_broadcast(&sleeping_santa_cond);

            pthread_cond_wait(&elf_fullq_cond,&elf_fullq_mutex);
        }
        solvedProblem =0;
        queueElf[free_q_id] = ID;
        free_q_id+=1;
        printf("Elf: czeka %d elfow na mikolaja,%d\n",free_q_id,ID);
        pthread_mutex_unlock(&elf_fullq_mutex);
        
        pthread_mutex_lock(&elf_q_mutex);
        if(free_q_id==3)
        {
            printf("Elf: Wybudzam mikolaja,%d\n",ID);            
            pthread_cond_broadcast(&sleeping_santa_cond);
        }
        while(solvedProblem<2)
        {
            pthread_cond_wait(&elf_q_cond,&elf_q_mutex);
            if(solvedProblem==1)
                printf("Elf: Mikolaj rozwaizuje problem,%d\n",ID);
        }
        pthread_mutex_unlock(&elf_q_mutex);
    }
}
int prezenty;
void mikolaj()
{
    prezenty=0;
    while(prezenty<3)
    {
        pthread_mutex_lock(&sleeping_santa_mutex);
        while(free_q_id<3 && waitingRenifers<9)
        {
            puts("Mikolaj: Zasypiam");
            pthread_cond_wait(&sleeping_santa_cond,&sleeping_santa_mutex);
        }
        puts("Mikolaj: Budze sie");
        if(free_q_id==3 )
        {
            solvedProblem = 1;
            pthread_cond_broadcast(&elf_q_cond);
            puts("Mikolaj starting to solve elves");
            int wait_t=1+getRandomNumber(1);
            sleep(wait_t);     
            puts("solvd");
            solvedProblem = 2;
            free_q_id=0;
            pthread_cond_broadcast(&elf_q_cond);
            pthread_cond_broadcast(&elf_fullq_cond);
        }
        else if(waitingRenifers==9){
            waitingRenifers=0;
            traveling=1;
            pthread_cond_broadcast(&renifer_cond);
            printf("Mikolaj: Dostarczam zabawki\n");
            int wait_t=2+getRandomNumber(2);
            sleep(wait_t);     
            traveling=2;
            pthread_cond_broadcast(&renifer_cond);
            prezenty++;
            printf("MIKOLAJ: I DONE %d PRESENTSAS\n",prezenty);
        }
        pthread_mutex_unlock(&sleeping_santa_mutex);
    }
    pthread_cond_broadcast(&exit_cond);
    puts("ME DONE WITH YOU ALL ");
    exit(0);
}

void createThreads(int num, pthread_t* arr,int mode, int* indexes)
{   indexes = malloc(num*sizeof(pthread_t));
    arr = malloc(num*sizeof(pthread_t));
    
    if(mode==1)//elf
    {
        for(int i=0;i<num;i++)
        {
            indexes[i]=i;
            pthread_create(&arr[i],0,&elf,&indexes[i]);
        }
    }
    else if(mode==2)//renifer
    {
        for(int i=0;i<num;i++)
        {
            indexes[i]=i;
            pthread_create(&arr[i],0,&renifer,&indexes[i]);
        }
    }
    else if( mode==3)
            pthread_create(&mikolajThrd,0,&mikolaj,0);

    puts("Succesfully created new threads");

}

int main(int argc, char* argv[])
{
    void stworzSemafory();
    createThreads(9,    reindThrd,  2,  trashREINDEERS);
    createThreads(10,   elfThrd,    1,  trashELFES);
    createThreads(0,0,3,0);


    pthread_mutex_lock(&exit_mutex);
    while(prezenty<3)
    {
        pthread_cond_wait(&exit_cond,&exit_mutex);
    }
    pthread_mutex_unlock(&exit_mutex);

    
    exit(0);
}