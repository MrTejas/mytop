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

// bytes to gigs convertor const
#define BTOG 1048576

typedef long long ll;
typedef long double ld;
const int refresh_rate = 1;
int num_cores;


// check for file pointer to be null
// dont forget to close all the file pointers in the ends

// returns battery level, percentage and charging/discharging state
void getBatteryStats(int* percentage, char* charging, char* level)
{
    FILE* bat1;
    FILE* bat2;
    FILE* bat3;
    bat1 = fopen("/sys/class/power_supply/BAT1/capacity","r");
    bat2 = fopen("/sys/class/power_supply/BAT1/status","r");
    bat3 = fopen("/sys/class/power_supply/BAT1/capacity_level","r");
    if(bat1==NULL || bat2==NULL || bat3==NULL)
    {
        *percentage = -1;
        charging = NULL;
        level = NULL;
        return;
    }
    char line1[256],line2[256],line3[256];
    fgets(line1,sizeof(line1),bat1);
    sscanf(line1,"%d",percentage);

    fgets(line2,sizeof(line2),bat2);
    sscanf(line2,"%s",charging);

    fgets(line3,sizeof(line3),bat3);
    sscanf(line3,"%s",level);

    fclose(bat1);
    fclose(bat2);
    fclose(bat3);    
}

// returns the total number of cores in the machine
int getNumCores()
{
    FILE* cpuinfo;
    cpuinfo = fopen("/proc/cpuinfo","r");
    if(cpuinfo==NULL)
    {
        return -1;
    }
    char line[256];
    char target[] = "siblings";
    int cores; 
    while(fgets(line,sizeof(line),cpuinfo))
    {
        char* found = (char*) malloc(sizeof(char)*9);
        strncpy(found,line,8);
        if(strcmp(found,target)==0)
        {
            sscanf(line,"%*[^0123456789]%d",&cores);
            break;
        }
    }
    fclose(cpuinfo);
    return cores;   
}

// returns memory information ar[4] = {mem_total, mem_used,swap_total, swap_used}
ll* getMemInfo()
{
    FILE* meminfo;
    meminfo = fopen("/proc/meminfo","r");
    if(meminfo==NULL)
    {
        return NULL;
    }
    ll* ar = (ll*)malloc(sizeof(ll)*4);
    char line[256];
    while(fgets(line,sizeof(line),meminfo))
    {
        char word[20];
        ll val;
        sscanf(line,"%s %lld",word,&val);

        if(strcmp(word,"MemTotal:")==0)
        {
            ar[0]=val;
        }
        else if(strcmp(word,"MemAvailable:")==0)
        {
            ar[1]=val;
        }
        else if(strcmp(word,"SwapTotal:")==0)
        {
            ar[2]=val;
        }
        else if(strcmp(word,"SwapFree:")==0)
        {
            ar[3]=val;
            break;
        }
        // printf("word : %s\n",word);
        // printf("val : %d\n",val);
    }
    fclose(meminfo);
    return ar;
}



// returns ar[4] = {hh,mm,ss,percentage_idle}
int* getUptimes()
{
    FILE* ut;
    ut = fopen("/proc/uptime","r");
    if(ut==NULL)
    {
        return NULL;
        // file not opened
    }
    float up_time,idle_time;
    fscanf(ut,"%f",&up_time);
    fscanf(ut,"%f",&idle_time);
    int* ret_val = (int*)malloc(3*sizeof(int));

    ret_val[0] = (int)(up_time/(60*60));
    ret_val[1] = (int)(up_time/60) - (60*ret_val[0]);
    ret_val[2] = (int)(up_time - (60*ret_val[1]) - (60*60*ret_val[0]));
    ret_val[3] = (int)(((idle_time/num_cores)/up_time)*100);
    
    fclose(ut);
    return ret_val;
}


// main method
int main ()
{
    // hiding the console cursor
    printf("\33[?25l");

    // initializong the global constants
    num_cores = getNumCores();
    
    // initializing the clock for infinite loop
    int val = clock();

    while(1)
    {
        if(clock()%(refresh_rate*CLOCKS_PER_SEC)==0)
        {
            system("clear");
            printf("----------%sTOP PROGRAM----------\n\n",KCYN);

            // printing the uptime
            int* ar = getUptimes();
            if(ar!=NULL)
            {   
                printf("UpTime :\t%d:",ar[0]);
                printf(ar[1]<10?"0":"");
                printf("%d:",ar[1]);
                printf(ar[2]<10?"0":"");
                printf("%d\n",ar[2]);
            }

            //printing the idle percentage
            printf("Idle :\t\t%d%%\n",ar[3]);

            // ptinting the number of cores
            printf("Cores :\t\t%d\n",num_cores);

            // printing RAM and swap info
            ll* mem = getMemInfo();
            float rt = ((float)mem[0])/BTOG;
            float ru = ((float)(mem[0]-mem[1])/BTOG);
            float swt = ((float)mem[2])/BTOG;
            float swu = ((float)(mem[2]-mem[3])/BTOG);
            printf("Memory used :\t%.2f/%.2f GB\n",ru,rt);
            printf("Swap used   :\t%.2f/%.2f GB\n",swu,swt);

            // battery stats
            char bat_level[20];
            char bat_status[20];
            int bat_percentage;
            getBatteryStats(&bat_percentage,bat_status,bat_level);
            printf("\nBattery Stats : %s - %d%% (%s)\n",bat_status,bat_percentage,bat_level);
            
        }
    }

    // re-enabling the console cursor *IMPORTANT
    printf("\33[?25h");
    return 0;
}


// int main()
// {
//     ll* ar = getMemInfo();
//     for(int x=0;x<4;x++)
//     {
//         printf("%lld\n",ar[x]);
//     }
//     return 0;

// }
