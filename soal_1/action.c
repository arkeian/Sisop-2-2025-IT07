#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define CLUES_URL "https://drive.google.com/uc?export=download&id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK"
#define ZIP_FILE "Clues.zip"
#define FOLDER_NAME "Clues"

void downloaddanunzip() {
    struct stat st = {0};
    if (stat(FOLDER_NAME, &st) == -1) {
        pid_t pid = fork();
        if (pid == 0) {
            char *args[] = {"wget", "-q", "--no-check-certificate", CLUES_URL, "-O", ZIP_FILE, NULL};
            execvp("wget", args);
            perror("gagal");
            exit(1);
        }
        wait(NULL);
        pid = fork();
        if (pid == 0) {
            char *args[] = {"unzip", "-q", ZIP_FILE, NULL};
            execvp("unzip", args);
            perror("gagal");
            exit(1);
        }
        wait(NULL);
        remove(ZIP_FILE);
        printf("Download dan unzip selesai\n");
    } else {
        printf("Folder Clues sudah ada\n");
    }
}

void filterfile() {
    mkdir("Filtered", 0755);
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"find", "Clues", "-type", "f", "-name", "[a-zA-Z0-9].txt", "-exec", "mv", "{}", "Filtered/", ";", NULL};
        execvp("find", args);
        perror("gagal");
        exit(1);
    }
    wait(NULL);
    pid = fork();
    if (pid == 0) {
        char *args[] = {"find", "Clues", "-type", "f", "!", "-name", "[a-zA-Z0-9].txt", "-delete", NULL};
        execvp("find", args);
        perror("gagal");
        exit(1);
    }
    wait(NULL);
    printf("Filter berhasil\n");
}

void combinefile() {
    FILE *combined = fopen("Combined.txt", "w");
    if (!combined) {
        perror("gagal");
        return;
    }

    for (int i = 0; i < 26; i++) {
        char numFile[256], letterFile[256];
        snprintf(numFile, sizeof(numFile), "Filtered/%d.txt", i+1);
        FILE *file = fopen(numFile, "r");
        if (file) {
            int c;
            while ((c = fgetc(file)) != EOF) {
                fputc(c, combined);
            }
            fclose(file);
            remove(numFile);
        }

        snprintf(letterFile, sizeof(letterFile), "Filtered/%c.txt", 'a' + i);
        file = fopen(letterFile, "r");
        if (file) {
            int c;
            while ((c = fgetc(file)) != EOF) {
                fputc(c, combined);
            }
            fclose(file);
            remove(letterFile);
        }
    }
    
    fclose(combined);
    printf("Combined file berhasil\n");
}

void decode() {
    FILE *combined = fopen("Combined.txt", "r");
    if (!combined) {
        perror("gagal");
        return;
    }

    FILE *decoded = fopen("Decoded.txt", "w");
    if (!decoded) {
        perror("gagal");
        fclose(combined);
        return;
    }

    int c;
    while ((c = fgetc(combined)) != EOF) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = base + (c - base + 13) % 26;
        }
        fputc(c, decoded);
    }

    fclose(combined);
    fclose(decoded);
    printf("Decode berhasil\n");
}

void command() {
    printf("Gunakan:\n");
    printf("./action\n");
    printf("./action -m Filter\n");
    printf("./action -m Combine\n");
    printf("./action -m Decode\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        downloaddanunzip();
    } 
    else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0) {
            filterfile();
        } 
        else if (strcmp(argv[2], "Combine") == 0) {
            combinefile();
        } 
        else if (strcmp(argv[2], "Decode") == 0) {
            decode();
        } 
        else {
            printf("Invalid command.\n");
            command();
        }
    } 
    else {
        command();
    }
    return 0;
}
