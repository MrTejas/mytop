#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>

// defining colours
#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"

// bytes to gigs convertor const
#define BTOG 1048576

typedef long long ll;
typedef long double ld;
typedef struct process* proc;
const int refresh_rate = 1;
int num_cores;


// check for file pointer to be null
// dont forget to close all the file pointers in the ends

struct process 
{
    char* command; // command that opened the process
    char* name;  
    char* state;
    int pid; // pid of the process
    int ppid; // pid of the parent of the process
    double cpu; //cpu percentage used by the process
    int tty; // the tty terminal process is using
    ll time; // stores the running time of the process in seconds
    int priority; // stores the priority for realtime/non realtime scheduled processes
    int nice; // nice value of priority range {19 to -20} (High to Low) 
    int user_id; // user id of the user using the process
    ll res; // resident memory of the process
    ll virt; // virtual memory of the process
};


proc createProcess(int pid)
{
    proc P = (proc)malloc(sizeof(struct process));
    if(P==NULL)
    {
        printf("unable to create struct!\n");
        return NULL;
    }
    char nil[20]="nil";
    P->cpu=-1;
    P->pid=pid;
    P->ppid=-1;
    P->tty=-1;
    P->command = (char*)malloc(20*sizeof(char));
    P->name = (char*)malloc(20*sizeof(char));
    P->state = (char*)malloc(5*sizeof(char));
    P->time=-1;
    P->user_id=-1;
    P->priority=101;
    P->nice = 21;
    P->res=-1;
    P->virt=-1;

    return P;
}


void getProcessMemory(int pid,ll* res, ll* vm)
{
    char path[20] = "/proc/";
    char str[10];
    sprintf(str,"%d",pid);
    strcat(path,str);
    strcat(path,"/status");
    
    FILE* fp;
    fp = fopen(path,"r");

    if(fp==NULL)
    {
        printf("could not open file %s\n",path);
        return;
    }

    char line[256];
    while(fgets(line,sizeof(line),fp))
    {
        char word[30];
        ll val;
        sscanf(line,"%s %lld",word,&val);

        if(strcmp(word,"VmSize:")==0)
        {
            *vm=val;
        }
        else if(strcmp(word,"VmRSS:")==0)
        {
            *res=val;
            break;
        }
    }
    fclose(fp);
}

// to work upon
void updateProcess(int pid, proc P)
{
    char path[20] = "/proc/";
    char path1[20] = "/proc/";
    char str[10];
    sprintf(str,"%d",pid);
    
    strcat(path,str);
    strcat(path1,str);
    
    strcat(path,"/stat");
    strcat(path1,"/loginuid");

    FILE* fp;
    fp = fopen(path,"r");
    // printf("path = %s\n",path);

    char line[256];
    fgets(line,sizeof(line),fp);
    char temp[10];
    sscanf(line,"%s%s%s",temp,P->name,P->state);

    FILE* lid = fopen(path1,"r");
    char string_uid [10];
    fgets(line,sizeof(line),lid);
    sscanf(line,"%d",&P->user_id);
    fclose(lid);

    // printf("temp = %s\nP->name = %s\nP->state = %s\n",temp,P->name,P->state);
    
    // fp = fopen("/proc/4378/stat","r");

    printf("ch3\n");

    fclose(fp);
    FILE* fpp = fopen(path,"r");
    ll val = 0;
    char strr[20];
    int count=0;  

    fscanf (fpp, "%lld", &val);  
    printf("ck4\n");
    fscanf (fpp, "%s", strr);
    fscanf (fpp, "%s", strr);


    // for calculating the cpu percentage
    printf("ck10\n");
    ld process_utime,process_stime,process_starttime;
    ld ticks = sysconf( _SC_CLK_TCK );
    // printf("ticks = %Lf\n",ticks);
    FILE* ut = fopen("/proc/uptime","r");
    ld system_uptime;
    fscanf(ut,"%Lf",&system_uptime);
    printf("system_uptime = %Lf\n",system_uptime);
    fclose(ut);


    while (!feof (fpp))
    {  
        // printf ("%d\t\t\t%lld\n", count,val);
        // here we have got the count(th) integer in the file
        if(count==0)
        {
            P->pid=val;
        }
        else if(count==3)
        {
            P->ppid=val;
        }
        else if(count==15)
        {
            P->priority=val;
        }
        else if(count==16)
        {
            P->nice=val;
        }
        else if(count==6)
        {
            P->tty=val;
        }
        else if(count==11)
        {
            process_utime=val/ticks;
            // printf("count = %d\tval = %lld\n",count,val);
        }
        else if(count==12)
        {
            process_stime=val/ticks;
            // printf("count = %d\tval = %lld\n",count,val);
        }
        else if(count==19)
        {
            process_starttime=val/ticks;
            // printf("count = %d\tval = %lld\n",count,val);
        }
        

        // cpu percentage calculation
        
        ld process_elapsed = system_uptime-process_starttime;
        ld process_used = process_utime + process_stime;
        ld cpu_percentage = (process_used * 100)/process_elapsed;
        P->cpu=cpu_percentage;
        P->time=((ll)process_elapsed);
        
        

        count++;
        fscanf (fpp, "%lld", &val);
    }
    fclose (fpp);  

    // obtaining memory figures of the pid
    getProcessMemory(P->pid,&P->res,&P->virt);

    
}

void displayProcess(proc P)
{
    if(P!=NULL)
    {
        printf("\n************************\n");
        printf("Displaying Process...\n");
        printf("pid = \t\t%d\nppid = \t\t%d\ncpu = \t\t%.2f%%\n",P->pid,P->ppid,P->cpu);
        printf("tty = \t\t%d\ntime = \t\t%lld s\npriority = \t%d\nnice = \t\t%d\nuser_id : \t%d\nres : \t\t%lld kb\nvirt : \t\t%lld kb\n",P->tty,P->time,P->priority,P->nice,P->user_id,P->res,P->virt);
        if(P->command!=NULL)
        {
            printf("command : %s\n",P->command);
        }
        if(P->name!=NULL)
        {
            printf("name : %s\n",P->name);
        }
        if(P->state!=NULL)
        {
            printf("state : %s\n",P->state);
        }
        printf("************************\n");
    }
}


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
        printf("could not open file /proc/meminfo\n");
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
int main()
{
    // hiding the console cursor (UNCOMMENT IN THE END)
    // printf("\33[?25l");

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
            printf("Battery Stats : %s - %d%% (%s)\n",bat_status,bat_percentage,bat_level);


            // ****************************Process Details******************************
            printf("\n\n");
            printf("PID\tUSER\tPRI\tNI\tRES\tSTATE\tCPU%%\tMEM%%\tTIME\tCOMMAND\n");
            
            DIR* dr = opendir("/proc");
            if(dr==NULL)
            {
                printf("Error opening /proc\n");
                return 0;
            }

            struct dirent* de;
            while((de = readdir(dr))!=NULL)
            {
                printf("%s\n",de->d_name);
            }
            closedir(dr);
        }
    }

    // re-enabling the console cursor *IMPORTANT
    printf("\33[?25h");
    return 0;
}


int main1()
{
    printf("\33[?25h");
    // proc P = (proc)malloc(sizeof(struct process));
    printf("ck1\n");
    proc P = createProcess(1876);
    printf("ck2\n");
    updateProcess(1876,P);
    displayProcess(P);
    return 0;

}