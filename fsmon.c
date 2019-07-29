#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>
#include <stdarg.h>
#include "def.h"

#define CNT_ARGS( ... ) CNT_ARGS_(dummy, ## __VA_ARGS__,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define CNT_ARGS_(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14, ... ) _14

#define ARGU(func, ... ) ARGU_(func, CNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ARGU_( ... ) ARGU__(__VA_ARGS__)
#define ARGU__( func, n, ... ) func##_##n(__VA_ARGS__)

//func = "DECLARE" "TYPE" "NAME"
#define DECLARE_0( ... ) 
#define DECLARE_2(t, n) t n
#define DECLARE_4(t, n, ... ) t n, DECLARE_2(__VA_ARGS__)
#define DECLARE_6(t, n, ... ) t n, DECLARE_4(__VA_ARGS__)
#define DECLARE_8(t, n, ... ) t n, DECLARE_6(__VA_ARGS__)

#define TYPE_0 
#define TYPE_2(t, n) t
#define TYPE_4(t, n, ... ) t , TYPE_2(__VA_ARGS__)
#define TYPE_6(t, n, ... ) t , TYPE_4(__VA_ARGS__)
#define TYPE_8(t, n, ... ) t , TYPE_6(__VA_ARGS__)

#define NAME_0 
#define NAME_2(t, n)  n
#define NAME_4(t, n, ... )  n, NAME_2(__VA_ARGS__)
#define NAME_6(t, n, ... )  n, NAME_4(__VA_ARGS__)
#define NAME_8(t, n, ... )  n, NAME_6(__VA_ARGS__)

#define SET_INTERPOSITIONING(funcName, returnType, ... )						\
		returnType funcName( ARGU(DECLARE, __VA_ARGS__ )){						\
			handleInit() ;														\
			returnType ( *real_##funcName )( ARGU(TYPE, __VA_ARGS__ ) ); 		\
			real_##funcName = dlsym(libcHandle, #funcName ); 					\
			char* error;  														\
			if( (error=dlerror()) != NULL ){									\
				fprintf(stderr, "%s\n", error);									\
				exit(EXIT_FAILURE);												\
			}																	\
																				\
			returnType res = real_##funcName( ARGU(NAME, __VA_ARGS__) );		\
			memset(buffer, '\0', sizeof(buffer));								\
			log_##funcName(res, ARGU(NAME, __VA_ARGS__));						\
			if(sizeof(buffer) != 0){ 											\
				WRITE_LOG();													\
			}																	\
			return res;															\
		}

#define WRITE_LOG()\
			size_t (*real_fwrite)(const void*, size_t, size_t, FILE*);		\
			real_fwrite = dlsym(libcHandle, "fwrite");						\
			real_fwrite(buffer, sizeof(char), sizeof(buffer), outputFile);	\



void *libcHandle;
FILE *outputFile;
struct dirTable *parentTable;
char buffer[128];
char* recover_filename(FILE *F);
char* fd_recover_filename(int fd);

__attribute__((constructor)) static void handleInit(){
	static int initially = 0;
	if(initially==0){
		initially++;

		libcHandle = dlopen("libc.so.6", RTLD_LAZY);
		char* outputFileName = getenv("MONITOR_OUTPUT");
		

		FILE* (*real_fopen)(const char*, const char*);
		real_fopen = dlsym(libcHandle, "fopen");
		outputFile = real_fopen(outputFileName, "w");
		if(outputFile == NULL){
			outputFile = stderr;
		}
	
		initDirTable(&parentTable, 1024);
	}
}

void log_opendir(DIR* res, const char *name){
	if(res != NULL){
		sprintf(buffer, "opendir(\"%s\") = %p\n", name, res);
		dir_push(parentTable, res, name);
	}
}

void log_readdir(struct dirent* res, DIR* dirp){
	if(res != NULL){
		char *parentDirName = dir_find(parentTable, dirp);
		sprintf(buffer, "readdir(\"%s\") = %s\n", parentDirName, res->d_name);
	}
}


void log_closedir(int res, DIR* dirp){
	char* parentDirName = dir_find(parentTable, dirp);
	sprintf(buffer, "closedir(\"%s\") = %d\n", parentDirName, res);
}


void log_creat(int res, const char* pathname, mode_t mode){
	if(res != 0){
		sprintf(buffer, "creat(\"%s\", %3o) = %d\n", pathname, mode&0777, res);
	}
}


void log_chdir(int res, const char* path){
	sprintf(buffer, "chdir(\"%s\") = %d\n", path, res);
}


void log_write(ssize_t res, int fd, const void *buf, size_t count){
	sprintf(buffer, "write(%d, %p, %zu) = %zd\n", fd, buf, count, res);
}


void log_pwrite(ssize_t res, int fd, void *buf, size_t count, off_t offset){
	sprintf(buffer, "pwrite(%d, \"%s\", %zu, %ld) = %zd\n", fd, (char*)buf, count, offset, res);
}

void log_open(int res, const char* pathname, int flags){
	sprintf(buffer, "open(\"%s\", %d) = %d\n", pathname, flags, res);
}

void log_dup(int res, int oldfd){
	sprintf(buffer, "dup(%d) = %d\n", oldfd, res);
}


void log_dup2(int res, int oldfd, int newfd){
	sprintf(buffer, "dup2(%d, %d) = %d\n", oldfd, newfd, res);
}

void log_read(ssize_t res, int fd, void* buf, size_t count){
	sprintf(buffer, "read(%d, %p, %zu) = %zd\n", fd, buf, count, res);
}

void log___xstat(int res, int ver, const char* path, struct stat* stat_buf){
	sprintf(buffer, "stat(\"%s\", %p {mode=%3o size=%ld}) = %d\n", path, stat_buf, (stat_buf->st_mode)&0777, stat_buf->st_size, res);
}

void log___lxstat(int res, int ver, const char* path, struct stat* stat_buf){
	sprintf(buffer, "lstat(\"%s\", %p {mode=%3o size=%ld}) = %d\n", path, stat_buf, (stat_buf->st_mode)&0777, stat_buf->st_size, res);
}

void log_rename(int res, const char* oldpath, const char* newpath){
	sprintf(buffer, "rename(\"%s\", \"%s\") = %d\n", oldpath, newpath, res);
}

void log_chown(int res, const char* pathname, uid_t owner, gid_t group){
	sprintf(buffer, "chown(\"%s\", %u, %u) = %d\n", pathname, owner, group, res);
}

void log_chmod(int res, const char* pathname, mode_t mode){
	sprintf(buffer, "chmod(\"%s\", %3o) = %d\n", pathname, mode&0777, res);
}

void log_link(int res, const char* oldpath, const char* newpath){
	sprintf(buffer, "link(\"%s\", \"%s\") = %d\n", oldpath, newpath, res);
}

void log_unlink(int res, const char* pathname){
	sprintf(buffer, "unlink(\"%s\") = %d\n", pathname, res);
}

void log_symlink(int res, const char* target, const char* linkpath){
	sprintf(buffer, "symlink(\"%s\", \"%s\") = %d\n", target, linkpath, res);
}

void log_readlink(ssize_t res, const char* pathname, char* buf, size_t bufsiz){
	sprintf(buffer, "readlink(\"%s\", \"%s\", %zu) = %zd\n", pathname, buf, bufsiz, res);
}

void log_mkdir(int res, const char* pathname, mode_t mode){
	sprintf(buffer, "mkdir(\"%s\", %3o) = %d\n", pathname, mode&0777, res);
}

void log_rmdir(int res, const char* pathname){
	sprintf(buffer, "rmdir(\"%s\") = %d\n", pathname, res);
}

void log_remove(int res, const char* pathname){
	sprintf(buffer, "remove(\"%s\") = %d\n", pathname, res);
}

void log_fopen(FILE* res, const char* path, const char* mode){
	if(res != NULL){
		sprintf(buffer, "fopen(\"%s\", \"%s\") = %p\n", path, mode, res);
	}
}

void log_fwrite(size_t res, const void* ptr, size_t size, size_t nmemb, FILE* stream){
	char* path = recover_filename(stream);
	sprintf(buffer, "fwrite(%p, %zu, %zu, \"%s\") = %zu\n", ptr, size, nmemb, path, res);
}

void log_fgetc(int res, FILE* stream){
	char* filename = recover_filename(stream);
	sprintf(buffer, "fgetc(\"%s\") = %d\n", filename, res);
}

void log_fgets(char* res, char* s, int size, FILE* stream){
	char* filename = recover_filename(stream);
	sprintf(buffer, "fgets(\"%s\", %d, \"%s\") = %s\n", s, size, filename, res);
}

void log_fread(size_t res, void* ptr, size_t size, size_t nmemb, FILE* stream){
	char* filename = recover_filename(stream);
	sprintf(buffer, "fread(%p, %zu, %zu, \"%s\") = %zu\n", ptr, size, nmemb, filename, res);
}

void log_fputs_unlocked(int res, const char *s, FILE* stream){
	char *filename = recover_filename(stream);
	sprintf(buffer, "fputs_unlocked(%p ,\" %s\") = %d\n", s, filename, res);
}

void log_fwrite_unlocked(size_t res, const void* ptr, size_t size, size_t n, FILE* stream){
	char *filename = recover_filename(stream);
	sprintf(buffer, "fwrite_unlocked(%p, %zu, %zu, \"%s\") = %zu\n", ptr, size, n, filename, res);
}

void log_fflush(int res, FILE* stream){
	char *filename = recover_filename(stream);
	sprintf(buffer, "fflush(\"%s\") = %d\n", filename, res);
}


void log_fflush_unlocked(int res, FILE* stream){
	char *filename = recover_filename(stream);
	sprintf(buffer, "fflush_unlocked(\"%s\") = %d\n", filename, res);
}

SET_INTERPOSITIONING(opendir, DIR*, const char*, name);
SET_INTERPOSITIONING(readdir, struct dirent*, DIR*, dirp);
SET_INTERPOSITIONING(closedir, int, DIR*, dirp);
SET_INTERPOSITIONING(creat, int ,const char*, pathname, mode_t, mode);
SET_INTERPOSITIONING(chdir, int, const char*, path);
SET_INTERPOSITIONING(write, ssize_t, int, fd, const void*, buf, size_t, count);
SET_INTERPOSITIONING(pwrite, ssize_t, int, fd, void*, buf, size_t, count, off_t, offset);
SET_INTERPOSITIONING(open, int, const char*, pathname, int, flags);
SET_INTERPOSITIONING(dup, int, int, oldfd);
SET_INTERPOSITIONING(dup2, int, int, oldfd, int, newfd);
SET_INTERPOSITIONING(read, ssize_t, int, fd, void*, buf, size_t, count);
SET_INTERPOSITIONING(__xstat, int, int, ver, const char*, path, struct stat*, stat_buf);
SET_INTERPOSITIONING(__lxstat, int, int, ver, const char*, path, struct stat*, stat_buf);
SET_INTERPOSITIONING(rename, int, const char*, oldpath, const char*, newpath);
SET_INTERPOSITIONING(chown, int, const char*, pathname, uid_t, owner, gid_t, group);
SET_INTERPOSITIONING(chmod, int, const char*, pathname, mode_t, mode);
SET_INTERPOSITIONING(link, int, const char*, oldpath, const char*, newpath);
SET_INTERPOSITIONING(unlink, int, const char*, pathname);
SET_INTERPOSITIONING(symlink, int, const char*, target, const char*, linkpath);
SET_INTERPOSITIONING(readlink, ssize_t, const char*, pathname, char*, buf, size_t, bufsiz);
SET_INTERPOSITIONING(mkdir, int, const char*, pathname, mode_t, mode);
SET_INTERPOSITIONING(rmdir, int, const char*, pathname);
SET_INTERPOSITIONING(remove, int, const char*, pathname);
SET_INTERPOSITIONING(fopen, FILE*, const char*, path, const char*, mode);
SET_INTERPOSITIONING(fwrite, size_t, const void*, ptr, size_t, size, size_t, nmemb, FILE*, stream);
SET_INTERPOSITIONING(fgetc, int, FILE*, stream);
SET_INTERPOSITIONING(fgets, char*, char* , s, int, size, FILE*, stream);
SET_INTERPOSITIONING(fread, size_t, void*, ptr, size_t, size, size_t, nmemb, FILE*, stream);
SET_INTERPOSITIONING(fputs_unlocked, int, const char*, s, FILE*, stream);
SET_INTERPOSITIONING(fwrite_unlocked, size_t, const void*, ptr, size_t, size, size_t, n, FILE*, stream);
SET_INTERPOSITIONING(fflush, int, FILE*, stream);
SET_INTERPOSITIONING(fflush_unlocked, int, FILE*, stream);


int fprintf(FILE* stream, const char* format, ... ){
	handleInit() ;
	va_list arg;
	int done;

	va_start (arg, format);
	done = vfprintf(stream, format, arg);
	va_end (arg);
	
	char* filename = recover_filename(stream);
	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, "fprintf(\"%s\", \"%s\", ...) = %d\n", filename, format, done);
	WRITE_LOG();

	return done;
}

int __isoc99_fscanf(FILE *stream, const char* format, ...){
	handleInit() ;
	va_list arg;
	int done;

	va_start (arg, format);
	done = vfscanf(stream, format, arg);
	va_end (arg);

	char* filename = recover_filename(stream);
	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, "fscanf(\"%s\", \"%s\", ...) = %d\n", filename, format, done);
	WRITE_LOG();

	return done;
}

int close(int fd){
	handleInit() ;
	int (*real_close)(int);
	real_close = dlsym(libcHandle, "close");
	char *error;
	if((error = dlerror()) != NULL){
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	char* openedFileName = fd_recover_filename(fd);
	int res = real_close(fd);
	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, "close(\"%s\") = %d\n", openedFileName, res);
	WRITE_LOG();

	return res;
}


int fclose(FILE *stream){
	handleInit() ;
	int (*real_fclose)(FILE*);
	real_fclose = dlsym(libcHandle, "fclose");
	char *error;
	if((error = dlerror()) != NULL){
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	char* filename = recover_filename(stream);
	int res = real_fclose(stream);
	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, "fclose(\"%s\") = %d\n", filename, res);
	WRITE_LOG();

	return res;

}

//=========================================
char* fd_recover_filename(int fd){
	char fd_path[255];
	ssize_t n;
	char *filename = (char*)malloc(sizeof(char) * 255);
	memset(filename, '\0', 255);

	if(fd == 0) return "<STDIN>";
	else if(fd == 1) return "<STDOUT>";
	else if(fd == 2) return "<STDERR>";

	sprintf(fd_path, "/proc/self/fd/%d", fd);
	ssize_t (*real_readlink)(const char*, char*, size_t);
	real_readlink = dlsym(libcHandle, "readlink");
	n = real_readlink(fd_path, filename, 255);
	if(n<0) return filename;
	filename[n] = '\0';

	return basename(filename);

}

char* recover_filename(FILE *f){
	int fd;

	fd = fileno(f);
	return fd_recover_filename(fd);
}



/*
	char *filename = recover_filename(stream);
DIR* opendir(const char *name){
	DIR* (*real_opendir)(const char*);
	real_opendir = dlsym(libcHandle, "opendir");
	char *error;
	if((error = dlerror()) != NULL){
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	DIR* res;
	char buffer[128] = {'\0'};
	res = real_opendir(name);
	if(res != NULL){
		sprintf(buffer, "opendir(\"%s\") = %p\n", name, res);
		fwrite(buffer, sizeof(char), sizeof(buffer), outputFile);
		push(parentTable, res, name);
	}

	return res;
}
*/
