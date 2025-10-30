// userIdFromName() 8-1 pg159
// Inspect Name: and Uid: lines of all /proc/PID/status
// // walking through all /proc/PID needs readdir(3) 18.8
// Make sure directory still exists at time of access

// uid_t userIdFromName(const char *name)
// {
//   struct passwd *pwd;
//   uid_t u;
//   char *endptr;

//   if (name == NULL || *name == '\0') return -1; // NULL or empty string

//   u = strtol(name, &endptr, 10);
//   if (*endptr == '\0') return u;

//   pwd = getpwnam(name);
//   if (pwd == NULL) return -1;

//   return pwd->pw_uid;
// }

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

static int is_all_digits(const char *s) {
    if (!s || !*s) return 0;
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p)
        if (!isdigit(*p)) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <user>\n", argv[0]);
        return 1;
    }

    uid_t target = userIdFromName(argv[1]);
    if (target == (uid_t)-1) {
        fprintf(stderr, "unknown user: %s\n", argv[1]);
        return 1;
    }

    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("opendir /proc");
        return 1;
    }

    struct dirent *de;
    char path[128];
    char line[4096];

    while ((de = readdir(dir)) != NULL) {
        if (!is_all_digits(de->d_name)) continue;

        // Build /proc/PID/status
        int n = snprintf(path, sizeof path, "/proc/%s/status", de->d_name);
        if (n < 0 || (size_t)n >= sizeof path) continue;

        FILE *fp = fopen(path, "r");
        if (!fp) {
            if (errno == ENOENT) continue; // PID vanished
            continue;                       // other errors, skip
        }

        unsigned r = (unsigned)-1, e = (unsigned)-1, s = 0, f = 0;
        char pname[256] = {0};

        while (fgets(line, sizeof line, fp)) {
            if (!pname[0] && strncmp(line, "Name:", 5) == 0) {
                char *val = line + 5;
                while (*val == ' ' || *val == '\t') val++;
                size_t L = strcspn(val, "\r\n");
                if (L >= sizeof pname) L = sizeof pname - 1;
                memcpy(pname, val, L);
                pname[L] = '\0';
            } else if (strncmp(line, "Uid:", 4) == 0) {
                // Uid: real  effective  saved  fs
                (void)sscanf(line, "Uid:%u%u%u%u", &r, &e, &s, &f);
                // We can break after Uid if Name is already found, but not required
            }
        }
        fclose(fp);

        if (r == (unsigned)target || e == (unsigned)target) {
            printf("%s %s\n", de->d_name, pname[0] ? pname : "(unknown)");
        }
    }

    closedir(dir);
    return 0;
}
