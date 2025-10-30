#include "/home/trenston/workspace/TLPI/tlpi-book/lib/tlpi_hdr.h"
#include "/home/trenston/workspace/TLPI/tlpi-book/users_groups/ugid_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <pwd.h>

// userIdFromName()
// opendir()       -> open a directory stream (e.g. /proc)
// readdir()       -> read entries (each PID directory)
// closedir()      -> close the directory stream

// snprintf()      -> build file path safely (/proc/<pid>/status)
// fopen()         -> open a file for reading
// fgets()         -> read one line from file
// fclose()        -> close the file

// strncmp()       -> compare start of string (match "Name:" / "Uid:")
// sscanf()        -> parse formatted data from a string (extract uid, name)
// strcspn()       -> find/remove newline or unwanted chars

// perror()        -> print error message for last system error
// access()        -> check if file exists or is readable
// printf()        -> print output to console

int isAllDigits(const char *s)
{
    if (!s || !*s) return 0;
    while (*s)
    {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

void strip(char *str)
{
    char *copy = str;
    while (*copy)
    {
        if (*copy == '\t' || *copy == '\n')
        {
            *copy = '\0';
            return;
        }
        copy++;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) return -1;

    uid_t uid = userIdFromName(argv[1]);
    if (uid == (uid_t)-1) return -1;
    
    DIR *proc = opendir("/proc");

    struct dirent *entry;
    char path[128];
    char line[4096];

    // loop through each pid directory in proc
    while ((entry = readdir(proc)) != NULL)
    {
        if (!isAllDigits(entry->d_name)) continue;
        // printf("%s\n", entry->d_name);

        int j = snprintf(path, sizeof path, "/proc/%s/status", entry->d_name);
        if (j < 0) printf("/proc/<pid> not found\n");
        // printf("%s\n", path);

        FILE *file = fopen(path, "r");
        if (file == NULL) continue;

        char namebuf[256] = {0};  
        char *name = NULL;

        // loop through each line
        while (fgets(line, sizeof(line), file))
        {
            int cmp = strncmp(line, "Name:", 5);
            if (cmp == 0)
            {
                char *p = line + 6; // +1 for \t
                strncpy(namebuf, p, sizeof(namebuf) - 1);
                namebuf[sizeof(namebuf) - 1] = '\0';
                strip(namebuf);
                name = namebuf;
                continue; 
            }

            cmp = strncmp(line, "Uid:", 4);
            if (cmp == 0)
            {
                unsigned long ruid, euid, suid, fsuid;
                sscanf(line, "Uid:\t%lu\t%lu\t%lu\t%lu", &ruid, &euid, &suid, &fsuid);
                if (ruid == uid || euid == uid || suid == uid || fsuid == uid)
                {
                    if (name)
                    {
                        printf("Process ID: %s\nCommand Name: %s\n\n", entry->d_name, name);
                    }
                    break;
                }
            }
        }

        fclose(file);
    }

    closedir(proc);
    return 0;
}
