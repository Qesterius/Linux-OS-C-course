#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#define BUFF_LEN 100

#ifndef NULL 
#define NULL 0
#endif

int **imagegrid;
int imW;
int imH;
int M;
int n_threads;
pthread_t *threadIds;
FILE* time_F;

char* getTimeElapsed( struct timeval last)
{
    struct timeval currTime;
    gettimeofday(&currTime, NULL);

    int elapsedSeconds = currTime.tv_sec -last.tv_sec;
    float elapsedMicrosec = ((float)(currTime.tv_usec - last.tv_usec))/1000000;

    char* timeStr = malloc(sizeof(char)*100);
    sprintf(timeStr, "%d + %f",elapsedSeconds , elapsedMicrosec);

    return timeStr;
}

void loadImage(char* path)
{
    FILE* f = fopen(path, "r");
    char* buff = malloc(BUFF_LEN*sizeof(char));
    fgets(buff,BUFF_LEN-1,f); //magic
    if(strcmp(buff,"P2\n")!=0)
        {
            puts("wrong file type");
            exit(-1);
        }
    fgets(buff,BUFF_LEN-1,f); //W H
    sscanf(buff,"%d %d",&imW,&imH);

    fgets(buff,BUFF_LEN-1,f); //M
    sscanf(buff,"%d",&M);

    imagegrid = malloc(imH*sizeof(int));//allocate im and write
    int i=0;
    for(i=0;i<imH;i++)
    {
        imagegrid[i] = malloc(imW*sizeof(int));
        int j=0;
        int pixel=0;
        for(j=0;j<imW;j++)
        {
                fscanf(f,"%d",&pixel); //wczytywanie
                imagegrid[i][j]=pixel; 
                //printf("%d %d\n",i,j);

        }            
    }
    for(i=0;i<2;i++)
    {
        int j;
        for(j=0;j<imW;j++)
            printf("%d",imagegrid[i][j]);
        puts("");
    }
}
void methodNumbers(void* arg)
{
    struct timeval threadStart;
    gettimeofday(&threadStart, NULL);

    int order = *(int*)arg;
    int min = (M+1)*order/n_threads;
    int max = (M+1)*(order+1)/n_threads;

    printf("%d %d %d\n",order,min,max);
    int i,j=0;
    for (i=0;i<imH;i++)
        for(j=0;j<imW;j++)
        {
            if(imagegrid[i][j]>=min && imagegrid[i][j]<max)
            {
                imagegrid[i][j] = 255-imagegrid[i][j];
            }
        }
    pthread_exit(getTimeElapsed(threadStart));
}

void methodBlock(void* arg)
{
    struct timeval threadStart;
    gettimeofday(&threadStart, NULL);
    int order = *(int*)arg;
    
    int j_min = order*(imW/n_threads);
    int j_max = (order+1)*(imW/n_threads);
    printf("%d",imagegrid[301][0]);
    fflush(stdout);

   // sleep(1000);
    printf("%d %d %d\n",order,j_min,j_max);
    int i,j=0;
    for (i=0;i<imH;i++)
        {for(j=j_min;j<j_max;j++)
        {
                printf("   col %d %d\n",j,imagegrid[i][j]);
                imagegrid[i][j] = 255-imagegrid[i][j];
        }
            printf("%d: done row %d\n",order,i);
        }

    pthread_exit(getTimeElapsed(threadStart));

}
int* threadCounter;
void createThreads(int mode)
{
    threadIds = malloc(n_threads*sizeof(pthread_t));
    threadCounter = malloc(n_threads*sizeof(int));
    if(mode==1)
    {
        for(int i=0;i<n_threads;i++)
        {
            threadCounter[i] =i;
            pthread_create(&threadIds[i],NULL,&methodNumbers,&threadCounter[i]);
        }
    }
    else if(mode==2)
    {
        for(int i=0;i<n_threads;i++)
        {
            threadCounter[i] =i;
            pthread_create(&threadIds[i],NULL,&methodBlock,&threadCounter[i]);
        }
    }
    puts("Succesfully created new threads");

}
void waittForThreads(struct timeval START)
{
    for(int i=0;i<n_threads;i++)
    {
        printf("Waiting for :%d\n",i);
        char* time;
        pthread_join(threadIds[i],&time);
        fprintf(time_F,"    thr %d:%s\n",i,time);
    }
    fprintf(time_F,"finished %s\n",getTimeElapsed(START));
}
int main( int argc, char* argv[])
{
    if( argc != 5 )
        {
            puts("args count provided is not sufficient");
            exit(-1);
        }
    n_threads = atoi(argv[1]);
    int mode = atoi(argv[2]); // 1 - numbers / 2 - block
    char* in_Image_path = argv[3];
    char* out_Image_path = argv[4];
    printf("n_threads:%d mode:%d in:%s out %s\n",n_threads,mode,in_Image_path,out_Image_path);    
    
    loadImage(in_Image_path);
    puts("loadedimg");
    time_F = fopen("Times.txt","a");
    fprintf(time_F,"Started in \nmode:%d\nnthreads:%d\nin_f:%s\nout_f:%s\n",  mode,n_threads,in_Image_path,out_Image_path);
    struct timeval startTime;
    gettimeofday(&startTime, NULL);

    createThreads(mode);
    waittForThreads(startTime);
    
    fclose(time_F);
    //savefile
    FILE* out = fopen(out_Image_path,"w");
    fprintf(out,"P2\n%d %d\n%d\n",imW,imH,M);

    int counter =0;
    for(int i=0;i<imH;i++)
    {
        int j=0;
        for(j=0;j<imW;j++)
        {
                fprintf(out,"%d ",imagegrid[i][j]);
                counter+=1;
                if(counter>19)
                {
                    counter=0;
                    fprintf(out,"\n");

                }
        }            
    }
}