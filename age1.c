#include "age.h"

int main(int argc,char *argv[])
{
        
        if(argc!=2)
        {
                printf("Usage ./age1 <filename>\n");
                return 0;
        }

                
        struct stat path_stat;
        stat(argv[1], &path_stat);
        printf("File,Creation time,Modification time,Access time\n");
        if(S_ISDIR(path_stat.st_mode))
        {
                agedir(argv[1]);
        }
        else
        {
                agefile(argv[1]);    

        }
        return 0;
}




