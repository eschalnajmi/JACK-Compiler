#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below
Token* alltokens;
int numtokens;
int currentindex = 0;

/*char* TokenTypeString (TokenType t)
{
	switch (t)
	{
		case RESWORD: return "RESWORD";
		case ID: return "ID";
		case INT: return "INT";
		case SYMBOL: return "SYMBOL";
		case STRING: return "STRING";
		case EOFile: return "EOFile";
		case ERR: return "ERR";
		default: return "Not a recognised token type";
	}

}

void PrintTokens (Token t)
{
	printf ("<%s, %i, %s, %s>\n", t.fl, t.ln , t.lx, TokenTypeString (t.tp));
}*/

ParserInfo CheckSymbols(int index, ParserInfo pi){
	int skip = 0;

	if(strcmp(alltokens[index].lx, "(") == 0){
		for(int j = index + 1; j < numtokens; j++){
			if(strcmp(alltokens[j].lx, ")") == 0 && skip == 0){
				return pi;
			} else if(strcmp(alltokens[j].lx, ")") == 0){
				skip--;
			} else if(strcmp(alltokens[j].lx, "(") == 0){
				skip++;
			}
		}
		pi.tk = alltokens[index + 1];
		pi.er = closeParenExpected;
	} else if(strcmp(alltokens[index].lx, ")") == 0){
		for(int j = index-1; j > 0; j--){
			if(strcmp(alltokens[j].lx, "(") == 0 && skip == 0){
				return pi;
			} else if(strcmp(alltokens[j].lx, "(") == 0){
				skip--;
			} else if(strcmp(alltokens[j].lx, ")") == 0){
				skip++;
			}
		}
		pi.tk = alltokens[index - 1];
		pi.er = openParenExpected;
	} else if(strcmp(alltokens[index].lx, "{") == 0){
		for(int j = index + 1; j < numtokens; j++){
			if(strcmp(alltokens[j].lx, "}") == 0 && skip == 0){
				return pi;
			} else if(strcmp(alltokens[j].lx, "}") == 0){
				skip--;
			} else if(strcmp(alltokens[j].lx, "{") == 0){
				skip++;
			}
		}
		pi.tk = alltokens[index + 1];
		pi.er = closeBraceExpected;
	} else if(strcmp(alltokens[index].lx, "}") == 0){
		for(int j = index-1; j > 0; j--){
			if(strcmp(alltokens[j].lx, "{") == 0 && skip == 0){
				return pi;
			} else if(strcmp(alltokens[j].lx, "{") == 0){
				skip--;
			} else if(strcmp(alltokens[j].lx, "}") == 0){
				skip++;
			}
		}
		pi.tk = alltokens[index - 1];
		pi.er = openBraceExpected;
	} else if(strcmp(alltokens[index].lx, "[") == 0){
		for(int j = index + 1; j < numtokens; j++){
			if(strcmp(alltokens[j].lx, "]") == 0 && skip == 0){
				return pi;
			} else if(strcmp(alltokens[j].lx, "]") == 0){
				skip--;
			} else if(strcmp(alltokens[j].lx, "[") == 0){
				skip++;
			}
		}
		pi.tk = alltokens[index + 2];
		pi.er = closeBracketExpected;
	}
	return pi;
}

int InitParser (char* file_name)
{
	InitLexer(file_name);
	numtokens = 1;

    // get the number of tokens
	Token token = GetNextToken();
    while(token.tp != EOFile && token.tp != ERR){
        numtokens++;
		token = GetNextToken();
    }

	StopLexer();

	alltokens = malloc((numtokens+1) * sizeof(Token));

	InitLexer(file_name);

	for(int i = 0; i < numtokens; i++){
		alltokens[i] = GetNextToken();
	}

	return 1;
}

ParserInfo Parse ()
{
	Token token = PeekNextToken();
	InitParser(token.fl);

	ParserInfo pi;

	if(strcmp(alltokens[0].lx, "class") != 0){
		pi.er = classExpected;
		pi.tk = alltokens[0];
		return pi;
	}
	
	for(int i = 1; i < numtokens; i++){
		if(alltokens[i].tp == SYMBOL){
			pi = CheckSymbols(i, pi);
			if(pi.er != none){
				break;
			}
		} else {
			pi.er = none;
		}

	}
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
