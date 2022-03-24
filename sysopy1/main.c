#include "lib/biblioteka.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
//#ifdef _WIN32
#include <Windows.h>
//#else
#include <unistd.h>
//#endif


/*
void help()
{
    printf("COMMANDS:\nwc_files\nremove_block\nhelp\n");
}
void wc_files()
{
    printf("Podaj ilosc plikow:");
    int n;
    scanf("%d",&n);
    for(int i=0;i<n;i++)
    {
        char* path="";
        scanf("%s",path);
        printf("Scores allocated at index: %p\n",wcsystem(path));
    }
}
void remove_block()
{
    
    printf("Podaj adres:");
    int* wskaznik;
    scanf("%p",&wskaznik);
    free(wskaznik);
}*/

int isCommand(char *command)
{
    if( strcmp(command,"create_table")==0 
    || strcmp(command,"wc_files")==0 
    || strcmp(command,"remove_block")==0 
    || strcmp(command,"save")==0 )
    return 1;
    else return 0;
}

void get_time(struct tms* a, struct timeval* b)
{
    gettimeofday(b,NULL);
    times(a);
}



int main(int argc, char *argv[])
{
    FILE* wyniki =fopen(argv[1],"a");
    fprintf(wyniki,"-------------------------------START\n");
    //struct timespec start,prev, stop;
    //times(&start);
    struct tms total_S,total_E;
    struct timeval total_realS, total_realE;


    struct tms _S,_E;
    struct timeval realS, realE;




    //int command =0; //0 wc 1 save 2 remove
    int **table;
    int index=0;
    get_time(&total_S,&total_realS);
    for(int i = 2; i< argc; i++)
        {      
            get_time(&_S,&realS);
            if(strcmp(argv[i],"create_table")==0)
            {
                
                table = (int**)calloc(atoi(argv[i+1]),1);
                get_time(&_E,&realE);
                fprintf(wyniki,"%lu %lf %lf create_table %s\n",
                 (realE.tv_sec - realS.tv_sec) * 1000000 + realE.tv_usec - realS.tv_usec,
                 (double)(_E.tms_utime- _S.tms_utime), 
                 (double)(_E.tms_stime-_S.tms_stime),
                 argv[i+1]);
                
                i++;
            }
            else if(strcmp(argv[i],"wc_files")==0)
            {
                char arguments[3000]="";
                int num=0;
                for(i=i+1;i<argc;i++)
                {
                    if( isCommand(argv[i]) )
                        {i-=1;
                        break;}
                    else
                    {
                        strcat(arguments,argv[i]);
                        strcat(arguments," ");
                        num++;
                    }
                }
                wcsystem(arguments,num);
                get_time(&_E,&realE);
                fprintf(wyniki,"%lu %lf %lf wc_files %s %d\n",
                 (realE.tv_sec - realS.tv_sec) * 1000000 + realE.tv_usec - realS.tv_usec,
                 (double)(_E.tms_utime- _S.tms_utime),
                 (double)(_E.tms_stime-_S.tms_stime),
                 arguments,
                 num);
                free(arguments);
            }
            else if(strcmp(argv[i],"remove_block")==0)
            {
                int __index= atoi(argv[i+1]);
                i++;
                free(table[__index]);
                get_time(&_E,&realE);
                fprintf(wyniki,"%lu %lf %lf remove_block %d\n",
                 (realE.tv_sec - realS.tv_sec) * 1000000 + realE.tv_usec - realS.tv_usec,
                 (double)(_E.tms_utime- _S.tms_utime), 
                 (double)(_E.tms_stime-_S.tms_stime),
                 index);
                
            }
            else if(strcmp(argv[i],"save")==0)
            {
                index= save(table,atoi(argv[i+1]),index);
                 get_time(&_E,&realE);
                fprintf(wyniki,"%lu %lf %lf save %d\n", 
                (realE.tv_sec - realS.tv_sec) * 1000000 + realE.tv_usec - realS.tv_usec,
                (double)(_E.tms_utime- _S.tms_utime), 
                (double)(_E.tms_stime-_S.tms_stime),
                atoi(argv[i+1]));
                
            }
        }
    get_time(&total_E,&total_realE);
    fprintf(wyniki,"-------------------------------------TOTALTIME:%lu %lf %lf\n", 
                (total_realE.tv_sec - total_realS.tv_sec) * 1000000 + total_realE.tv_usec - total_realS.tv_usec,
                (double)(total_E.tms_utime- total_S.tms_utime), 
                (double)(total_E.tms_stime- total_S.tms_stime));
    fclose(wyniki);
    return 0;
}
