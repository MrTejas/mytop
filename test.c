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


void read_ints (const char* file_name)
{
    FILE* file = fopen (file_name, "r");
    ll i = 0;
    char str[20];
    int count=0;  

    fscanf (file, "%lld", &i);  
    fscanf (file, "%s", str);
    fscanf (file, "%s", str);
    while (!feof (file))
    {  
        printf ("%lld\t\t\t%d\n", i,count);
        count++;
        fscanf (file, "%lld", &i);      
    }
    fclose (file);        
}

int main()
{
    printf("\e[?25h");
    read_ints("/proc/4267/stat");

    return 0;
}

