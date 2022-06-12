#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int count;
int main ( int argc, char *argv[] )
{
    int i;

    if(argc != 2){
        printf("Not a suitable number of program arguments");
        exit(2);
    }else {
        count=0;
       for (i = 0; i < atoi(argv[1]); i++) {
           count+=1;
           int child =fork();
           if(child ==0)
           {
                printf("jestem dzieckiem numer %d, moje pid to:%d, moj rodzic to:%d\n",count,getpid(),getppid());
                exit(1);
           }
        //*********************************************************
        //Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1] procesow potomnych, bedacych dziecmi
        //   tego samego procesu macierzystego.
           // Kazdy proces potomny powinien:
               // - "powiedziec ktorym jest dzieckiem",
                //-  jaki ma pid,
                //- kto jest jego rodzicem
           //******************************************************
        }
    }
    return 0;
}
