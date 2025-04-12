#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER 256
#define BUFFER2 512
#define BUFFER3 526
#define ll long long

void brain_damage_2(const char *user) {
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

void run_daemon_to_log_user_activity(const char *user) {
    pid_t pid, sid;

    pid = fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
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
        char activityLogPath[BUFFER2];
        snprintf(activityLogPath, sizeof(activityLogPath), "/tmp/debugmon_%s.log", user);

        FILE *logfile = fopen(activityLogPath, "a");
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

void stop_daemon_to_log_user_activity(const char *user) {
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

    if (kill(pid, SIGKILL) != 0) {
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

bool is_user_on_the_f_up_list(const char *user) {
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

void f_up_the_selected_user_system(const char *user) {
    struct passwd *pwd = getpwnam(user);
    if (pwd == NULL) {
        fprintf(stderr, "Error: User does not exist\n");
        exit(EXIT_FAILURE);
    }

    if (is_user_on_the_f_up_list(user)) {
        fprintf(stderr, "Error: User is already prevented from using any commands. Have mercy\n");
        exit(EXIT_FAILURE);
    }
    else {
        FILE *blockedlist = fopen("/tmp/debugmon_blocked.txt", "a");
        if (blockedlist == NULL) {
            fprintf(stderr, "Error: Unable to open blocked list file\n");
            exit(EXIT_FAILURE);
        }

        fprintf(blockedlist, "%s\n", user);
        fclose(blockedlist);

        char daemonPID[BUFFER];
        snprintf(daemonPID, sizeof(daemonPID), "/tmp/debugmon_%s.pid", user);
        if (access(daemonPID, F_OK) == 0) {
            stop_daemon_to_log_user_activity(user);
        }

        uid_t userUID = pwd->pw_uid;

        char activityLogPath[BUFFER2];
        snprintf(activityLogPath, sizeof(activityLogPath), "/tmp/debugmon_%s.log", user);

        FILE *logfile = fopen(activityLogPath, "a");
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
            }

            fclose(status);

            if (uid == userUID) {
                pid_t pid = atoi(entry->d_name);
                if (pid > 0 && kill(pid, SIGKILL) == 0) {
                    printf("Killed process PID: %d\n", pid);
                    fprintf(logfile, "%s_%s_STATUS(FAILED)\n", currenttime, command);
                }
            }

        }

        closedir(proc);
        fclose(logfile);
    }
}

void user_cant_run_debugmon_no_more(const char *user, const char *command) {
    char activityLogPath[BUFFER2];
    snprintf(activityLogPath, sizeof(activityLogPath), "/tmp/debugmon_%s.log", user);

    FILE *logfile = fopen(activityLogPath, "a");
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

void run_commands_using_execvp(const char *command, char *const argv[]) {
    pid_t pid = fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execvp(command, argv);
        fprintf(stderr, "Error: Unable to execute execvp command\n");
        exit(EXIT_FAILURE);
    }
    else {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {}
        else {
            fprintf(stderr, "Error: Child process terminated abnormally\n");
            exit(EXIT_FAILURE); 
        }
    }
}
void user_cant_run_any_commands_no_more(char const *user) {
    struct passwd *pwd = getpwnam(user);
    if (pwd == NULL) {
        fprintf(stderr, "Error: User does not exist\n");
        exit(EXIT_FAILURE);
    }

    char homedir[BUFFER2];
    snprintf(homedir, sizeof(homedir), "%s", pwd->pw_dir);

    char bashProfilePath[BUFFER3];
    snprintf(bashProfilePath, sizeof(bashProfilePath), "%s/.bash_profile", homedir);

    char *argv1[] = {"cp", "/bin/bash", "/bin/rbash", NULL};
    run_commands_using_execvp("cp", argv1);

    char *argv2[] = {"usermod", "-s", "/bin/rbash", (char *)user, NULL};
    run_commands_using_execvp("usermod", argv2);

    FILE *bashprofile = fopen(bashProfilePath, "w");
    if (bashprofile == NULL) {
        fprintf(stderr, "Error: Unable to open bash profile\n");
        exit(EXIT_FAILURE);
    }

    fprintf(bashprofile,
            "# .bash_profile\n\n"
            "if [ -f ~/.bashrc ]; then\n"
            ". ~/.bashrc\n"
            "fi\n\n"
            "readonly PATH=$HOME/programs\n"
            "export PATH\n");
    
    fclose(bashprofile);

    char *argv3[] = {"chattr", "+i", bashProfilePath, NULL};
    run_commands_using_execvp("chattr", argv3);

    printf("%s has been blocked successfully\n", user);
}

void un_user_cant_run_any_commands_no_more(char const *user) {
    struct passwd *pwd = getpwnam(user);
    if (pwd == NULL) {
        fprintf(stderr, "Error: User does not exist\n");
        exit(EXIT_FAILURE);
    }

    char homedir[BUFFER2];
    snprintf(homedir, sizeof(homedir), "%s", pwd->pw_dir);

    char bashProfilePath[BUFFER3];
    snprintf(bashProfilePath, sizeof(bashProfilePath), "%s/.bash_profile", homedir);

    char *argv1[] = {"chattr", "-i", bashProfilePath, NULL};
    run_commands_using_execvp("chattr", argv1);


    FILE *bashprofile = fopen(bashProfilePath, "w");
    if (bashprofile == NULL) {
        fprintf(stderr, "Error: Unable to open bash profile\n");
        exit(EXIT_FAILURE);
    }

    fprintf(bashprofile,
            "# .bash_profile\n\n"
            "if [ -f ~/.bashrc ]; then\n"
            ". ~/.bashrc\n"
            "fi\n\n"
            "PATH=$PATH:$HOME/.local/bin:$HOME/bin\n"
            "export PATH\n");
    
    fclose(bashprofile);

    char *argv2[] = {"usermod", "-s", "/bin/bash", (char *)user, NULL};
    run_commands_using_execvp("usermod", argv2);

    char *argv3[] = {"rm", "/bin/rbash", NULL};
    run_commands_using_execvp("rm", argv3);

    printf("%s has been unblocked successfully\n", user);
}

void un_user_cant_run_debugmon_no_more(const char *user) {
    FILE *blockedlist = fopen("/tmp/debugmon_blocked.txt", "r");
    if (blockedlist == NULL) {
        fprintf(stderr, "Error: Unable to open blocked list file\n");
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

    if (is_user_on_the_f_up_list(user)) {
        if (strcmp(command, "revert") != 0) {
            user_cant_run_debugmon_no_more(user, command);
        }
    }

    if (!strcmp(command, "list")) {
        brain_damage_2(user);
    }
    else if (!strcmp(command, "daemon")) {
        run_daemon_to_log_user_activity(user);
    }
    else if (!strcmp(command, "stop")) {
        stop_daemon_to_log_user_activity(user);
    }
    else if (!strcmp(command, "fail")) {
        f_up_the_selected_user_system(user);
        user_cant_run_any_commands_no_more(user);
    }
    else if (!strcmp(command, "revert")) {
        un_user_cant_run_debugmon_no_more(user);
        un_user_cant_run_any_commands_no_more(user);
    }
    else {
        fprintf(stderr, "Error: Unknown argument\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
