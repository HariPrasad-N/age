#include "age.h"
#include <time.h>
int main(int argc,char *argv[])
{
        
        if(argc!=2)
        {
                printf("Usage ./age <filename>\n");
                return 0;
        }

        long int crtime;
        int hours, minutes,seconds,days,months,years,diff;
        time_t currtime;
        time(&currtime);
            
        
        printf("%s ", argv[1]);
        
        crtime=age(argv[1]);
                        
        
        if(crtime<0)
        {
                printf("Age not available\n");
                
        }

        
        minutes=(currtime-crtime)/60;
        seconds=(currtime-crtime)%60;
        hours=minutes/60;
        minutes=minutes%60;
        days=hours/24;
        hours=hours%24;
        months=days/30;
        days=days%30;
        years=months/12;
        months=months%12;

        if(years!=0)
        {
                printf("%d years ",years);
        }
        if(months!=0)
        {
                printf("%d months ",months);
        }
        if(days!=0)
        {
                printf("%d days ",days);
        }
        
        printf("%d hours %d minutes %d seconds\n",hours,minutes,seconds);
        
        return 0;
}




