# Laporan Resmi Modul 2 Kelompok IT-07
## Anggota

| Nama 				| NRP		|
|-------------------------------|---------------|
| Muhammad Rakha Hananditya R.	| 5027241015 	|
| Zaenal Mustofa		| 5027241018 	|
| Mochkamad Maulana Syafaat	| 5027241021 	|

## • Soal 1
### • Soal 1.A
### • Soal 1.B
### • Soal 1.C
### • Soal 1.D
### • Soal 1.E
### • Kendala Pengerjaan Soal
## • Soal 2
### • Soal 2.A
### • Soal 2.B
### • Soal 2.C
### • Soal 2.D
### • Soal 2.E
### • SOal 2.F
### • Soal 2.G
### • Kendala Pengerjaan Soal
## • Soal 3
### • Kendala Pengerjaan Soal

## • Soal 4: Debugmon

### • Pendahuluan

Soal 4 terdiri dari enam subsoal, dimana subsoal A-E merupakan argumen dari `./debugmon` dan subsoal F merupakan sistem logging aktivitas debugmon. Adapun tampilan `main()` untuk dapat menjalankan argumen pada debugmon adalah sebagai berikut:
```c
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
```
Dimana:

```c
int main(int argc, char *argv[]) {
	...
}
```
Merupakan deklarasi `main()` dengan ketentuan:
- `int argc`: Jumlah argumen yang perlu di-passing ke dalam program.
- `char *argv[]`: Array yang berisi argumen-argumen yang di-passing ke dalam program.

```c
if (argc < 3) {
	fprintf(stderr, "Error: Not enough arguments\n");
	exit(EXIT_FAILURE);
}
else if (argc > 3) {
	fprintf(stderr, "Error: Too many arguments\n");
	exit(EXIT_FAILURE);
}
```
Memastikan bahwa jumlah argumen yang di-passing ke dalam program tidak lebih dan tidak kurang jumlahnya dari 3. Apabila hal tersebut terjadi, maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
const char *command = argv[1];
const char *user = argv[2];
```
Mendeklarasikan variabel yang merupakan tipe data char dengan sifat konstan (tidak bisa dirubah), dimana:
- `const char *command = argv[1];`: Mendeklarasikan variabel untuk menyimpan opsi debugmon yang hendak dijalankan berdasarkan argumen kedua yang di-passing ke dalam program.
- `const char *user = argv[2];`: Mendeklarasikan variabel untuk menyimpan user yang akan menjadi target opsi debugmon berdasarkan argumen ketiga yang di-passing ke dalam program.
  
Argumen pertama pada program ini tidak digunakan karena berisi nama program yang dijalankan, yaitu `./debugmon`. 

```c
if (is_user_on_the_f_up_list(user)) {
	if (strcmp(command, "revert") != 0) {
		user_cant_run_debugmon_no_more(user, command);
	}
}
```
Merupakan bagian dari subsoal 4.D: Fail User's System, dimana program memastikan apakah user memiliki akses untuk menjalankan program debugmon.

```c
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
```
Memeriksa value variabel `command` dan mengarahkannya ke cabang yang sesuai dan menjalankan function yang berkaitan. Apabila value command tidak dikenal, maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
exit(EXIT_SUCCESS);
```
Setelah function yang berkaitan berhasil dijalankan tanpa kendala, maka program dinyatakan berhasil dieksekusi dan keluar.

### • Soal 4.A: List User's Processes

Pada subsoal 4.A: List User's Processes, kita diperintahkan untuk membuat sebuah program layaknya `ps`, `top`, atau `htop` yang dapat menampilkan semua proses yang sedang berjalan pada suatu user dengan PID, nama command, CPU usage, dan memory usage-nya. Untuk membuat program ini dibuatlah function bernama `brain_damage_2()`, dengan tampilan sebagai berikut:
```c
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
```
Dimana:

```c
void brain_damage_2(const char *user) {
	...
}
```
Merupakan deklarasi `brain_damage_2()` dengan ketentuan:
- `const char *user`: Nama user yang di-passing dari `main()` yang nantinya akan ditampilkan proses-proses yang sedang dijalankannya.

```c
struct passwd *pwd = getpwnam(user);
if (pwd == NULL) {
	fprintf(stderr, "Error: User does not exist\n");
	exit(EXIT_FAILURE);
} 
uid_t userUID = pwd->pw_uid;
```
Mengambil data UID user dari entry user yang disimpan pada `/etc/passwd`. Apabila tidak ditemukan user yang sesuai pada `/etc/passwd`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
FILE *meminfo = fopen("/proc/meminfo", "r");
if (meminfo == NULL) {
	fprintf (stderr, "Error: Unable to open the content of /proc/meminfo\n");
	exit(EXIT_FAILURE);
}
```
Membuka file `/proc/meminfo` untuk mengambil data jumlah memori total yang terdapat pada perangkat yang menjalankan program debugmon. Apabila tidak ditemukan atau tidak dapat membuka `/proc/meminfo`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
char line[BUFFER];
ll memtotal = 0;

while (fgets(line, sizeof(line), meminfo)) {
	if (strncmp(line, "MemTotal:", 9) == 0) {
		sscanf(line, "MemTotal: %lld kB", &memtotal);
		break;
	} 
}
```
Mengambil input dari `/proc/meminfo` dan mencari baris yang mempunyai prefix `MemTotal:`. Jika ditemukan, maka data jumlah total memori perangkat diambil dan disimpan ke variabel `memtotal`.

```c
fclose(meminfo);
```
Menutup kembali file `/proc/meminfo`.

```c
DIR *proc = opendir("/proc");
if (proc == NULL) {
	fprintf (stderr, "Error: Unable to open folder /proc\n");
	exit(EXIT_FAILURE);
}
```
Membuka direktori `/proc` yang berisi file-file yang berhubungan dengan proses yang ada pada sistem user. Apabila tidak ditemukan atau tidak dapat membuka `/proc`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
struct dirent *entry;
```
Mendeklarasikan struct yang berisi directory entry untuk setiap file proses yang terdapat pada `/proc`.

```c
printf("%-8s %-8s %-8s %-8s %s\n", "PID", "USER", "\%CPU", "%MEM", "COMMAND");
```
Mengoutput heading untuk kolom PID, USER, STATUS, %CPU, %MEM, dan COMMAND ke stdout, hanya sebagai aspek desain estetika.

### • Soal 4.B: Activity Logging Daemon
### • Soal 4.C: Stop Daemon
### • Soal 4.D: Fail User's System
### • Soal 4.E: Revert Failing
### • Soal 4.F: Debugmon Log File
### • Kendala Pengerjaan Soal
## • Revisi
### • Revisi Soal 1
### • Revisi Soal 2
### • Revisi Soal 3
### • Revisi Soal 4
