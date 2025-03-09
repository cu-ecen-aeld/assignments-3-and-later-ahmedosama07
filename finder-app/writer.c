#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>
#include <syslog.h>
#include <linux/limits.h>


int create_directory(const char *path) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    if (path == NULL || strlen(path) == 0) {
        return -1;
    }

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    // Remove trailing slash
    if (len > 0 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
        len--;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0777)) {
                if (errno != EEXIST) {
                    return -1;
                }
            }
            *p = '/';
        }
    }

    // Create the final directory
    if (mkdir(tmp, 0777)) {
        if (errno != EEXIST) {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    openlog(argv[0], LOG_PID, LOG_USER);
    atexit(closelog);
    
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <writefile> <writestr>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *writefile = argv[1];
    char *writestr = argv[2];

    // Extract directory from writefile
    char *path_copy = strdup(writefile);
    if (path_copy == NULL) {
        syslog(LOG_ERR, "strdup failed: %m");
        exit(EXIT_FAILURE);
    }

    char *dir = dirname(path_copy);

    if (create_directory(dir) != 0) {
        syslog(LOG_ERR, "Failed to create directory %s: %m", dir);
        free(path_copy);
        exit(EXIT_FAILURE);
    }

    free(path_copy);

    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        perror("fopen");
        syslog(LOG_ERR, "Failed to open file %s: %m", writefile);
        exit(EXIT_FAILURE);
    }

    if (fprintf(file, "%s", writestr) < 0) {
        syslog(LOG_ERR, "Failed to write to file %s: %m", writefile);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (fclose(file) != 0) {
        syslog(LOG_ERR, "Failed to close file %s: %m", writefile);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}