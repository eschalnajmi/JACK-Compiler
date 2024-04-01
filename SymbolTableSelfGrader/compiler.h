#ifndef COMPILER_H
#define COMPILER_H

//#define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"
#include <dirent.h>
#include <stdio.h>

int InitCompiler ();
ParserInfo compile (char* dir_name);
int StopCompiler();

/*char* ErrorString (SyntaxErrors e)
{

	switch (e)
	{
		case none: return "no errors";
		case lexerErr: return "lexer error";
		case classExpected: return "keyword class expected";
		case idExpected: return "identifier expected";
		case openBraceExpected:	return "{ expected";
		case closeBraceExpected: return "} expected";
		case memberDeclarErr: return "class member declaration must begin with static, field, constructor , function , or method";
		case classVarErr: return "class variables must begin with field or static";
		case illegalType: return "a type must be int, char, boolean, or identifier";
		case semicolonExpected: return "; expected";
		case subroutineDeclarErr: return "subrouting declaration must begin with constructor, function, or method";
		case openParenExpected: return "( expected";
		case closeParenExpected: return ") expected";
		case closeBracketExpected: return "] expected";
		case equalExpected: return "= expected";
		case syntaxError: return "syntax error";
		// semantic errors
		case undecIdentifier: return "undeclared identifier";
		case redecIdentifier: return "redeclaration of identifier";
		default: return "not a valid error code";
	}
}

void PrintError (ParserInfo pn)
{
	if (pn.er == none)
		printf ("No errors\n");
	else
		printf ("Error in file %s line %i at or near %s: %s\n" , pn.tk.fl , pn.tk.ln , pn.tk.lx , ErrorString(pn.er));
}*/

#endif
