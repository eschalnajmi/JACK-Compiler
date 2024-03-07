#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below
Token* alltokens;
int numtokens;
int currentindex = 0;
char identifier[128];

int InitParser (char* file_name)
{
	
	return 1;
}

ParserInfo expression(){
	ParserInfo pi;
	Token token = GetNextToken();
	if(strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0 || token.tp |= STRING){

	}
	return pi;
}

ParserInfo Parse ()
{
	ParserInfo pi;

	return pi;
}


int StopParser ()
{
	free(alltokens);
	return 1;
}

#ifndef TEST_PARSER
int main ()
{
	InitLexer("closeParenExpected.jack");
	Parse();
	return 1;
}
#endif
