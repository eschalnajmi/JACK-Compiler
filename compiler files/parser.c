#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "compiler.h"


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

int numtokens;

// [] 0 or 1
// {} 0 or many
// integerConstant | identifier [.identifier ] [ [ expression ] | (expressionList ) ] | (expression) | stringLiteral | true | false | null | this
ParserInfo operand(){
	ParserInfo pi;
	pi.er = none;

	Token token =  GetNextToken();

	if(!(strcmp(token.lx, "true") == 0 || strcmp(token.lx, "false") == 0 || strcmp(token.lx, "null") == 0 || strcmp(token.lx, "this") == 0 || token.tp == STRING || token.tp == INT || token.tp == ID || strcmp(token.lx, "(") == 0)){
		pi.er = syntaxError;
		pi.tk = token;
		return pi;
	}

	if(token.tp == ID){

		if(FindUndeclarSymbol(token) == -1){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}

		token = PeekNextToken();
		if(strcmp(token.lx, ".") == 0){
			token = GetNextToken();
			
			token = GetNextToken();

			if(token.tp != ID){
				pi.er = idExpected;
				pi.tk = token;
				return pi;
			}

			if(FindUndeclarSymbol(token) == -1){
				pi.er = undecIdentifier;
				pi.tk = token;
				return pi;
			}

			token = PeekNextToken();

			if(strcmp(token.lx, "[") == 0){
				GetNextToken();
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
			} else if(strcmp(token.lx, "(") == 0){
				token = GetNextToken();
				pi = expressionList();

				if(pi.er != none){
					return pi;
				}

				token = GetNextToken();

				if(!(strcmp(token.lx, ")") == 0)){
					pi.er = closeParenExpected;
					pi.tk = token;
					return pi;
				}
			}
		} else if(strcmp(token.lx, "[") == 0){
			token = GetNextToken();
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
	}

	if(strcmp(token.lx, "(") == 0){
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
	}
	return pi;
}

//  ( - | ~ | ε ) operand
ParserInfo factor(){
	ParserInfo pi;
	pi.er = none;

	Token token = PeekNextToken();

	if (strcmp(token.lx, "-") == 0 || strcmp(token.lx, "~") == 0)
	{
		token = GetNextToken();
		pi = operand();
		if(pi.er != none){
			return pi;
		} 
		
	} else {
		pi = operand();
		if(pi.er != none){
			return pi;
		} 
	}

	return pi;
}

// factor { ( * | / ) factor }
ParserInfo term(){
	ParserInfo pi = factor();
	Token token;
	
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
	Token token;
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
	Token token;
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
	Token token;
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
	Token token = PeekNextToken();

	if(strcmp(token.lx, ";") == 0){
		token = GetNextToken();
		return pi;
	}

	pi = expression();
	if(pi.er != none){
		pi.er = semicolonExpected;
		return pi;
	}

	token = GetNextToken();

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

	if(strcmp(PeekNextToken().lx, ")") == 0){
		return pi;
	} else{
		pi = expression();
		if(pi.er != none){
			return pi;
		}
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

	if(FindUndeclarSymbol(token) == -1){
		pi.er = undecIdentifier;
		pi.tk = token;
		return pi;
	}

	token = GetNextToken();

	if(strcmp(token.lx, ".") == 0){
		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		}

		if(FindUndeclarSymbol(token) == -1){
			pi.er = undecIdentifier;
			pi.tk = token;
			return pi;
		}

		token = GetNextToken();
	}

	if(!(strcmp(token.lx, "(") == 0)){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}
	

	pi = expressionList();
	if(pi.er != none){
		return pi;
	}

	token = GetNextToken();

	if(!(strcmp(token.lx, ")") == 0)){
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
	if(!(strcmp(token.lx, ";") == 0)){
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

	newScope();

	while(!(strcmp(token.lx, "}") == 0)){
		pi = statement();
		if(pi.er != none){
			return pi;
		}	
		token = PeekNextToken();
	}

	token = GetNextToken();
	if(!(strcmp(token.lx, "}") == 0)){
		pi.er = closeBraceExpected;
		pi.tk = token;
	}

	endScope();

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

	newScope();

	while(!(strcmp(token.lx, "}") == 0)){
		token = PeekNextToken();
		pi = statement();
		if(pi.er != none){
			token = PeekNextToken();
			if(pi.er == undecIdentifier){
				return pi;
			}
			pi.er = closeBraceExpected;
			pi.tk = token;
			return pi;
		}	
		token = PeekNextToken();
	}

	endScope();

	GetNextToken();
	token = PeekNextToken();
	if(strcmp(token.lx, "else")== 0){
		GetNextToken();
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

	newScope();

	Token token = GetNextToken();
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
		token = PeekNextToken();
	}
	token = GetNextToken();

	if(!(strcmp(token.lx, "}") == 0)){
		pi.er = closeBraceExpected;
		pi.tk = token;
	}

	endScope();

	return pi;
}

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
	
	if(FindUndeclarSymbol(token) == -1){
		pi.er = undecIdentifier;
		pi.tk = token;
		return pi;
	}

	token = GetNextToken();

	if(strcmp(token.lx, "[") == 0){
		pi = expression();
		if(pi.er != none){
			return pi;
		}
		token = GetNextToken();
		if(!(strcmp(token.lx,"]") == 0)){
			pi.er = closeBracketExpected;
			pi.tk = token;
			return pi;
		}
		token = GetNextToken();
	}

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
	}  else if(FindRedeclarSymbol(token) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	InsertSymbol(token);

	token = GetNextToken();

	while(strcmp(token.lx, ",") == 0){
		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		} else if(FindRedeclarSymbol(token) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}

		InsertSymbol(token);

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

	if(!(strcmp(token.lx, "{") == 0)){
		pi.er = openBraceExpected;
		pi.tk = token;
		return pi;
	}

	while(!(strcmp(PeekNextToken().lx, "}") == 0)){
		pi = statement();
		if(pi.er != none){
			return pi;
		}
	}

	token = GetNextToken();
	
	if(!(strcmp(token.lx, "}") == 0)){
		pi.er = closeBraceExpected;
		pi.tk = token;
	}

	endScope();

	return pi;
}

// type identifier {, type identifier} | ε
ParserInfo paramList(){
	ParserInfo pi;
	pi.er = none;

	Token token = PeekNextToken();
	if(strcmp(token.lx, "{") == 0 || strcmp(token.lx, ")") == 0){
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
	} else if(FindRedeclarSymbol(token) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	InsertSymbol(token);

	while(strcmp(PeekNextToken().lx, ",") == 0){
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
		} else if(FindRedeclarSymbol(token) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}

		InsertSymbol(token);


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
	} else {
		GetNextToken();
	}

	token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	} else if(FindRedeclarSymbol(token) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	InsertSymbol(token);

	token = GetNextToken();

	if(!(strcmp(token.lx, "(") == 0)){
		pi.er = openParenExpected;
		pi.tk = token;
		return pi;
	}

	newScope();

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

	if(!(strcmp(token.lx, "int") == 0 || strcmp(token.lx, "char") == 0 || strcmp(token.lx, "boolean") == 0 || token.tp == ID)){
		pi.er = illegalType;
		pi.tk = token;
	}

	if(token.tp == ID && FindUndeclarSymbol(token) == -1){

		pi.er = undecIdentifier;
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
	} else if(FindRedeclarSymbol(token) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	InsertSymbol(token);

	while(strcmp(PeekNextToken().lx, ",") == 0){
		GetNextToken();
		token = GetNextToken();

		if(token.tp != ID){
			pi.er = idExpected;
			pi.tk = token;
			return pi;
		} else if(FindRedeclarSymbol(token) == 0){
			pi.er = redecIdentifier;
			pi.tk = token;
			return pi;
		}

		InsertSymbol(token);
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

	while(pi.er == none){
		token = PeekNextToken();
		if(strcmp(token.lx, "static") == 0 || strcmp(token.lx, "field") == 0){
			pi = classVarDeclar();
		} else if(strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0){
			pi = subroutineDeclar();
		} else if(strcmp(token.lx, "}") == 0 ){
			break;
		} else {
			pi.er = memberDeclarErr;
			pi.tk = token;
		}
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

	newScope();

	token = GetNextToken();

	if(token.tp != ID){
		pi.er = idExpected;
		pi.tk = token;
		return pi;
	} else if(FindRedeclarSymbol(token) == 0){
		pi.er = redecIdentifier;
		pi.tk = token;
		return pi;
	}

	InsertSymbol(token);

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
	if(!(strcmp(token.lx, "}") == 0)){
		pi.er = closeBraceExpected;
		pi.tk = token;
		return pi;
	}

	endScope();

	return pi;

}

int InitParser (char* file_name)
{
	InitLexer(file_name);

	char vmfn[1028];

	strcpy(vmfn, file_name);
	vmfn[strlen(file_name) - 4] = 'v';
	vmfn[strlen(file_name) - 3] = 'm';
	vmfn[strlen(file_name) - 2] = '\0';

	vmFile = fopen(vmfn, "w");

	if (vmFile == NULL)
	{
		return 0;
	}

	return 1;
}

ParserInfo Parse ()
{

	ParserInfo pi;

	Token token = GetNextToken();
	while(token.tp != EOFile){
		if(token.tp == ERR){
			pi.er = lexerErr;
			pi.tk = token;
			return pi;
		}
		token = GetNextToken();
	}
	StopParser();

	if(InitParser(token.fl) ==0){
		pi.er = lexerErr;
		pi.tk = token;
		return pi;
	}

	pi = class();

	return pi;
}


int StopParser ()
{
	StopLexer();

	if (vmFile != NULL)
	{
		fclose(vmFile);
	}

	return 1;
}
