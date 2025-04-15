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
## • Soal 2 : Starterkit

### • Pendahuluan

Soal 2 terdiri dari enam subsoal, di mana subsoal A–E merupakan argumen dari `./starterkit`, subsoal F merupakan error handling sederhana untuk mencegah penggunaan yang salah pada program, dan soal G merupakan sistem logging aktivitas `starterkit`. Program ini dirancang untuk berjalan sebagai daemon yang memonitor direktori tertentu dan melakukan tindakan seperti karantina file terenkripsi, pengembalian file, penghapusan file permanen, serta penghentian proses daemon itu sendiri. Masing-masing argumen menjalankan fungsi spesifik yang diimplementasikan dalam bahasa pemrograman C dengan memanfaatkan fitur-fitur sistem operasi Linux seperti manajemen proses, direktori, sinyal, serta encoding dan decoding base64. Adapun tampilan `main()` untuk dapat menjalankan argumen pada starterkit adalah sebagai berikut :
```c
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
```
Dimana 
```c
printf("[*] Kanade's starter kit initializing...\n");
download_zip();
unzip_file();
delete_zip();
printf("[+] Starter kit setup complete.\n");
```
Baris-baris ini menjalankan tiga fungsi awal untuk:
- Mengunduh file ZIP (starterkit.zip)
- Mengekstraknya ke folder starter_kit
- Menghapus file ZIP setelah berhasil diekstrak
```c
if (argc == 2 && strcmp(argv[1], "--decrypt") == 0) {
    daemonize();
    write_pid();
    char msg[128];
    snprintf(msg, sizeof(msg), "Successfully started decryption process with PID %d.", getpid());
    write_log(msg);
    daemon_loop();
}
```
adalah blok kode yang akan dieksekusi jika argumen yang diberikan ke program adalah --decrypt. Function ini berkaitan langsung dengan subsoal A.
```c
} else if (argc == 2 && strcmp(argv[1], "--quarantine") == 0) {
    quarantine_files();
    exit(0);
}
```
adalah untuk menangani argumen --quarantine yang berkaitan dengan Subsoal B, yaitu fitur karantina file berbahaya.
```c
} else if (argc == 2 && strcmp(argv[1], "--return") == 0) {
    return_files();
    exit(0);
}
```
adalah pengondisian argumen dari program debugmon. Tujuannya adalah menangani perintah --return dari pengguna.
```c
} else if (argc == 2 && strcmp(argv[1], "--eradicate") == 0) {
    eradicate_files();
    exit(0);
}
```
Bagian kode ini adalah pengondisian argumen untuk menangani perintah `--eradicate` dari pengguna. Tujuannya adalah menghapus permanen file yang sebelumnya telah dikarantina oleh program debugmon.
```c
} else if (argc == 2 && strcmp(argv[1], "--shutdown") == 0) {
    shutdown_daemon();
    exit(0);
}
```
Bagian kode ini adalah percabangan yang akan dijalankan jika program debugmon dipanggil dengan argumen `--shutdown`. Tujuannya: menghentikan proses daemon debugmon yang sedang berjalan di latar belakang.
```c
} else {
    fprintf(stderr, "[!] Invalid argument: %s\n", argv[1]);
    print_usage();
    return 1;
}
```
Blok else ini berfungsi sebagai penjaga terakhir (fallback) untuk menangani input yang tidak dikenal, agar pengguna tidak menjalankan perintah yang salah tanpa disadari.

### • Soal 2.A
Pada soal sub bab 2.A, kita ditugaskan untuk membuat fitur untuk mendownload dan unzip sebuah starter kit berisi file - file acak (sudah termasuk virus) melalui link. Juga ada fitur untuk menghapus file zip asli setelah melakukan unzip. Function nya yaitu sebagai berikut :
```c
void download_zip() {
    char *argv[] = {
        "wget", "--no-check-certificate", "-q",
        "-O", (char *)ZIP_FILE,
        (char *)("https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download"),
        NULL
    };
    run_command(argv);
}

void unzip_file() {
    char *mkdir_argv[] = { "mkdir", "-p", (char *)EXTRACT_DIR, NULL };
    run_command(mkdir_argv);

    char *unzip_argv[] = { "unzip", "-o", (char *)ZIP_FILE, "-d", (char *)EXTRACT_DIR, NULL };
    run_command(unzip_argv);
}

void delete_zip() {
    char *rm_argv[] = { "rm", "-f", (char *)ZIP_FILE, NULL };
    run_command(rm_argv);
}
```
Program akan mengunduh file ZIP dari internet, menyimpannya sebagai starterkit.zip, mengekstraknya ke direktori starter_kit, lalu menghapus file ZIP tersebut. Kemudian untuk menjalankan function tersebut, kita menggunakan command seperti berikut :
```
./starterkit
```
### • Soal 2.B
Pada soal ini, kita membuat fitur yang bisa untuk membuat sebuah directory karantina yang dapat mendecrypt nama file yang ada di dalamnya menggunakan algoritma base64. Functionnya yaitu sebagai berikut :
```c
void daemonize() {
    pid_t pid = fork();
    if (pid > 0) exit(0); // parent keluar
    setsid(); // jadi session leader
    chdir("/home/zenal_24/Praktikum_Sisop25/modul2/");
    close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);
}

void decrypt_and_move_files() {
    DIR *dir = opendir(EXTRACT_DIR);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;
        int out_len = base64_decode(entry->d_name, decoded);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", QUARANTINE_DIR, decoded);
        rename(src_path, dst_path);
    }
    closedir(dir);
}

void daemon_loop() {
    while (1) {
        decrypt_and_move_files();
        sleep(5);
    }
}
```
Program menjalankan sebuah daemon (background process) untuk mendekripsi nama file terenkripsi (Base64) yang ada di direktori starter_kit. Hasil dekripsi akan dipindahkan ke direktori quarantine. Untuk menjalankan program tersebut, bisa menggunakan argumen berikut :
```
./starterkit --decrypt
```
### • Soal 2.C
Pada soal ini, kita ditugaskan untuk menambah fitur yang bisa untuk memindahkan file yang ada pada directory starter kit ke directory karantina, dan begitu juga sebaliknya. Berikut function yang diberikan :
```c
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
```
Program akan memindahkan file yang ada pada directory starter kit ke directory karantina, dan begitu juga sebaliknya. Untuk function ini, ada 2 argumen yang dipakai, yaitu untuk memindahkan file yang ada di directory `quarantine` ke directory `starterkit`, dan dari directory starterkit ke quarantine. Berikut `argument` yang digunakan :
```
./starterkit --quarantine
```
Argumen tersebut untuk memindahkan file dari directory starter kit ke karantina.
```
./starterkit --return
```
Argumen tersebut untuk memindahkan file dari directory karantina ke starter kit.
### • Soal 2.D
Pada soal ini, kita ditugaskan untuk menambahkan fitur untuk menghapus seluruh file yang ada di dalam directory karantina. Berikut function yang digunakan :
```c
void eradicate_files() {
    DIR *dir = opendir(QUARANTINE_DIR);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) {
            snprintf(path, ..., "%s/%s", QUARANTINE_DIR, entry->d_name);
            remove(path);
            write_log(...);
        }
    }
    closedir(dir);
}
```
Program tersebut akan menghapus semua file di dalam direktori quarantine secara permanen. Berikut argumen yang digunakan :
```
./starterkit --eradicate
```
### • Soal 2.E
Pada soal ini, kita akan membuat fitur untuk mematikan program decrypt nama file secara aman berdasarkan PID dari proses program. Function yang dipakai adalah sebagai berikut :
```c
void shutdown_daemon() {
    FILE *fp = fopen(PID_FILE, "r");
    fscanf(fp, "%d", &pid);
    kill(pid, SIGTERM); // mengirim sinyal ke daemon
    write_log(...);
    remove(PID_FILE);
}
```
Program akan menghentikan proses daemon dan PID dari proses disimpan di file starterkit.pid. Argumen yang digunakan untuk program tersebut adalah sebagai berikut :
```
./starterkit --shutdown
```
### • Soal 2.F
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

Dimana langkah implementasinya:

```c
void brain_damage_2(const char *user) {
	...
}
```
1. Mendeklarasikan `brain_damage_2()` dengan ketentuan:
- `const char *user`: Nama user yang di-passing dari `main()` yang nantinya akan ditampilkan proses-proses yang sedang dijalankannya.

```c
struct passwd *pwd = getpwnam(user);
if (pwd == NULL) {
	fprintf(stderr, "Error: User does not exist\n");
	exit(EXIT_FAILURE);
} 
uid_t userUID = pwd->pw_uid;
```
2. Mengambil data UID user dari entry user yang disimpan pada `/etc/passwd` dan menyimpannya ke dalam variabel userUID. Apabila tidak ditemukan user yang sesuai pada `/etc/passwd`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
FILE *meminfo = fopen("/proc/meminfo", "r");
if (meminfo == NULL) {
	fprintf (stderr, "Error: Unable to open the content of /proc/meminfo\n");
	exit(EXIT_FAILURE);
}
```
3. Membuka file `/proc/meminfo` untuk membaca data jumlah memori total yang terdapat pada perangkat yang menjalankan program debugmon. Apabila tidak ditemukan atau tidak dapat membuka `/proc/meminfo`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

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
4. Membaca input dari `/proc/meminfo` dan mencari baris yang mempunyai prefix `MemTotal:`. Jika ditemukan, maka data jumlah total memori perangkat dalam satuan kB diambil dan disimpan ke dalam variabel `memtotal`.

```c
fclose(meminfo);
```
5. Menutup kembali file `/proc/meminfo`.

```c
DIR *proc = opendir("/proc");
if (proc == NULL) {
	fprintf (stderr, "Error: Unable to open folder /proc\n");
	exit(EXIT_FAILURE);
}
```
6. Membuka direktori `/proc` yang berisi file-file yang berhubungan dengan proses yang ada pada sistem user. Apabila tidak ditemukan atau tidak dapat membuka `/proc`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
struct dirent *entry;
```
7. Mendeklarasikan struct yang berisi directory entry untuk setiap file proses yang terdapat pada `/proc`.

```c
printf("%-8s %-8s %-8s %-8s %s\n", "PID", "USER", "\%CPU", "%MEM", "COMMAND");
```
8. Mengoutput heading untuk kolom PID, USER, STATUS, %CPU, %MEM, dan COMMAND ke stdout, hanya sebagai aspek desain estetika.

```c
 while ((entry = readdir(proc)) != NULL) {
	if (!isdigit(entry->d_name[0])) {
		 continue;
	}

	...
}
```
9. Membaca setiap nama file proses yang terdapat pada `/proc`, umumnya nama file proses hanya terdiri atas angka yang merepresentasikan PID-nya. Oleh karena itu, file lain yang bukan merupakan sebuah proses seperti `meminfo` dan `cpuinfo` akan dilewati. Selain itu, jika sudah tidak ada nama file lagi untuk dibaca maka, while-loop akan bernilai false dan loop akan berhenti.

```c
char procStatusPath[BUFFER2];
snprintf(procStatusPath, sizeof(procStatusPath), "/proc/%s/status", entry->d_name);

FILE *status = fopen(procStatusPath, "r");
if (status == NULL) {
	continue;
}
```
10. Nama file untuk setiap proses yang telah dibaca kemudian disematkan ke dalam `/proc/[PID]/status` yang juga merupakan sebuah file. Setelah itu, setiap file `/proc/[PID]/status` dibuka untuk dibaca data seperti UID, memori yang dipakai, status, dan nama command setiap proses. Apabila file tidak dapat dibaca maka file akan dilewati.

```c
uid_t uid;
char command[BUFFER];
ll memused = 0;
char state;
```
11. Mendeklarasikan variabel-variabel, dimana:
- `uid`: untuk menyimpan data UID suatu proses.
- `command[]`: untuk menyimpan nama suatu proses.
- `memused`: untuk menyimpan besar memori yang dipakai suatu proses.
- `state`: untuk menyimpan status suatu proses, seperti running, idle, zombie, stopped, atau lainnya.

```c
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
```
12. Membaca input dari `/proc/[PID]/status` dan mencari baris yang mempunyai prefix `UID:`, `Name:`, `VmRSS`, dan `State:`. Jika ditemukan, maka data UID, nama command, memori yang dipakai, dan status suatu proses diambil dan disimpan ke dalam variabel yang berkaitan. 

```c
fclose(status);
```
13. Menutup kembali file `/proc/[PID]/status`.

```c
if (uid != userUID || (state != 'R' && state != 'S')) {
	continue;
}
```
14. Memastikan bahwa UID proses yang ditemukan pada `/proc/[PID]/status` sesuai dengan UID user yang menjadi target dari program dan juga memastikan bahwa status proses yang ditemukan pada `/proc/[PID]/status` adalah antara `R` (running) atau `S` (sleeping). Jika kondisi tidak terpenuhi, maka proses tersebut akan dilewati.

```c
double memusedPercentage;
if (memtotal > 0) {
    memusedPercentage = 100.0 * memused / memtotal;
}
else {
    memusedPercentage = 0.0;
}
```
15. Memastikan bahwa memori total tidak bernilai nol supaya tidak terjadi pembagian dengan nol dan apabila terjadi kasus dimana memori total bernilai nol, maka nilai besar memori yang dipakai suatu proses dalam bentuk persentase dianggap nol. Namun, jika memori total memiliki nilai lebih dari nol, maka besar memori yang dipakai suatu proses dalam bentuk persentase dapat dihitung dan disimpan di variabel `memusedPercentage`.

```c
char procStatPath[BUFFER2];
snprintf(procStatPath, sizeof(procStatPath), "/proc/%s/stat", entry->d_name);

FILE *stat = fopen(procStatPath, "r");
if (stat == NULL) {
    continue;
}
```
16. Nama file untuk setiap proses yang telah dibaca kemudian disematkan ke dalam `/proc/[PID]/stat` yang juga merupakan sebuah file. Setelah itu, setiap file `/proc/[PID]/stat` dibuka untuk dibaca data yang berkaitan dengan penggunaan cpu setiap proses. Apabila file tidak dapat dibaca maka file akan dilewati.

```c
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
```
17. Membaca input dari `/proc/[PID]/stat` dan meng-loop untuk mencari argumen ke-14 (utime), ke-15 (stime), dan ke-22 (starttime). Jika ditemukan, maka data utime, stime, dan starttime suatu proses diambil dan disimpan ke dalam variabel yang berkaitan.

```c
fclose(stat);
```
18. Menutup kembali file `/proc/[PID]/stat`.

```c
FILE *uptime = fopen("/proc/uptime", "r");
if (uptime == NULL) {
    continue;
}
```
19. Membuka file `/proc/uptime` untuk membaca data uptime sistem setiap kali iterasi loop. Apabila terdapat kasus dimana file tidak dapat dibaca maka file akan dilewati pada iterasi tersebut.

```c
double sysuptime;
fscanf(uptime, "%lf", &sysuptime);
```
20. Membaca input dari `/proc/uptime` dan mengambil data uptime sistem dalam satuan detik. Setelah itu, disimpan ke dalam variabel `sysuptime`.

```c
fclose(uptime);
```
21. Menutup kembali file `/proc/uptime`.

```c
unsigned long ticks = sysconf(_SC_CLK_TCK);
double timespent = utime + stime;
double elapsedtime = sysuptime - (starttime / ticks);
double cpuusage = 100 * ((timespent / ticks) / elapsedtime);
```
22. Menghitung persentase cpu usage yang digunakan oleh sebuah proses dan menyimpannya ke dalam variabel `cpuusage`.

```c
printf("%-8s %-8s %-8.2f %-8.2f %s\n", entry->d_name, user, cpuusage, memusedPercentage, command);
```
23. Mengoutput data PID, nama user, cpu usage, memory usage, dan nama command setiap proses yang dijalankan user dengan ketentuan satu baris diisi oleh satu proses ke stdout.

```c
closedir(proc);
```
24. Menutup kembali direktori `/proc`.

### • Soal 4.B: Activity Logging Daemon

Pada subsoal 4.B: Activity Logging Daemon, kita diperintahkan untuk membuat sebuah program daemon untuk memantau setiap command/proses yang dijalankan oleh target user dan menyimpannya ke dalam suatu file log. Untuk membuat program ini dibuatlah function bernama `run_daemon_to_log_user_activity()`, dengan tampilan sebagai berikut:

```c
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
```

Dimana langkah implementasinya:

```c
void run_daemon_to_log_user_activity(const char *user) {
	...
}
```
1. Mendeklarasikan `run_daemon_to_log_user_activity()` dengan ketentuan:
- `const char *user`: Nama user yang di-passing dari `main()` yang nantinya akan dipantau proses-proses yang sedang dijalankannya secara daemon.

```c
pid_t pid, sid;

pid = fork();
if (pid == -1) {
	exit(EXIT_FAILURE);
}

if (pid > 0) {
	exit(EXIT_SUCCESS);
}
```
2. Membuat sebuah child process menggunakan `fork()` dan memastikan bahwa child process berhasil dibuat. Apabila tidak, maka program akan keluar. Setalah itu, proses dengan PID bukan nol (parent process) dimatikan.

```c
sid = setsid();
if (sid < 0) {
	exit(EXIT_FAILURE);
}
```
3. Membuat session ID (SID) yang unik untuk child process agar dapat berjalan.

```c
if ((chdir("/")) < 0) {
	exit(EXIT_FAILURE);
}
```
4. Memindahkan working directory child process ke `/` (root).

```c
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
```
5. Menutup file descriptor standar.

```c
char daemonPID[BUFFER];
snprintf(daemonPID, sizeof(daemonPID), "/tmp/debugmon_%s.pid", user);

FILE *pidfile = fopen(daemonPID, "w");
if (pidfile == NULL) {
	fprintf(stderr, "Error: Unable to open %s PID file\n", user);
	exit(EXIT_FAILURE);
}
```
6. Membuat file baru pada folder `/tmp` dengan nama `debugmon_[USER].pid` untuk menyimpan data PID daemon yang dijalankan untuk suatu user. Apabila tidak dapat membuka `/tmp/debugmon_[USER].pid`, maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
else {
        fprintf(pidfile, "%d\n", getpid());
	fclose(pidfile);
}
```
7. Mengubah output dari stdout ke `/tmp/debugmon_[USER].pid` dan menyimpan data PID daemon di dalamnya. Setelah itu, menutup kembali file `/tmp/debugmon_[USER].pid`.

```c
struct passwd *pwd = getpwnam(user);
if (pwd == NULL) {
	fprintf(stderr, "Error: Failed to get user's UID\n");
	exit(EXIT_FAILURE);
} 
uid_t userUID = pwd->pw_uid;
```
8. Mengambil data UID user dari entry user yang disimpan pada `/etc/passwd` dan menyimpannya ke dalam variabel userUID. Apabila tidak ditemukan user yang sesuai pada `/etc/passwd`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
while (1) {
	...
	sleep(5);
}
```
9. Membuat sebuah while-loop dengan kondisi `1` (true) sehingga akan berjalan secara indefinitely kecuali jika ada faktor eksternal seperti munculnya suatu error. Selain itu, loop memiliki jeda waktu selama lima detik supaya program dapat berjalan dengan stabil dan tidak menghabiskan CPU.

```c
char activityLogPath[BUFFER2];
snprintf(activityLogPath, sizeof(activityLogPath), "/tmp/debugmon_%s.log", user);

FILE *logfile = fopen(activityLogPath, "a");
if (logfile == NULL) {
	fprintf(stderr, "Error: Unable to open log file\n");
	exit(EXIT_FAILURE);
}
```
10. Membuat file baru pada folder `/tmp` dengan nama `debugmon_[USER].log` untuk menyimpan data log proses yang sedang dijalankan oleh suatu user. Apabila tidak dapat membuka `/tmp/debugmon_[USER].log`, maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
DIR *proc = opendir("/proc");
if (proc == NULL) {
	fprintf (stderr, "Error: Unable to open folder /proc\n");
	exit(EXIT_FAILURE);
}
```
11. Membuka direktori `/proc` yang berisi file-file yang berhubungan dengan proses yang ada pada sistem user. Apabila tidak ditemukan atau tidak dapat membuka `/proc`,  maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
struct dirent *entry;
```
12. Mendeklarasikan struct yang berisi directory entry untuk setiap file proses yang terdapat pada `/proc`.

```c
uid_t uid;
char line[BUFFER], command[BUFFER];
char state;
```
13. Mendeklarasikan variabel-variabel, dimana:
- `uid`: untuk menyimpan data UID suatu proses.
- `line[]`: untuk menyimpan data satu baris penuh pada suatu file.
- `command[]`: untuk menyimpan nama suatu proses.
- `state`: untuk menyimpan status suatu proses, seperti running, idle, zombie, stopped, atau lainnya.

```c
time_t rawtime = time(NULL);
struct tm *timeinfo = localtime(&rawtime);
char currenttime[32];

strftime(currenttime, sizeof(currenttime), "[%d-%m-%Y]-[%H:%M:%S]", timeinfo);
```
14. Mengambil data waktu lokal saat program dijalankan dan menyimpannya ke dalam variabel currenttime dalam format `[DD-MM-YYYY]-[HH:MM:SS]`.

```c
while ((entry = readdir(proc)) != NULL) {
	if (!isdigit(entry->d_name[0])) {
	continue;
	}

	...
}
```
15. Membaca setiap nama file proses yang terdapat pada `/proc`, umumnya nama file proses hanya terdiri atas angka yang merepresentasikan PID-nya. Oleh karena itu, file lain yang bukan merupakan sebuah proses seperti `meminfo` dan `cpuinfo` akan dilewati. Selain itu, jika sudah tidak ada nama file lagi untuk dibaca maka, while-loop akan bernilai false dan loop akan berhenti.

```c
char procStatusPath[BUFFER2];
snprintf(procStatusPath, sizeof(procStatusPath), "/proc/%s/status", entry->d_name);

FILE *status = fopen(procStatusPath, "r");
if (status == NULL) {
	continue;
}
```
16. Nama file untuk setiap proses yang telah dibaca kemudian disematkan ke dalam `/proc/[PID]/status` yang juga merupakan sebuah file. Setelah itu, setiap file `/proc/[PID]/status` dibuka untuk dibaca data seperti UID, memori yang dipakai, status, dan nama command setiap proses. Apabila file tidak dapat dibaca maka file akan dilewati.

```c
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
```
17. Membaca input dari `/proc/[PID]/status` dan mencari baris yang mempunyai prefix `UID:`, `Name:`, dan `State:`. Jika ditemukan, maka data UID, nama command, dan status suatu proses diambil dan disimpan ke dalam variabel yang berkaitan.

```c
fclose(status);
```
18. Menutup kembali file `/proc/[PID]/status`.

```c
if (uid == userUID && (state == 'R' || state == 'S')) {
	fprintf(logfile, "%s_%s_STATUS(RUNNING)\n", currenttime, command);
}
```
19. Memastikan bahwa UID proses yang ditemukan pada `/proc/[PID]/status` sesuai dengan UID user yang menjadi target dari program dan juga memastikan bahwa status proses yang ditemukan pada `/proc/[PID]/status` adalah antara `R` (running) atau `S` (sleeping). Jika kondisi tidak terpenuhi, maka proses tersebut akan dilewati.

```c
closedir(proc);
```
20. Menutup kembali direktori `/proc`.

```c
fclose(logfile);
```
21. Menutup kembali file `/tmp/debugmon_[USER].log` (logfile).

### • Soal 4.C: Stop Daemon

Pada subsoal 4.C: Stop Daemon, kita diperintahkan untuk membuat sebuah program untuk mematikan program daemon yang telah dijalankan sebelumnya menggunakan `run_daemon_to_log_user_activity()`. Untuk membuat program ini dibuatlah function bernama `stop_daemon_to_log_user_activity()`, dengan tampilan sebagai berikut:

```c
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
```

Dimana langkah implementasinya:

```c
char daemonPID[BUFFER];
snprintf(daemonPID, sizeof(daemonPID), "/tmp/debugmon_%s.pid", user);

FILE *pidfile = fopen(daemonPID, "r");
if (pidfile == NULL) {
	fprintf(stderr, "Error: User PID not found\n");
	exit(EXIT_FAILURE);
}
```
1. Membaca file yang dibuat oleh `run_daemon_to_log_user_activity()` pada folder `/tmp` dengan nama `debugmon_[USER].pid`, yang digunakan untuk menyimpan data PID daemon yang dijalankan untuk suatu user. Apabila tidak dapat membuka `/tmp/debugmon_[USER].pid`, maka program akan keluar setelah melempar sebuah error ke stderr yang akan ditampilkan ke user.

```c
pid_t pid;
```
2. Menyimpan data PID daemon yang diambil dari `/tmp/debugmon_[USER].pid` ke dalam variabel `pid`.
### • Soal 4.D: Fail User's System
### • Soal 4.E: Revert Failing
### • Soal 4.F: Debugmon Log File
### • Kendala Pengerjaan Soal
## • Revisi
### • Revisi Soal 1
### • Revisi Soal 2
### • Revisi Soal 3
### • Revisi Soal 4
