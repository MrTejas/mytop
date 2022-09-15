#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// defining colours
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef long long ll;
typedef long double ld;
const int refresh_rate = 1;

// check for file pointer to be null
// dont forget to close all the file pointers in the ends

void delay(int milli_seconds)
{
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds);
}


int* getUptime()
{
    FILE* ut;
    ut = fopen("/proc/uptime","r");
    if(ut==NULL)
    {
        return NULL;
        // file not opened
    }
    float up_time;
    fscanf(ut,"%f",&up_time);
    int* ret_val = (int*)malloc(3*sizeof(int));

    ret_val[0] = (int)(up_time/(60*60));
    ret_val[1] = (int)(up_time/60) - (60*ret_val[0]);
    ret_val[2] = (int)(up_time - (60*ret_val[1]) - (60*60*ret_val[0]));
    
    fclose(ut);
    return ret_val;
}


// main method
int main ()
{
    // hiding the console cursor
    printf("\33[?25l");

    int val = clock();
    printf("clock = %d\n",val);

    while(1)
    {
        // delay(100000);
        if(clock()%(refresh_rate*CLOCKS_PER_SEC)==0)
        {
            system("clear");
            printf("----------%sTOP PROGRAM----------\n\n",KCYN);

            // printing the uptime
            int* ar = getUptime();
            if(ar!=NULL)
            {   
                printf("UpTime :\t%d:",ar[0]);
                printf(ar[1]<10?"0":"");
                printf("%d:",ar[1]);
                printf(ar[2]<10?"0":"");
                printf("%d\n",ar[2]);
            }
        }
    }

    // re-enabling the console cursor
    printf("\33[?25h");
    return 0;
}
