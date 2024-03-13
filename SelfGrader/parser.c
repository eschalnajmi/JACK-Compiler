#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below
ParserInfo operand();
ParserInfo factor();
ParserInfo term();
ParserInfo ArithmeticExpression();
ParserInfo relationalExpression();
ParserInfo expression();
ParserInfo returnStatement();
ParserInfo expressionList();
ParserInfo subroutineCall();
ParserInfo doStatement();
ParserInfo whileStatement();
ParserInfo ifStatement();
ParserInfo elseStatement();
ParserInfo letStatement();
ParserInfo varDeclarStatement();
ParserInfo statement();
ParserInfo subroutineBody();
ParserInfo paramList();
ParserInfo subroutineDeclar();
ParserInfo type();
ParserInfo classVarDeclar();
ParserInfo memberDeclar();
ParserInfo class();
int isVarIdentifier(char* inputID);
int isFunctionIdentifier(char* inputID);

int numtokens;
int currentindex = 0;
/*char* varIdentifiers[500];
int varIDIndex = 0;
char* functionIdentifiers[500];
int functionIDIndex = 0;*/

// [] 0 or 1
// {} 0 or many
// integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList ) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand(){
	ParserInfo pi;
	pi.er = none;
	int synterror = 0;

	Token token =  GetNextToken();

	if(strcmp(token.lx, "[") == 0){
		pi = expression();
		if(pi.er != none){
			return pi;
		}

		token = GetNextToken();
		
		if(!(strcmp(token.lx, "]") == 0)){
			pi.er = closeBracketExpected;
			pi.tk = token;
			return pi;
		}
	}

	if(strcmp(token.lx, "(") == 0){
		if(strcmp(PeekNextToken().lx,",") == 0){
			pi = expressionList();
		} else{
			pi = expression();
		}

		if(pi.er != none){
			return pi;
		}

		token = GetNextToken();
	}

	if(!(strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0 || token.tp == STRING || token.tp == INT || token.tp == ID)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	if(token.tp == ID){
		/*if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}*/

		if(strcmp(PeekNextToken().lx, ".") == 0){
			token = GetNextToken();
			token = GetNextToken();

			if(token.tp != ID){
				pi.er = idExpected;
				pi.tk = token;
				return pi;
			}

			/*if(isFunctionIdentifier(token.lx) == 0){
				pi.er = undecIdentifier;
				pi.tk = token;
				return pi;
			}*/

			token = GetNextToken();
		}
	}

	return pi;
}

//  ( - | ~ | ε ) operand
ParserInfo factor(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if ((strcmp(token.lx, "-") == 0 || strcmp(token.lx, "~")) == 0 || strcmp(token.lx, "") == 0)
	{
		pi = operand();
		
	} else {
		pi.er = syntaxError;
		pi.tk = token;
	}

	return pi;
}

// factor { ( * | / ) factor }
ParserInfo term(){
	ParserInfo pi = factor();
	
	if(pi.er != none){
		return pi;
	}

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "*") == 0 || strcmp(token.lx, "/") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	pi = factor();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(PeekNextToken().lx, "*") == 0 || strcmp(PeekNextToken().lx, "/") == 0)){
		token = GetNextToken();
		pi = factor();
		if(pi.er != none){
			return pi;
		}
	}

	return pi;
}

// term { ( + | - ) term }
ParserInfo ArithmeticExpression(){
	ParserInfo pi = term();
	if(pi.er != none){
		return pi;
	}

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "+") == 0|| strcmp(token.lx, "-") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	pi = term();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(PeekNextToken().lx, "+") == 0 || strcmp(PeekNextToken().lx, "-") == 0)){
		token = GetNextToken();
		pi = term();
		if(pi.er != none){
			return pi;
		}
	}

	return pi;
}

// ArithmeticExpression { ( = | > | < ) ArithmeticExpression }
ParserInfo relationalExpression(){
	ParserInfo pi = ArithmeticExpression();
	if(pi.er != none){
		return pi;
	}

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "=") == 0 || strcmp(token.lx, ">") == 0 || strcmp(token.lx, "<") == 0 )){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	pi = ArithmeticExpression();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(PeekNextToken().lx, "=") == 0 || strcmp(PeekNextToken().lx, ">") == 0 || strcmp(PeekNextToken().lx, "<") == 0)){
		token = GetNextToken();
		pi = ArithmeticExpression();
		if(pi.er != none){
			return pi;
		}
	}

	return pi;
}

// relationalExpression { ( & | | ) relationalExpression }
ParserInfo expression(){
	ParserInfo pi = relationalExpression();
	if(pi.er != none){
		return pi;
	}

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "&") == 0 || strcmp(token.lx, "|") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	} 

	pi = relationalExpression();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(PeekNextToken().lx, "&") == 0 || strcmp(PeekNextToken().lx, "|") == 0)){
		token = GetNextToken();
		pi = relationalExpression();
		if(pi.er != none){
			return pi;
		}
	}

	return pi;
}

// return [ expression ] ;
ParserInfo returnStatement(){
	ParserInfo pi;
	pi.er = none;

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	Token token = GetNextToken();

	if(strcmp(token.lx, ";") == 0){
		return pi;
	}

	pi.er = semicolonExpected;
	pi.tk = token;
	return pi;
}

// expression { , expression } | ε
ParserInfo expressionList(){
	ParserInfo pi;
	pi.er = none;

	if(strcmp(PeekNextToken().lx, "") == 0){
		return pi;
	} else{
		pi = expression();
		while(strcmp(PeekNextToken().lx, ",") == 0){
			GetNextToken();

			pi = expression();
			if(pi.er != none){
				return pi;
			}
		}
	}
	return pi;
}

// identifier [ . identifier ] ( expressionList )
ParserInfo subroutineCall(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	/*if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
		pi.er = undecIdentifier;
		pi.tk = token;
		return pi;
	}*/

	token = GetNextToken();

	if(strcmp(token.lx, ".") == 0){
		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		/*if(isFunctionIdentifier(token.lx) == 0){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}*/

		token = GetNextToken();
	}

	if(!(strcmp(token.lx, "("))){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = expressionList();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, ")"))){
		pi.er = closeParenExpected;
		pi.tk = token;
		return pi;
	}

	return pi;
}

// do subroutineCall ;
ParserInfo doStatement(){
	ParserInfo pi;
	pi.er = none;
	
	pi = subroutineCall();
	if(pi.er != none){
		return pi;
	}


	Token token = GetNextToken();

	if(!strcmp(token.lx, ";")){
		pi.er = semicolonExpected;
		pi.tk = token;
	}

	return pi;
}

// while ( expression ) { {statement} }
ParserInfo whileStatement(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(!strcmp(token.lx, "(")){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(!strcmp(token.lx, ")")){
		pi.er = closeParenExpected;
		pi.tk = token;
		return pi;
	}

	token = GetNextToken();

	if(!strcmp(token.lx, "{")){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!strcmp(token.lx, "}")){
		GetNextToken();
		pi = statement();
		if(pi.er != none){
			return pi;
		}	
		token = PeekNextToken();
	}

	return pi;
}

//  if ( expression ) { {statement} } [ else { {statement} } ]
ParserInfo ifStatement(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "(") == 0)){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, ")") == 0)){
		pi.er = closeParenExpected;
		pi.tk = token;
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(token.lx, "}") == 0)){
		pi = statement();
		if(pi.er != none){
			pi.er = closeBraceExpected;
			pi.tk = token;
			return pi;
		}	
		token = PeekNextToken();
	}
	GetNextToken();
	token = GetNextToken();
	currentindex++;
	if(strcmp(token.lx, "else")== 0){
		pi = elseStatement();
		if(pi.er != none){
			return pi;
		}	
	}

	return pi;
}

ParserInfo elseStatement(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();
	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(token.lx, "}") == 0)){
		pi = statement();
		if(pi.er != none){
			pi.er = closeBraceExpected;
			pi.tk = token;
			return pi;
		}	
		token = PeekNextToken();
	}
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	return pi;
}

/*int isVarIdentifier(char* inputID){
	for(int i = 0; i <= varIDIndex; i++){
		if(strcmp(inputID, varIdentifiers[i]) == 0){
			return 1;
		}
	}
	return 0;
}

int isFunctionIdentifier(char* inputID){
	for(int i = 0; i <= functionIDIndex; i++){
		if(strcmp(inputID, functionIdentifiers[i]) == 0){
			return 1;
		}
	}
	return 0;
}*/

// let identifier [ [ expression ] ] = expression ;
ParserInfo letStatement(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	/*if(isVarIdentifier(token.lx) == 0){
		pi.er = undecIdentifier;
		pi.tk = token;
		return pi;
	}*/

	token = GetNextToken();

	if(strcmp(token.lx, "[") == 0){
		pi = expression();
		if(pi.er != none){
			return pi;
		}
	}

	token = GetNextToken();

	if(!(strcmp(token.lx,"]") == 0)){
		pi.er = closeBracketExpected;
		pi.tk = token;
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx,"=") == 0)){
		pi.er = equalExpected;
		pi.tk = token;
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx,";") == 0)){
		pi.er = semicolonExpected;
		pi.tk = token;
		return pi;
	}

	return pi;
}

// var type identifier { , identifier } ;
ParserInfo varDeclarStatement(){
	ParserInfo pi;
	pi.er = none;

	pi = type();
	if(pi.er != none){
		return pi;
	}

	Token token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	/*varIdentifiers[varIDIndex] = (char*)malloc(sizeof(token.lx));
	strcpy(varIdentifiers[varIDIndex], token.lx);
	varIDIndex++;*/

	token = GetNextToken();

	while(strcmp(token.lx, ",") == 0){
		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		/*varIdentifiers[varIDIndex] = (char*)malloc(sizeof(token.lx));
		strcpy(varIdentifiers[varIDIndex], token.lx);
		varIDIndex++;*/

		token = GetNextToken();
	}

	if(!(strcmp(token.lx, ";") == 0)){
		pi.er = semicolonExpected;
		pi.tk = token;
		return pi;
	}

	return pi;
}

// varDeclarStatement | letStatemnt | ifStatement | whileStatement | doStatement | returnStatement
ParserInfo statement(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(strcmp(token.lx, "var") == 0){
		pi = varDeclarStatement();
	} else if(strcmp(token.lx, "if") == 0){
		pi = ifStatement();
	} else if(strcmp(token.lx, "while") == 0){
		pi = whileStatement();
	} else if(strcmp(token.lx, "let") == 0){
		pi = letStatement();
	} else if(strcmp(token.lx, "do") == 0){
		pi = doStatement();
	} else if(strcmp(token.lx, "return") == 0){
		pi = returnStatement();
	} else {
		pi.er = syntaxError;
		pi.tk = token;
	}
	
	return pi;
}

//  { {statement} } 
ParserInfo subroutineBody(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "{"))){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(PeekNextToken().lx, "}"))){
		GetNextToken();
		pi = statement();
		if(pi.er != none){
			pi.er = closeBraceExpected;
			pi.tk = token;
			return pi;
		}
	}

	GetNextToken();

	return pi;
}

// type identifier {, type identifier} | ε
ParserInfo paramList(){
	ParserInfo pi;
	pi.er = none;

	Token token = PeekNextToken();
	if(strcmp(token.lx, "") == 0){
		return pi;
	}

	pi = type();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	if(strcmp(PeekNextToken().lx, ",")){
		GetNextToken();
		pi = type();
		if(pi.er != none){
			return pi;
		}

		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}
	}
	return pi;
}

// (constructor | function | method) (type|void) identifier (paramList) subroutineBody
ParserInfo subroutineDeclar(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0)){
		pi.er = subroutineDeclarErr;
		pi.tk = token;
		return pi;
	}

	token = PeekNextToken();
	if(!(strcmp(token.lx, "void") == 0)){
		pi = type();
		if(pi.er != none){
			return pi;
		}
	}

	token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	} else {
		/*if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}
		functionIdentifiers[functionIDIndex] = (char*)malloc(sizeof(token.lx));
		strcpy(functionIdentifiers[functionIDIndex], token.lx);
		functionIDIndex++;*/
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, "(") == 0)){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = paramList();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, ")") == 0)){
		pi.er = closeParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = subroutineBody();
	if(pi.er != none){
		return pi;
	}

	return pi;
	
}

// int | char | boolean | identifier
ParserInfo type(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();
	
	if(token.tp == ID){
		/*if(isVarIdentifier(token.lx) == 0 && isFunctionIdentifier(token.lx) == 0){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}*/
	
	}

	if(!(strcmp(token.lx, "int") == 0 || strcmp(token.lx, "char") == 0 || strcmp(token.lx, "boolean") == 0 || token.tp == ID)){
		pi.er = illegalType;
		pi.tk = token;
	}

	return pi;
}

//  (static | field) type identifier {, identifier} ;
ParserInfo classVarDeclar(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0)){
		pi.er = classVarErr;
		pi.tk = token;
		return pi;
	}

	pi = type();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	/*if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	varIdentifiers[varIDIndex] = (char*)malloc(sizeof(token.lx));
	strcpy(varIdentifiers[varIDIndex], token.lx);
	varIDIndex++;*/

	while(strcmp(PeekNextToken().lx, ",")){
		GetNextToken();
		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		/*if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}

		varIdentifiers[varIDIndex] = (char*)malloc(sizeof(token.lx));
		strcpy(varIdentifiers[varIDIndex], token.lx);
		varIDIndex++;*/
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, ";") == 0)){
		pi.er = semicolonExpected;
		pi.tk = token;
		return pi;
	}

	return pi;

}

// classVarDeclar | subroutineDeclar
ParserInfo memberDeclar(){
	ParserInfo pi;
	pi.er = none;

	Token token = PeekNextToken();

	if(strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0){
		pi = classVarDeclar();
	} else if(strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0){
		pi = subroutineDeclar();
	} else {
		pi.er = memberDeclarErr;
		pi.tk = token;
	}

	return pi;
}

// class identifier { {memberDeclar} }
ParserInfo class(){
	ParserInfo pi;
	pi.er = none;

	Token token = GetNextToken();

	if(!(strcmp(token.lx, "class") == 0)){
		pi.er = classExpected;
		pi.tk = token;
		return pi;
	}

	token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}
	
	/*functionIdentifiers[functionIDIndex] = (char*)malloc(sizeof(token.lx));
	strcpy(functionIdentifiers[functionIDIndex], token.lx);
	functionIDIndex++;*/

	token = GetNextToken();

	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	pi = memberDeclar();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();
	if(!(strcmp(token.lx, "}"))){
		pi.er = closeBraceExpected;
		pi.tk = token;
		return pi;
	}

	return pi;

}

int InitParser (char* file_name)
{
	InitLexer(file_name);

	return 1;
}

ParserInfo Parse ()
{
	ParserInfo pi;

	pi = class();

	return pi;
}


int StopParser ()
{
	//free(varIdentifiers);
	//free(functionIdentifiers);

	return 1;
}

/*#ifndef TEST_PARSER
int main ()
{
	InitParser("closeParenExpected.jack");
	Parse();
	return 1;
}
#endif*/
