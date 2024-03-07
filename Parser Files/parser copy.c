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



// integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList ) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand(){
	ParserInfo pi;
	Token token = GetNextToken();
	if(!(strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0 || token.tp == STRING || token.tp == INT || token.tp == ID)){
		pi.er = syntaxError;
		pi.tk = token;
	}
	// add .identifier, expression and expessionlist

	return pi;
}

//  ( - | ~ | ε ) operand
ParserInfo factor(){
	ParserInfo pi;

	Token token = PeekNextToken();

	if ((strcmp(token.lx, "-") == 0 || strcmp(token.lx, "~")) == 0)
	{
		GetNextToken();
	}

	return pi;
}

// factor { ( * | / ) factor }
ParserInfo term(){

}

// term { ( + | - ) term }
ParserInfo ArithmeticExpression(){

}

// ArithmeticExpression { ( = | > | < ) ArithmeticExpression }
ParserInfo relationalExpression(){

}

// relationalExpression { ( & | | ) relationalExpression }
ParserInfo expression(){

}

// return [ expression ] ;
ParserInfo returnStatement(){

}

// expression { , expression } | ε
ParserInfo expressionList(){

}

// identifier [ . identifier ] ( expressionList )
ParserInfo subroutineCall(){

}

// do subroutineCall ;
ParserInfo doStatement(){

}

// while ( expression ) { {statement} }
ParserInfo whileStatement(){

}

//  if ( expression ) { {statement} } [ else { {statement} } ]
ParserInfo ifStatement(){

}

// let identifier [ [ expression ] ] = expression ;
ParserInfo letStatement(){

}

// var type identifier { , identifier } ;
ParserInfo varDeclarStatement(){

}

// varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatement
ParserInfo statement(){

}

//  { {statement} } 
ParserInfo subroutineBody(){

}

// type identifier {, type identifier} | ε
ParserInfo paramList(){

}

// (constructor | function | method) (type|void) identifier (paramList) subroutineBody
ParserInfo subroutineDeclar(){

}

// int | char | boolean | identifier
ParserInfo type(){

}

//  (static | field) type identifier {, identifier} ;
ParserInfo classVarDeclar(){

}

// classVarDeclar | subroutineDeclar
ParserInfo memberDeclar(){

}

// class identifier { {memberDeclar} }
ParserInfo class(){

}

ParserInfo checkBrackets(int index, ParserInfo pi){
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
	
	StopLexer();

	return 1;
}

ParserInfo Parse ()
{
	Token token = PeekNextToken();
	InitParser(token.fl);

	ParserInfo pi;

	// Check for missing brackets
	for(int i = 1; i < numtokens; i++){
		if(alltokens[i].tp == SYMBOL){
			pi = checkBrackets(i, pi);
			if(pi.er != none){
				break;
			}
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
