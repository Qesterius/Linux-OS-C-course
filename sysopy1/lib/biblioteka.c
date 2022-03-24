#include "biblioteka.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#include <sys/times.h>

int gettimeofday(struct timeval* t,void* timezone)
{       struct _timeb timebuffer;
        _ftime( &timebuffer );
        t->tv_sec=timebuffer.time;
        t->tv_usec=1000*timebuffer.millitm;
		return 0;
}

clock_t times (struct tms *__buffer) {

	__buffer->tms_utime = clock();
	__buffer->tms_stime = 0;
	__buffer->tms_cstime = 0;
	__buffer->tms_cutime = 0;
	return __buffer->tms_utime;
}

FILE* wc( char path[] )
{
    FILE *f = fopen(path,"r");
    if( !f )
    {
        fprintf(stderr, "ERROR OPENING THE FILE: %s\n", path);
        return NULL;
    }


    FILE* tmp = tmpfile();

    int started_word = 0;
    int words=0;
    int characters=0;
    int lines=0;
    int character = fgetc(f);
    while(character != EOF )
    {
        
        if(character == ' ')
        {
            if(started_word ==1)
            {
                words = words+1;
                started_word=0;
            }
        }
        else if( character == '\n')
        {
            if(started_word ==1)
            {
                words = words+1;
                started_word=0;
            }
            lines = lines+1;
        }
        else
        {
            characters +=1;
            started_word = 1;
        }
        character = fgetc(f);
    }

    fprintf(tmp,"%d %d %d\n",characters,words,lines);
    printf("%d %d %d\n",characters,words,lines);
    fclose(f);
    return tmp;
}
void wcsystem(char* path, int num)
{
   FILE* tmp = fopen("out.txt","w");
   fclose(tmp);

   //printf("arg:%s\n", path);


   char command[3000] = "";
   strcat(command,"wc ");
   strcat(command,path);
   strcat(command," >> out.txt");
   printf("comm:%s\n", command);
   system(command);
   free(command);
    

   return;
}
extern int save(int** table,int n, int index)
{

    FILE* tmp = fopen("out.txt","r");
    if(!tmp) printf("cant open file!!\n");
    else printf("opened file\n");
    rewind(tmp);
    char dupa[100]="";

    //printf("\nbfr:%d %d %d\n",a,b,c);
    for(int i=0; i<n;i++)
    {
        int a=0;
        int b=0;
        int c=0;
        fscanf(tmp, "%d",&a);
        fscanf(tmp, "%d",&b);
        fscanf(tmp, "%d",&c);
        fscanf(tmp, "%s",dupa);
        printf("Ignored:%s\n",dupa);
        table[index+i]= allocate3Ints(&a,&b,&c);
        printf("saved in memory at%p:%d %d %d\n",table[index+i],table[index+i][0],table[index+i][1],table[index+i][2]);
    }
    free(dupa);
    fclose(tmp);
    return index+n;
}





void cat_path(char path[])
{
    FILE* file  = fopen(path,"r");
    if( !file )
    {
        fprintf(stderr, "ERROR OPENING THE FILE: %s\n", path);
        return ;
    }
    char line[256];
    while(fgets(line,sizeof(line),file))
    {
        printf("%s",line);
    }

    fclose(file);
}

void cat_file(FILE* file)
{
    rewind(file);
    if( !file )
    {
        printf("ERROR: FILE DOES NOT EXIST\n");
        fprintf(stderr, "ERROR: FILE DOES NOT EXIST\n");
        return ;
    }
    char line[256];
    while(fgets(line,sizeof(line),file))
    {
        printf("%s",line);
    }
}

int* allocate3Ints(int* a, int* b, int* c)
{
  int *score = (int*)calloc(3,sizeof(int));
  score[0] = *a;
  score[1] = *b;
  score[2] = *c;

  free(a);
  free(b);
  free(c);
  return score;
}