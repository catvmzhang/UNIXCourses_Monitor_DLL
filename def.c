#include "def.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

void dir_push(struct dirTable *table, DIR* dir, const char* name){
	int id = table->index;
	(table->dir)[id] = (DIR*)malloc(sizeof(DIR*));
	(table->name)[id] = (char*)malloc(sizeof(char) * sizeof(name));

	(table->dir)[id] = dir;
	strcpy( (table->name)[id], name);
	table->index++;
}

char* dir_find(struct dirTable *table, DIR* dir){
	int len = table->index;

	for(int i=0;i<len;i++){
		if( (table->dir)[i] == dir){
			return (table->name)[i];
		}
	}
}

void initDirTable(struct dirTable **self, int num){
	(*self) = (struct dirTable*)malloc(sizeof(struct dirTable));
	(*self)->index = 0;
	(*self)->dir = (DIR**)malloc(sizeof(DIR*) * num);
	(*self)->name = (char**)malloc(sizeof(char*) * num);

}


