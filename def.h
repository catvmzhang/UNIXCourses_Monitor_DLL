#ifndef _def_h_
#define _def_h_

#include <dirent.h>
#include <sys/types.h>


struct dirTable{
	DIR **dir;
	char **name;
	int index;
};

void initDirTable(struct dirTable **self, int num);
void dir_push(struct dirTable *table, DIR* dir, const char* name);
char* dir_find(struct dirTable *table, DIR* dir);


#endif
