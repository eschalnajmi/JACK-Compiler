/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/

#include "compiler.h"


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
		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", dir_name, enter2->d_name);

		InitParser(full_path);
		globalParse();
	}

	DIR *directory = opendir(dir_name);

	struct dirent *enter;

	InitSymbolTable();
	while((enter = readdir(directory)) != NULL){
		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", dir_name, enter->d_name);

		InitParser(full_path);
		p = Parse();
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
	ParserInfo p = compile ("Square3");
	
	PrintError (p);
	StopCompiler ();
	return 1;
}
#endif
