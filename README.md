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
