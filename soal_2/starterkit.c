#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>

#define LOG_FILE "activity.log"
#define FILE_ID "1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS"
#define ZIP_FILE "starterkit.zip"
#define EXTRACT_DIR "starter_kit"
#define PID_FILE "starterkit.pid"
#define QUARANTINE_DIR "/home/zenal_24/Praktikum_Sisop25/modul2/quarantine"

void write_log(const char *message); 


void run_command(char *argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("[!] Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(argv[0], argv);
        perror("[!] Exec failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "[!] Command failed with status %d\n", WEXITSTATUS(status));
            exit(EXIT_FAILURE);
        }
    }
}

void download_zip() {
    printf("[*] Downloading ZIP...\n");
    char *argv[] = {
        "wget", "--no-check-certificate", "-q",
        "-O", (char *)ZIP_FILE,
        (char *)("https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download" FILE_ID),
        NULL
    };
    run_command(argv);
}

void unzip_file() {
    printf("[*] Unzipping ZIP...\n");
    char *mkdir_argv[] = { "mkdir", "-p", (char *)EXTRACT_DIR, NULL };
    run_command(mkdir_argv);

    char *unzip_argv[] = { "unzip", "-o", (char *)ZIP_FILE, "-d", (char *)EXTRACT_DIR, NULL };
    run_command(unzip_argv);
}

void delete_zip() {
    printf("[*] Deleting ZIP file...\n");
    char *rm_argv[] = { "rm", "-f", (char *)ZIP_FILE, NULL };
    run_command(rm_argv);
}

// Base64 decode helper
int base64_char(char c) {
    if ('A' <= c && c <= 'Z') return c - 'A';
    if ('a' <= c && c <= 'z') return c - 'a' + 26;
    if ('0' <= c && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

int base64_decode(const char *input, unsigned char *output) {
    int len = strlen(input);
    int out_len = 0, val = 0, valb = -8;

    for (int i = 0; i < len; i++) {
        if (input[i] == '=') break;
        int c = base64_char(input[i]);
        if (c == -1) return -1;
        val = (val << 6) + c;
        valb += 6;
        if (valb >= 0) {
            output[out_len++] = (val >> valb) & 0xFF;
            valb -= 8;
        }
    }
    output[out_len] = '\0';
    return out_len;
}

void move_files(const char *src_dir, const char *dst_dir) {
    mkdir(dst_dir, 0755);

    DIR *src = opendir(src_dir);
    if (!src) {
        fprintf(stderr, "[!] Source directory '%s' not found.\n", src_dir);
        return;
    }

    struct dirent *entry;
    char src_path[512], dst_path[512], log_msg[1024];

    while ((entry = readdir(src)) != NULL) {
        if (entry->d_type != DT_REG) continue;

        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, entry->d_name);

        if (rename(src_path, dst_path) == 0) {
            snprintf(log_msg, sizeof(log_msg),
                     "%s - Successfully moved to %s directory.",
                     entry->d_name,
                     strcmp(dst_dir, QUARANTINE_DIR) == 0 ? "quarantine" : "starter kit");
            write_log(log_msg);
        }
    }

    closedir(src);
}


void quarantine_files() {
    move_files(EXTRACT_DIR, QUARANTINE_DIR);
}

void return_files() {
    move_files(QUARANTINE_DIR, EXTRACT_DIR);
}

void eradicate_files() {
    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) {
        fprintf(stderr, "[!] Cannot open quarantine directory\n");
        return;
    }

    struct dirent *entry;
    char path[512], log_msg[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;

        snprintf(path, sizeof(path), "%s/%s", QUARANTINE_DIR, entry->d_name);
        if (remove(path) == 0) {
            snprintf(log_msg, sizeof(log_msg), "%s - Successfully deleted.", entry->d_name);
            write_log(log_msg);
        }
    }

    closedir(dir);
}



void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(0); // Parent exit

    setsid(); // Become session leader
    umask(0); // Reset file mode
    chdir("/home/zenal_24/Praktikum_Sisop25/modul2/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void decrypt_and_move_files() {
    mkdir(QUARANTINE_DIR, 0755);

    DIR *dir = opendir(EXTRACT_DIR);
    if (!dir) exit(EXIT_FAILURE);

    struct dirent *entry;
    char src_path[512], dst_path[512];
    unsigned char decoded[256];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;

        // Filter nama base64
        int len = strlen(entry->d_name);
        if (len % 4 != 0) continue;

        int valid = 1;
        for (int i = 0; i < len; i++) {
            char c = entry->d_name[i];
            if (!(isalnum(c) || c == '+' || c == '/' || c == '=')) {
                valid = 0;
                break;
            }
        }
        if (!valid) continue;

        // Base64 decode
        snprintf(src_path, sizeof(src_path), "%s/%s", EXTRACT_DIR, entry->d_name);
        int out_len = base64_decode(entry->d_name, decoded);
        if (out_len <= 0) continue;

        // Hapus newline
        if (decoded[out_len - 1] == '\n') decoded[out_len - 1] = '\0';

        snprintf(dst_path, sizeof(dst_path), "%s/%s", QUARANTINE_DIR, decoded);
        rename(src_path, dst_path);
    }

    closedir(dir);
}

void write_pid() {
    FILE *fp = fopen(PID_FILE, "w");
    if (fp) {
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    }
}

void daemon_loop() {
    mkdir(QUARANTINE_DIR, 0755);
    while (1) {
        decrypt_and_move_files();
        sleep(5); // setiap 5 detik
    }
}

void shutdown_daemon() {
    FILE *fp = fopen(PID_FILE, "r");
    if (!fp) {
        fprintf(stderr, "[!] PID file not found. Daemon might not be running.\n");
        return;
    }

    pid_t pid;
    fscanf(fp, "%d", &pid);
    fclose(fp);

    if (kill(pid, SIGTERM) == 0) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Successfully shut off decryption process with PID %d.", pid);
        write_log(msg);
        remove(PID_FILE);
    } else {
        perror("[!] Failed to kill daemon");
    }
}


void print_usage() {
    printf("Usage:\n");
    printf("  ./starterkit                 : Download & setup starter kit\n");
    printf("  ./starterkit --decrypt       : Jalankan daemon untuk mendekripsi nama file terenkripsi\n");
    printf("  ./starterkit --shutdown      : Hentikan daemon decrypt\n");
    printf("  ./starterkit --quarantine    : Pindahkan file dari starter_kit ke karantina\n");
    printf("  ./starterkit --return        : Pindahkan file dari karantina ke starter_kit\n");
    printf("  ./starterkit --eradicate     : Hapus seluruh file di direktori karantina\n");
}

void write_log(const char *message) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "[%d-%m-%Y][%H:%M:%S]", t);

    fprintf(fp, "%s - %s\n", timestamp, message);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    printf("[*] Kanade's starter kit initializing...\n");
    download_zip();
    unzip_file();
    delete_zip();
    printf("[+] Starter kit setup complete.\n");
    if (argc == 2 && strcmp(argv[1], "--decrypt") == 0) {
        // daemonize();
        // write_pid();
        // daemon_loop(); // terus berjalan
        daemonize();
        write_pid();
        char msg[128];
        snprintf(msg, sizeof(msg), "Successfully started decryption process with PID %d.", getpid());
        write_log(msg);
        daemon_loop();
    
    } else if (argc == 2 && strcmp(argv[1], "--quarantine") == 0) {
        quarantine_files();
        exit(0);
    } else if (argc == 2 && strcmp(argv[1], "--return") == 0) {
        return_files();
        exit(0);
    }else if (argc == 2 && strcmp(argv[1], "--eradicate") == 0) {
        eradicate_files();
        exit(0);
    }else if (argc == 2 && strcmp(argv[1], "--shutdown") == 0) {
        shutdown_daemon();
        exit(0);
    }else {
        fprintf(stderr, "[!] Invalid argument: %s\n", argv[1]);
        print_usage();
        return 1;
    }
    
        

    return 0;
}
