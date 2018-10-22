#include <ext2fs/ext2fs.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

ext2_filsys current_fs;



static int open_filesystem(char *device)
{
    int retval;
    io_manager io_ptr = unix_io_manager;

   

    retval = ext2fs_open(device, 0, 0, 0, io_ptr, &current_fs);
    if (retval) {
        
        return retval;
    }
    return 0;
}

static int get_inode(char *f, ext2_ino_t *inode) {

    struct stat sb;

    if (stat(f, &sb) == -1) {
        perror("stat");
        return errno;
    }

    if ((sizeof(sb.st_ino) > sizeof(ext2_ino_t)) &&
        (sb.st_ino > 0xffffffff)) {
        
        return -1;
    }
    memcpy((void *)inode, (void *)&sb.st_ino, sizeof(ext2_ino_t));
    return 0;
}


#define MAX_PATH 4096
#define DF_COMMAND "/bin/df "


int get_fs_name(char *f, char **fs) {

    FILE *fp = NULL;
    char cmd[MAX_PATH + sizeof(DF_COMMAND) + 1];
    int retval;
    char *first_space;

    *fs = NULL;

    retval = snprintf(cmd, sizeof(cmd), DF_COMMAND"%s", f);
    if (retval >= sizeof(cmd) - 1) {
        
        goto fail;
    }

    fp = popen(cmd, "r");
    if (fp == NULL) {
        
        goto fail;
    }

    /* leave space for newline and space for null termination */
    *fs = malloc(MAX_PATH + 2);
    if (*fs == NULL) {
        
        goto fail;
    }

    /* throw away the header */
    if (fgets(*fs, MAX_PATH, fp) == NULL) {
       
        goto fail;
    }

    if (fgets(*fs, MAX_PATH, fp) == NULL) {
        
        goto fail;
    }

    /* assume the drive name has no spaces (i.e., the first space is a
     * delimiter)
     */
    first_space = strchr(*fs, ' ');
    if (!first_space) {
        goto fail;
    }
    *first_space = 0;

    pclose(fp);
    return 0;

fail:
    if (*fs)
        free(*fs);
    if (fp)
        pclose(fp);
    return -1;
}

int check_permissions(char *f) {
    int retval;

    retval = access(f, R_OK);
    if (retval) {
        perror(f);
    }
    return retval;
}

long int age(char *file) {

    ext2_ino_t inode;
    struct ext2_inode *inode_buf;
    struct ext2_inode_large *large_inode;
    int retval;
    char *fs;
    time_t rawtime;

    if (file==NULL) {
        
        return -1;
    }

    retval = check_permissions(file);
    if (retval) {
        return retval;
    }

    retval = get_inode(file, &inode);
    if (retval) {
        return retval;
    }

    retval = get_fs_name(file, &fs);
    if (retval) {
        return retval;
    }

    retval = open_filesystem(fs);
    if (retval) {
        free(fs);
        return retval;
    }

    inode_buf = (struct ext2_inode *)malloc(EXT2_INODE_SIZE(current_fs->super));
    if (!inode_buf) {
        free(fs);
        return -1;
    }

    retval = ext2fs_read_inode_full(current_fs, inode, inode_buf, EXT2_INODE_SIZE(current_fs->super));
    if (retval) {
        free(fs);
        free(inode_buf);
        return retval;
    }

    if (EXT2_INODE_SIZE(current_fs->super) <= EXT2_GOOD_OLD_INODE_SIZE) {
        free(fs);
        free(inode_buf);
        return -1;
    }

    large_inode = (struct ext2_inode_large *)inode_buf;
    if (large_inode->i_extra_isize < 24) {
        free(fs);
        free(inode_buf);
        return -1;
    }

    free(fs);
    free(inode_buf);
    ext2fs_close(current_fs);
    return large_inode->i_crtime;;
}

void agedir(char *path)
{
        DIR *d;
        struct dirent *dir;
        long int crtime;  
        struct stat stats;
        d = opendir(path);
        chdir(path);
        if(d)
        {
                while ((dir = readdir(d)) != NULL)
                {
                        crtime=age(dir->d_name);
                        stat(dir->d_name,&stats);
                        printf("%s ,", dir->d_name);
                        if(crtime<0)
                        {
                                printf("creation time not available,%ld,%ld",stats.st_mtime,stats.st_atime);
                                
                        }
                        else
                        {
                                printf("%ld,%ld,%ld\n",crtime,stats.st_mtime,stats.st_atime);
                        }
                }
        
                closedir(d);

        }

}

void agefile(char *path)
{
        long int crtime;
        struct stat stats;
        stat(path,&stats);
        printf("%s ,", path);
        if(crtime<0)
        {
               printf("creation time not available,%ld,%ld\n",stats.st_mtime,stats.st_atime);
               
        }
        else
        {
                printf("%ld,%ld,%ld\n",crtime,stats.st_mtime,stats.st_atime);         
        }
       
}

