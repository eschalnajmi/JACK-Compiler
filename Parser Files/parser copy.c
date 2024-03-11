#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below




Token* alltokens;
int numtokens;
int currentindex = 0;
char* varIdentifiers[500];
int varIDIndex = 0;
char* functionIdentifiers[500];
int functionIDIndex = 0;

// [] 0 or 1
// {} 0 or many
// integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList ) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand(){
	ParserInfo pi;
	pi.er = none;
	int synterror = 0;

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(strcmp(token.lx, "[") == 0){
		pi = expression();
		if(pi.er != none){
			return pi;
		}

		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;
		
		if(!(strcmp(token.lx, "]") == 0)){
			pi.er = closeBracketExpected;
			pi.tk = token;
			return pi;
		}
	}

	if(strcmp(token.lx, "(") == 0){
		if(strcmp(alltokens[currentindex + 1].lx,",") == 0){
			pi = expressionList();
		} else{
			pi = expression();
		}

		if(pi.er != none){
			return pi;
		}

		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;
	}

	if(!(strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0 || token.tp == STRING || token.tp == INT || token.tp == ID)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}
	if(token.tp == ID){
		if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}
		if(strcmp(token.lx, ".") == 0){
			token = alltokens[currentindex];
			if(currentindex >= numtokens){
				return pi;
			}
			currentindex++;

			if(token.tp != ID){
				pi.er = idExpected;
				pi.tk = token;
				return pi;
			}

			if(isFunctionIdentifier(token.lx) == 0){
				pi.er = undecIdentifier;
				pi.tk = token;
				return pi;
			}

			token = alltokens[currentindex];
			if(currentindex >= numtokens){
				return pi;
			}
			currentindex++;
		}
	}
	// add expression and expessionlist

	return pi;
}

//  ( - | ~ | ε ) operand
ParserInfo factor(){
	ParserInfo pi;
	pi.er = none;

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if ((strcmp(token.lx, "-") == 0 || strcmp(token.lx, "~")) == 0 || strcmp(token.lx, "") == 0)
	{
		operand();
		
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "*") == 0 || strcmp(token.lx, "/") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	pi = factor();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(token.lx, "*") == 0 || strcmp(token.lx, "/") == 0)){
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "+") == 0|| strcmp(token.lx, "-") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	pi = term();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(token.lx, "+") == 0 || strcmp(token.lx, "-") == 0)){
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "=") == 0 || strcmp(token.lx, ">") == 0 || strcmp(token.lx, "<") == 0 )){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	pi = ArithmeticExpression();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(token.lx, "=") == 0 || strcmp(token.lx, ">") == 0 || strcmp(token.lx, "<") == 0)){
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "&") == 0 || strcmp(token.lx, "|") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	} 

	pi = relationalExpression();
	if(pi.er != none){
		return pi;
	}

	while((strcmp(token.lx, "&") == 0 || strcmp(token.lx, "|") == 0)){
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

	if(alltokens[currentindex].ln == alltokens[currentindex - 1].ln){
		pi = expression();
		if(pi.er != none){
			return pi;
		}
	}

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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

	if(strcmp(alltokens[currentindex].lx, "") == 0){
		return pi;
	} else{
		pi = expression();
		while(strcmp(alltokens[currentindex].lx, ",") == 0){
			if(currentindex >= numtokens){
				return pi;
			}
			currentindex++;

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
		pi.er = undecIdentifier;
		pi.tk = token;
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(strcmp(token.lx, ".") == 0){
		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		if(isFunctionIdentifier(token.lx) == 0){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}

		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;
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

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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


	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!strcmp(token.lx, "(")){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!strcmp(token.lx, ")")){
		pi.er = closeParenExpected;
		pi.tk = token;
		return pi;
	}

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!strcmp(token.lx, "{")){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!strcmp(token.lx, "}")){
		pi = statement();
		if(pi.er != none){
			return pi;
		}	
		token = alltokens[currentindex];
	}

	return pi;
}

//  if ( expression ) { {statement} } [ else { {statement} } ]
ParserInfo ifStatement(){
	ParserInfo pi;
	pi.er = none;

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "(") == 0)){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, ")") == 0)){
		pi.er = closeParenExpected;
		pi.tk = token;
		return pi;
	}

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(token.lx, "}") == 0)){
		pi = statement();
		if(pi.er != none){
			return pi;
		}	
		token = alltokens[currentindex];
	}
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(token.lx, "}") == 0)){
		pi = statement();
		if(pi.er != none){
			return pi;
		}	
		token = alltokens[currentindex];
	}
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	return pi;
}

int isVarIdentifier(char* inputID){
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
}

// let identifier [ [ expression ] ] = expression ;
ParserInfo letStatement(){
	ParserInfo pi;
	pi.er = none;

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	if(isVarIdentifier(token.lx) == 0){
		pi.er = undecIdentifier;
		pi.tk = token;
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(strcmp(token.lx, "[") == 0){
		pi = expression();
		if(pi.er != none){
			return pi;
		}
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx,"]") == 0)){
		pi.er = closeBracketExpected;
		pi.tk = token;
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx,"=") == 0)){
		pi.er = equalExpected;
		pi.tk = token;
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	strcpy(varIdentifiers[varIDIndex], token.lx);
	varIDIndex++;

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	while(strcmp(token.lx, ",") == 0){
		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		strcpy(varIdentifiers[varIDIndex], token.lx);
		varIDIndex++;

		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "{"))){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(alltokens[currentindex].lx, "}"))){
		pi = statement();
		if(pi.er != none){
			return pi;
		}
	}

	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	return pi;
}

// type identifier {, type identifier} | ε
ParserInfo paramList(){
	ParserInfo pi;
	pi.er = none;

	Token token = alltokens[currentindex];
	if(strcmp(token.lx, "") == 0){
		return pi;
	}

	pi = type();
	if(pi.er != none){
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	if(strcmp(alltokens[currentindex].lx, ",")){
		currentindex++;
		pi = type();
		if(pi.er != none){
			return pi;
		}

		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0)){
		pi.er = subroutineDeclarErr;
		pi.tk = token;
		return pi;
	}

	Token token = alltokens[currentindex];
	if(!(strcmp(token.lx, "void") == 0)){
		pi = type();
		if(pi.er != none){
			return pi;
		}
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	} else {
		if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}
		strcpy(functionIdentifiers[functionIDIndex], token.lx);
		functionIDIndex++;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "(") == 0)){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	pi = paramList();

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;
	
	if(token.tp == ID){
		if(isVarIdentifier(token.lx) == 0 && isFunctionIdentifier(token.lx) == 0){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}
	
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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0)){
		pi.er = classVarErr;
		pi.tk = token;
		return pi;
	}

	pi = type();
	if(pi.er != none){
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	strcpy(varIdentifiers[varIDIndex], token.lx);
	varIDIndex++;

	while(strcmp(alltokens[currentindex].lx, ",")){
		currentindex++;
		token = alltokens[currentindex];
		if(currentindex >= numtokens){
			return pi;
		}
		currentindex++;

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		if(isVarIdentifier(token.lx) == 0 || isFunctionIdentifier(token.lx) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}

		strcpy(varIdentifiers[varIDIndex], token.lx);
		varIDIndex++;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

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

	Token token = alltokens[currentindex];

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

	Token token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "class") == 0)){
		pi.er = classExpected;
		pi.tk = token;
		return pi;
	}

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	}

	strcpy(functionIdentifiers[functionIDIndex], token.lx);
	functionIDIndex++;

	token = alltokens[currentindex];
	if(currentindex >= numtokens){
		return pi;
	}
	currentindex++;

	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	pi = memberDeclar();
	if(pi.er != none){
		return pi;
	}

	token = alltokens[currentindex];
	if(!(strcmp(token.lx, "}"))){
		pi.er = closeBraceExpected;
		pi.tk = token;
		return pi;
	}

	return pi;

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

	pi = class();

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
