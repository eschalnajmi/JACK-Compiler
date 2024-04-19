/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Eschal Najmi
Student ID: 201602938
Email: sc22en@leeds.ac.uk
Date Work Commenced: 19/03/2024
*************************************************************************/


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


#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("Square2");
	
	PrintError (p);
	StopCompiler ();
	return 1;
}
#endif
