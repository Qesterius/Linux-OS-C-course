#include <sys/times.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>



//#ifdef _WIN32
#include <Windows.h>
//#else
#include <unistd.h>
//#endif

int main()
{
    struct tm * timeinfo;
    struct tms uno,duo;

    struct timeval realS, realE;
   
    gettimeofday(&realS,NULL);
    times(&uno);
    sleep(1);
    times(&duo);
    gettimeofday(&realE,NULL);


    

    printf("%Lf,%Lf\n", (double)uno.tms_utime, (double)uno.tms_stime);
    printf("%Lf,%Lf\n", (double)duo.tms_utime, (double)duo.tms_stime);
    printf("%Lf,%Lf\n",(double)(duo.tms_utime- uno.tms_utime), (double)(duo.tms_stime-uno.tms_stime));
    
    printf("divided by clk%Lf,%Lf\n",(double)((duo.tms_utime- uno.tms_utime)/CLK_TCK), (double)((duo.tms_stime-uno.tms_stime)/CLK_TCK));

    printf("%lu\n",(realE.tv_sec - realS.tv_sec) * 1000000 + realE.tv_usec - realS.tv_usec);
    printf("%lu %lu\n",realE.tv_sec, realS.tv_sec);


}
