#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>
#include <wchar.h>
#include <locale.h>

// defining colours
#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"
#define KBLK  "\033[30m"

#define BBLK    "\033[0;40m"	
#define BRED    "\033[0;41m"	
#define BGRN    "\033[0;42m"	
#define BYEL    "\033[0;43m"	
#define BBLU    "\033[0;44m"	
#define BMAG    "\033[0;45m"	
#define BCYN    "\033[0;46m"	
#define BWHT    "\033[0;47m"	

#define FREG    "\033[0;31m"	
#define FBLD    "\033[1;31m"	
#define FLIN    "\033[2;31m"	
#define FITA    "\033[3;31m"	
#define FUND    "\033[4;31m"	
#define FBLN    "\033[5;31m"	
#define FREV    "\033[6;31m"	
#define FBCK    "\033[7;31m"	
#define FINV    "\033[8;31m"	

#define RESET   "\033[0m"

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define moveup(x) printf("\033[%dA",(x)) // Move up X lines;
#define movedown(x) printf("\033[%dB",(x)) // Move down X lines;
#define moveright(x) printf("\033[%dC",(x)) // Move right X column;
#define moveleft(x) printf("\033[%dD",(x)) // Move left X column;
#define cls() printf("\033[2J") // Clear screen


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

    // printf("ch3\n");

    fclose(fp);
    FILE* fpp = fopen(path,"r");
    ll val = 0;
    char strr[20];
    int count=0;  

    fscanf (fpp, "%lld", &val);  
    // printf("ck4\n");
    
    fscanf (fpp, "%s", strr);
    fscanf (fpp, "%s", strr);


    // for calculating the cpu percentage
    // printf("ck10\n");
    ld process_utime,process_stime,process_starttime;
    ld ticks = sysconf( _SC_CLK_TCK );
    // printf("ticks = %Lf\n",ticks);
    FILE* ut = fopen("/proc/uptime","r");
    ld system_uptime;
    fscanf(ut,"%Lf",&system_uptime);
    // printf("system_uptime = %Lf\n",system_uptime);
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

void displayProcess1(proc P)
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

// n-tab
void giveSpaces(char* s, int n)
{
    int len = strlen(s);
    ll diff = n-len;
    for(int x=0;x<diff;x++)
        printf(" ");
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
            // line = 'siblings : 8'
            // following statement finds the integer in the line
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

    ret_val[0] = (int)(up_time/(60*60)); // hours
    ret_val[1] = (int)(up_time/60) - (60*ret_val[0]); // minutes
    ret_val[2] = (int)(up_time - (60*ret_val[1]) - (60*60*ret_val[0])); // seconds
    ret_val[3] = (int)(((idle_time/num_cores)/up_time)*100); // idle percentage
    
    fclose(ut);
    return ret_val;
}

int isNumber(char* name)
{
    int n = strlen(name);
    for(int x=0;x<n;x++)
    {
        if(name[x]-'0'<0 || name[x]-'0'>9)
        {
            return 0;
        }
    }
    return 1;
}

void displayProcess(proc P)
{
    if(P->virt>=0 && P->res>=0)
    {
        {
            printf("%s",KWHT);
            printf("%d\t",P->pid);
        }
        {
            printf("%s",KBLU);
            printf("%d\t",P->user_id);
        }
        {
            if(P->priority==20)
            {
                printf("%s",KWHT);
                printf("%d\t",P->priority);
            }
            else
            {
                printf("%s",KCYN);
                printf("%d\t",P->priority);
            }
        }
        {
            if(P->nice<0)
            {
                printf("%s",KRED);
                printf("%d\t",P->nice);
            }
            else
            {
                printf("%s",KNRM);
                printf("%d\t",P->nice);
            }
        }
        {
            printf("%s",KCYN);
            float mb = (P->virt/(float)1024);
            if(mb<1024)
            {
                printf("%.2f\t",mb);
            }
            else if((mb/1024.0)<1024)
            {
                printf("%s",KWHT);
                printf("%.2f",mb/1024);
                printf("G\t");
                printf("%s",KCYN);
                mb/=1024;
            }
            else
            {
                printf("%s",KRED);
                printf("%.2f",(mb/(1024*1024)));
                printf("T\t");
                printf("%s",KCYN);
                mb/=(1024*1024);
            }
        }
        {
            float mb = (P->res/(float)1024);
            if(mb<1024)
                printf("%.2f\t",mb);
            else if((mb/1024.0)<1024)
                printf("%.2fG\t",mb/1024);
            else
                printf("%.2fT\t",(mb/(1024*1024)));
        }
        {
            printf("%s",KWHT);
            printf("%s     ",P->state);
        }
        {
            printf("%s",KWHT);
            printf("%.2f\t",P->cpu);
        }
        {
            printf("%s",KWHT);
            int up_time = P->time;
            int val0 = (int)(up_time/(60*60)); // hours
            int val1 = (int)(up_time/60) - (60*val0); // minutes
            int val2 = (int)(up_time - (60*val1) - (60*60*val0)); // seconds

            printf("%d:",val0);
            printf(val1<10?"0":"");
            printf("%d:",val1);
            printf(val2<10?"0":"");
            printf("%d",val2);

            printf("\t");
            
        }
        {
            printf("%s",KGRN);
            printf("%s",P->name);
            printf("%s",KNRM);
        }
        printf("\n");

    }
}

float* getCPUusage()
{
    FILE* cpustat;
    cpustat = fopen("/proc/stat","r");
    if(cpustat==NULL)
    {
        printf("could not open file /proc/stat\n");
        return NULL;
    }
    float* ar = (float*)malloc(sizeof(float)*(num_cores+1));
    char line[256];
    for(int x=0;x<=num_cores;x++)
    {
        fgets(line,sizeof(line),cpustat);
        // printf("line fetched : ");
        int val[7];
        char buf[10];
        sscanf(line,"%s %d %d %d %d %d %d %d",buf,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6]);
        int tot = 0;
        for(int y=0;y<7;y++)
        {
            tot+=val[y];
            // printf("%d\t",val[y]);
        }
        // printf("\n");
        float per = (val[3]*100)/((float)tot);
        ar[x]=100-per;
    }
    fclose(cpustat);
    return ar;
}

void displayCPU()
{

    setlocale(LC_ALL, "en_US.UTF-8");
    wchar_t box = 0x2592;
    int max_bar=8;
    
    // num_cores = getNumCores();
    float* ar;
    ar = getCPUusage();
    float max=0;
    for(int x=0;x<=num_cores;x++)
    {
        if(ar[x]>max)
            max=ar[x];
    }

    int X=83,Y=12;
    gotoxy(X,Y);
    printf("AVG");
    moveup(1);
    moveleft(3);
    printf("%s",KCYN);
    printf("%.1f%%",ar[0]);
    printf("%s",KNRM);
    moveup(1);
    moveleft(4);

    printf("%s",KGRN);
    for(int x=1;x<=max_bar*(ar[0]/max);x++)
    {
        printf("%lc", box);
        moveup(1);
        moveleft(1);
    }
    printf("%s",KNRM);


    for(int x=1;x<num_cores+1;x++)
    {
    // printf("here %f",max_bar*(ar[x]/(float)max));
        X-=6,Y=12;
        gotoxy(X,Y);
        
        printf("core%d",x);
        moveup(1);
        moveleft(5);
        printf("%s",KBLU);
        printf("%.1f%%",ar[x]);
        printf("%s",KNRM);
        moveup(1);
        moveleft(4);
        if(ar[x]==max)
        {
            printf("%s",KRED);
        }
        else
        {
            printf("%s",KBLU);
        }
        for(int y=1;y<=max_bar*(ar[x]/(float)max);y++)
        {
            printf("%lc", box);
            moveup(1);
            moveleft(1);
        }
        printf("%s",KNRM);

        // printf("core %d:\t%f\n",x-1,ar[x]);
    }
    
    // moveup(1);
    // moveleft(1);
    // printf("%lc", box);
    
    // printf("total %% = %f\n",ar[0]);
    gotoxy(1,3);
    // printf("lala\n");
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
    displayCPU();
    while(1)
    {
        if(clock()%(refresh_rate*CLOCKS_PER_SEC)==0)
        {
            system("clear");
            displayCPU();
            // gotoxy(1,1);
            printf(FLIN);
            printf("%s----------TOP PROGRAM----------\n\n",KCYN);
            printf(RESET);
            // printing the uptime
            int* upt = getUptimes();
            if(upt!=NULL)
            {   
                // printing the uptime in proper format
                printf("UpTime \t: %d:",upt[0]);
                printf(upt[1]<10?"0":"");
                printf("%d:",upt[1]);
                printf(upt[2]<10?"0":"");
                printf("%d\n",upt[2]);
            }

            //printing the idle percentage
            printf("Idle \t\t: %d%%\n",upt[3]);

            // ptinting the number of cores
            printf("Cores \t\t: %d\n",num_cores);

            // printing RAM and swap info
            {
                ll* mem = getMemInfo();
                float rt = ((float)mem[0])/BTOG; //mem_total
                float ru = ((float)(mem[0]-mem[1])/BTOG); //mem_used
                float swt = ((float)mem[2])/BTOG; //swap_total
                float swu = ((float)(mem[2]-mem[3])/BTOG); //swap_used
                printf("Memory used \t: %.2f/%.2f GB\n",ru,rt);
                printf("Swap used   \t: %.2f/%.2f GB\n",swu,swt);
            }

            // battery stats
            {
                char bat_level[20];
                char bat_status[20];
                int bat_percentage;
                getBatteryStats(&bat_percentage,bat_status,bat_level);
                printf("Bat\t\t: %s - %d%% \n\t\t  (%s)\n",bat_status,bat_percentage,bat_level);
            }
            printf("\n\n");
            // ****************************Process Details******************************
            printf("%s%s",BGRN,KBLK);
            // printf("PID\tUSER\tPRI\tNI\tRES\tSTATE\tCPU%%\tMEM%%\tTIME\tCOMMAND\n");
            printf("PID     USER    PRI     NI      VIR(mb) RES(mb) STATE CPU%%      TIME+   PROGRAM        \n");
            printf(RESET);
            DIR* dr = opendir("/proc");
            if(dr==NULL)
            {
                printf("Error opening /proc\n");
                return 0;
            }

            struct dirent* de;
            while((de = readdir(dr))!=NULL /*&& isdigit(de->d_name)*/)
            {
                if(isNumber(de->d_name))
                {
                    int pid_num;
                    char* ptr;
                    // sscanf(de->d_name,"%*[^0123456789]%d",&pid_num);
                    pid_num = (int)strtol(de->d_name,&ptr,10);
                    // printf("pid num = %d\t",pid_num);
                    proc P = createProcess(pid_num);
                    updateProcess(pid_num,P);
                    displayProcess(P);
                }
                // printf("%s",de->d_name);
                // printf("\n");
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
    proc P = createProcess(3774);
    printf("ck2\n");
    updateProcess(3774,P);
    displayProcess(P);
    return 0;

}

int main2()
{
    int ret = isNumber("123523");
    printf("%d\n",ret);

    int num = 12;
    char* ptr;
    num = (int)strtol("123523",&ptr,10);
    // sscanf("123523","%*[^0123456789]%d",&num);

    printf("the number is %d\n",num);
    return 0;
}

