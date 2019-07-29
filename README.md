# UNIXCourses_Monitor_DLL

Monitor File and Directory Activities of Dynamically Linked Programs.
we are going to practive library injection and API hijacking. Please implement a "library call monitor"(LCM) program that is able to show the activities of an arbitrary binary running on a Linux operating system. The result should be output to either stderr or a filename, e.g., "fsmon.log". By default, the output is written to stderr. But you may write the output to a filename specfied by an environment variable "MONITOR_OUTPUT".

## Minimum Requirements
```
closedir  opendir   readdir   creat     open        read      write     dup 
dup2      close     lstat     stat      pwrite      fopen     fclose    fread 
fwrite    fgetc     fgets     fscanf    fprintf     chdir     chown     chmod 
remove    rename    link      unlink    readlink    symlink   mkdir     rmdir
```

## Display Function Call Parameters and Return Values
* For char * data type, you may optionally print it out as a string.
* For file descriptors (passed as an int), FILE*, and DIR* pointers, you can convert them to corresponding file names.
* For struct stat or its pointer, retrieve meaningful information from the structure. For example, file type, file size, and permissions.

## Sample
```
$ MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./fsmon.so head -n 1000 /etc/services > /dev/null
$ cat fsmon.log
# open("/etc/services", 0x0) = 4
# read("/etc/services", 0x7fff85d879b0, 8192) = 8192
# read("/etc/services", 0x7fff85d879b0, 8192) = 8192
# read("/etc/services", 0x7fff85d879b0, 8192) = 2799
# read("/etc/services", 0x7fff85d879b0, 8192) = 0
# close("/etc/services") = 0
# fflush("<STDOUT>") = 0 (0x0)
# fclose("<STDOUT>") = 0
# fflush("<STDERR>") = 0 (0x0)
# fclose("<STDERR>") = 0
```
```
$ LD_PRELOAD=./fsmon.so ls -la
# fopen("/proc/filesystems", "re") = 0x55a5b0caa270
# fclose("/proc/filesystems") = 0
# opendir(".") = 0x55a5b0cb3ac0
# readdir(".") = tests
# lstat("tests", 0x55a5b0caec78 {mode=00775, size=4096}) = 0
# fopen("/etc/passwd", "rme") = 0x55a5b0caa270
# fclose("/etc/passwd") = 0
# fopen("/etc/group", "rme") = 0x55a5b0caa270
# fclose("/etc/group") = 0
# readdir(".") = ..
# lstat("..", 0x55a5b0caed40 {mode=00775, size=4096}) = 0
# readdir(".") = .
# lstat(".", 0x55a5b0caee08 {mode=00775, size=4096}) = 0
# readdir(".") = fsmon.so
# lstat("fsmon.so", 0x55a5b0caeed0 {mode=00775, size=33352}) = 0
# readdir(".") = Makefile
# lstat("Makefile", 0x55a5b0caef98 {mode=00664, size=390}) = 0
# readdir(".") = fsmon.c
# lstat("fsmon.c", 0x55a5b0caf060 {mode=00664, size=21370}) = 0
# readdir(".") = NULL
# closedir(".") = 0
# fputs_unlocked(0x55a5af0e0cf8, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733bf505, "<STDOUT>") = 1
total 76
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc1ce0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc2300, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac270, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac271, 5, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cc2580, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac270, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
drwxrwxr-x  3 chuang chuang  4096 Apr  2 00:54 .
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc1ce0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc2300, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac271, 5, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cc2450, 1, 2, "<STDOUT>") = 2
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac270, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
drwxrwxr-x 35 chuang chuang  4096 Mar 26 20:28 ..
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc1ce0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc2300, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cc2910, 1, 7, "<STDOUT>") = 7
-rw-rw-r--  1 chuang chuang 21370 Apr  2 00:54 fsmon.c
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc1ce0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc2300, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac2c4, 5, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cc26b0, 1, 8, "<STDOUT>") = 8
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac270, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
-rwxrwxr-x  1 chuang chuang 33352 Apr  2 00:54 fsmon.so
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc1ce0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc2300, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cc27e0, 1, 8, "<STDOUT>") = 8
-rw-rw-r--  1 chuang chuang   390 Apr  1 23:48 Makefile
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc1ce0, "<STDOUT>") = 1
# fputs_unlocked(0x55a5b0cc2300, "<STDOUT>") = 1
# fputs_unlocked(0x7ffd733be2e0, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac271, 5, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cc2320, 1, 5, "<STDOUT>") = 5
# fwrite_unlocked(0x55a5af0e113c, 2, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5b0cac270, 1, 1, "<STDOUT>") = 1
# fwrite_unlocked(0x55a5af0e0d5e, 1, 1, "<STDOUT>") = 1
drwxrwxr-x  2 chuang chuang  4096 Apr  2 00:35 tests
# fflush_unlocked("<STDOUT>") = 0 (0x0)
# fflush("<STDOUT>") = 0 (0x0)
# fclose("<STDOUT>") = 0
# fflush("<STDERR>") = 0 (0x0)
# fclose("<STDERR>") = 0
$
```

## hints
Basically we have two different symbol tables. One is the regular symbol table and the other is dynamic table. The one removed by strip is the regular symbol table. So you will need to work with **nm -D** or **readelf --syms** to read the dynamic symbol table.
