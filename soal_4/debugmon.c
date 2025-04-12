#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER 256
#define BUFFER2 512
#define ll long long

void listproc(const char *user) {
    struct passwd *pwd = getpwnam(user);
    if (pwd == NULL) {
        fprintf(stderr, "Error: User does not exist\n");
        exit(EXIT_FAILURE);
    } 
    uid_t userUID = pwd->pw_uid;
    
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL) {
        fprintf (stderr, "Error: Unable to open the content of /proc/meminfo\n");
        exit(EXIT_FAILURE);
    }

    char line[BUFFER];
    ll memtotal = 0;

    while (fgets(line, sizeof(line), meminfo)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %lld kB", &memtotal);
            break;
        } 
    }

    fclose(meminfo);

    DIR *proc = opendir("/proc");
    if (proc == NULL) {
        fprintf (stderr, "Error: Unable to open folder /proc\n");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    printf("%-8s %-8s %-8s %-8s %s\n", "PID", "USER", "\%CPU", "%MEM", "COMMAND");

    while ((entry = readdir(proc)) != NULL) {
        if (!isdigit(entry->d_name[0])) {
            continue;
        }

        char procStatusPath[BUFFER2];
        snprintf(procStatusPath, sizeof(procStatusPath), "/proc/%s/status", entry->d_name);

        FILE *status = fopen(procStatusPath, "r");
        if (status == NULL) {
            continue;
        }

        uid_t uid;
        char command[BUFFER];
        ll memused = 0;
        char state;

        while (fgets(line, sizeof(line), status)) {
            if (strncmp(line, "Uid:", 4) == 0) {
                sscanf(line, "Uid:\t%d", &uid);
            }
            else if (strncmp(line, "Name:", 5) == 0) {
                sscanf(line, "Name:\t%s", command);
            }
            else if (strncmp(line, "VmRSS:", 6) == 0) {
                sscanf(line, "VmRSS:\t%lld kB", &memused);
            }
            else if (strncmp(line, "State:", 6) == 0) {
                sscanf(line, "State:\t%c", &state);
            }
        }

        fclose(status);

        if (uid != userUID || (state != 'R' && state != 'S')) {
            continue;
        }

        double memusedPercentage;
        if (memtotal > 0) {
            memusedPercentage = 100.0 * memused / memtotal;
        }
        else {
            memusedPercentage = 0.0;
        }

        char procStatPath[BUFFER2];
        snprintf(procStatPath, sizeof(procStatPath), "/proc/%s/stat", entry->d_name);

        FILE *stat = fopen(procStatPath, "r");
        if (stat == NULL) {
            continue;
        }

        unsigned long utime, stime, starttime;
        char ignore[BUFFER];
        for (int i = 0; i < 13; i++) {
            fscanf(stat, "%s", ignore);
        }
        fscanf(stat, "%lu %lu", &utime, &stime);

        for (int i = 0; i < 4; i++) {
            fscanf(stat, "%s", ignore);
        }
        fscanf(stat, "%lu", &starttime);
        
        fclose(stat);

        FILE *uptime = fopen("/proc/uptime", "r");
        if (uptime == NULL) {
            continue;
        }

        double sysuptime;
        fscanf(uptime, "%lf", &sysuptime);
        
        fclose(uptime);

        unsigned long ticks = sysconf(_SC_CLK_TCK);
        double timespent = utime + stime;
        double elapsedtime = sysuptime - (starttime / ticks);
        double cpuusage = 100 * ((timespent / ticks) / elapsedtime);

        printf("%-8s %-8s %-8.2f %-8.2f %s\n", entry->d_name, user, cpuusage, memusedPercentage, command);

    }

    closedir(proc);
}

void daemonrun(const char *user) {
    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Daemon for %s created successfully\n", user);
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    char daemonPID[BUFFER];
    snprintf(daemonPID, sizeof(daemonPID), "/tmp/debugmon_%s.pid", user);

    FILE *pidfile = fopen(daemonPID, "w");
    if (pidfile == NULL) {
        fprintf(stderr, "Error: Unable to open %s PID file\n", user);
        exit(EXIT_FAILURE);
    } 
    else {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }

    struct passwd *pwd = getpwnam(user);
        if (pwd == NULL) {
            fprintf(stderr, "Error: Failed to get user's UID\n");
            exit(EXIT_FAILURE);
        } 
        uid_t userUID = pwd->pw_uid;

    while (1) {
        FILE *logfile = fopen("/tmp/debugmon.log", "a");
        if (logfile == NULL) {
            fprintf(stderr, "Error: Unable to open log file\n");
            exit(EXIT_FAILURE);
        }

        DIR *proc = opendir("/proc");
        if (proc == NULL) {
            fprintf (stderr, "Error: Unable to open folder /proc\n");
            exit(EXIT_FAILURE);
        }

        struct dirent *entry;

        uid_t uid;
        char line[BUFFER], command[BUFFER];
        char state;

        time_t rawtime = time(NULL);
        struct tm *timeinfo = localtime(&rawtime);
        char currenttime[32];

        strftime(currenttime, sizeof(currenttime), "[%d-%m-%Y]-[%H:%M:%S]", timeinfo);

        while ((entry = readdir(proc)) != NULL) {
            if (!isdigit(entry->d_name[0])) {
                continue;
            }
            
            char procStatusPath[BUFFER2];
            snprintf(procStatusPath, sizeof(procStatusPath), "/proc/%s/status", entry->d_name);

            FILE *status = fopen(procStatusPath, "r");
            if (status == NULL) {
                continue;
            }

            while (fgets(line, sizeof(line), status)) {
                if (strncmp(line, "Uid:", 4) == 0) {
                    sscanf(line, "Uid:\t%d", &uid);
                }
                else if (strncmp(line, "Name:", 5) == 0) {
                    sscanf(line, "Name:\t%s", command);
                }
                else if (strncmp(line, "State:", 6) == 0) {
                    sscanf(line, "State:\t%c", &state);
                }
            }

            fclose(status);

            if (uid == userUID && (state == 'R' || state == 'S')) {
                fprintf(logfile, "%s_%s_STATUS(RUNNING)\n", currenttime, command);
            }
        }

        closedir(proc);
        fclose(logfile);

        sleep(5);
    }
}

void daemonstop(const char *user) {
    char daemonPID[BUFFER];
    snprintf(daemonPID, sizeof(daemonPID), "/tmp/debugmon_%s.pid", user);

    FILE *pidfile = fopen(daemonPID, "r");
    if (pidfile == NULL) {
        fprintf(stderr, "Error: User PID not found\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    if (fscanf(pidfile, "%d", &pid) != 1) {
        fprintf(stderr, "Error: Failed to read PID from file\n");
        fclose(pidfile);
        exit(EXIT_FAILURE);
    }

    fclose(pidfile);

    if (kill(pid, SIGTERM) != 0) {
        fprintf(stderr, "Error: Failed to terminate daemon process\n");
        exit(EXIT_FAILURE);
    }

    if (remove(daemonPID) != 0) {
        fprintf(stderr, "Error: Failed to remove PID file\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Daemon for %s stopped successfully\n", user);
    }
}

bool isblocked(const char *user) {
    FILE *blockedlist = fopen("/tmp/debugmon_blocked.txt", "r");
    if (blockedlist == NULL) {
        return false;
    }
    else {
        char line[BUFFER];

        while (fgets(line, sizeof(line), blockedlist)) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, user) == 0) {
                fclose(blockedlist);
                return true;
            } 
        }
        fclose(blockedlist);
        return false;
    }
}

void failproc(const char *user) {
    struct passwd *pwd = getpwnam(user);
    if (pwd == NULL) {
        fprintf(stderr, "Error: User does not exist\n");
        exit(EXIT_FAILURE);
    }

    if (isblocked(user)) {
        fprintf(stderr, "Error: User is already blocked\n");
        exit(EXIT_FAILURE);
    }
    else {
        FILE *blockedlist = fopen("/tmp/debugmon_blocked.txt", "a");
        if (blockedlist == NULL) {
            fprintf(stderr, "Error: Unable to open blocked user file\n");
            exit(EXIT_FAILURE);
        }
        else {
            fprintf(blockedlist, "%s\n", user);
            fclose(blockedlist);

            char daemonPID[BUFFER];
            snprintf(daemonPID, sizeof(daemonPID), "/tmp/debugmon_%s.pid", user);
            if (access(daemonPID, F_OK) == 0) {
                daemonstop(user);
            }

            printf("%s has been blocked successfully\n", user);
        }
    }
}

void blockaccess(const char *user, const char *command) {
    FILE *logfile = fopen("/tmp/debugmon.log", "a");
    if (logfile == NULL) {
        fprintf(stderr, "Error: Unable to open log file\n");
        exit(EXIT_FAILURE);
    }
    else {
        time_t rawtime = time(NULL);
        struct tm *timeinfo = localtime(&rawtime);
        char currenttime[32];

        strftime(currenttime, sizeof(currenttime), "[%d-%m-%Y]-[%H:%M:%S]", timeinfo);

        fprintf(logfile, "%s_./debugmon %s %s_STATUS(FAILED)\n", currenttime, command, user);
        fclose(logfile);
    }

    fprintf(stderr, "Error: User is blocked from using this program\n");
    exit(EXIT_FAILURE);
}

void revertfail(const char *user) {
    FILE *blockedlist = fopen("/tmp/debugmon_blocked.txt", "r");
    if (blockedlist == NULL) {
        fprintf(stderr, "Error: Unable to open blocked user file\n");
        exit(EXIT_FAILURE);
    }

    FILE *tmp = fopen("/tmp/debugmon_tmp.txt", "w");
    if (tmp == NULL) {
        fclose(blockedlist);
        fprintf(stderr, "Error: Unable to open tmp file\n");
        exit(EXIT_FAILURE);
    }

    char line[BUFFER];
    bool found = false;
    while (fgets(line, sizeof(line), blockedlist)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, user) != 0) {
            fprintf(tmp, "%s\n", line);
        } else {
            found = true;
        }
    }

    fclose(blockedlist);
    fclose(tmp);

    if (found) {
        remove("/tmp/debugmon_blocked.txt");
        rename("/tmp/debugmon_tmp.txt", "/tmp/debugmon_blocked.txt");
        printf("%s has been unblocked successfully\n", user);
    }
    else {
        remove("/tmp/debugmon_blocked_tmp.txt");
        fprintf(stderr, "%s is not in the blocked list\n", user);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error: Not enough arguments\n");
        exit(EXIT_FAILURE);
    }
    else if (argc > 3) {
        fprintf(stderr, "Error: Too many arguments\n");
        exit(EXIT_FAILURE);
    }
    
    const char *command = argv[1];
    const char *user = argv[2];

    if (isblocked(user)) {
        if (strcmp(command, "revert") != 0) {
            blockaccess(user, command);
        }
    }

    if (!strcmp(command, "list")) {
        listproc(user);
    }
    else if (!strcmp(command, "daemon")) {
        daemonrun(user);
    }
    else if (!strcmp(command, "stop")) {
        daemonstop(user);
    }
    else if (!strcmp(command, "fail")) {
        failproc(user);
    }
    else if (!strcmp(command, "revert")) {
        revertfail(user);
    }
    else {
        fprintf(stderr, "Error: Unknown argument\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}