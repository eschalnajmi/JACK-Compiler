#include "compiler.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int InitCompiler ()
{
	return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;

	DIR *directory2 = opendir(dir_name);

	struct dirent *enter2;

	initglobalST();
	while((enter2 = readdir(directory2)) != NULL){

		if (strcmp(enter2->d_name , ".") == 0 || strcmp(enter2->d_name ,"..") == 0 || 5 >= strlen(enter2->d_name) || strncmp(enter2->d_name + strlen(enter2->d_name) - 5, ".jack", 5) != 0) continue;

		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", dir_name, enter2->d_name);

		int filesize = strlen(enter2->d_name) + 3;
        char* filepath = malloc(filesize);
        strcpy(filepath, "./");
        strcat(filepath, enter2->d_name);

		InitParser(full_path);
		globalParse();
		StopParser();
		free(filepath);
	}

	DIR *directory = opendir(dir_name);

	struct dirent *enter;

	InitSymbolTable();
	while((enter = readdir(directory)) != NULL){

		

		if (strcmp(enter->d_name , ".") == 0 || strcmp(enter->d_name ,"..") == 0 || 5 >= strlen(enter->d_name) || strncmp(enter->d_name + strlen(enter->d_name) - 5, ".jack", 5) != 0) continue;


		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", dir_name, enter->d_name);

		int fsize = strlen(dir_name) + strlen(enter->d_name) + 2;
        char* fpath = malloc(fsize);
        strcpy(fpath, "/");
        strcat(fpath, enter->d_name);


		InitParser(full_path);
		p = Parse();
		StopParser();
		free(fpath);


		if(p.er != none){
			break;
		}
	}
	
	StopSymbolTable();
	return p;
}

int StopCompiler ()
{


	return 1;
}


int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("Square2");
	
	PrintError (p);
	StopCompiler ();
	return 1;
}

