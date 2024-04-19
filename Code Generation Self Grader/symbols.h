#ifndef SYMBOLS_H
#define SYMBOLS_H

#define TEST_COMPILER

#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define your own types and function prototypes for the symbol table(s) module below

typedef enum{
    Math,
    Memory,
    Screen,
    Output,
    Keyboard,
    String,
    Array,
    Sys
} JACKlibrariesEnum;


typedef struct {
    Token token;
    JACKlibrariesEnum lib;
} Symbol;

typedef struct {
	Symbol symbols [1000];
	int address;
    int scope;
} SymbolTable;

typedef struct {
	//Symbol varidentifiers [1000];
    //int varpointer;
    Symbol methodidentifiers [1000];
    int methodpointer;
    Symbol classIdentifiers [1000];
    int classpointer;
    Symbol librariesUsed[8];
    int librariesUsedPointer;
} dirSymbolTable;


void initglobalST();
//void addglobalvar(Token tk);
void addglobalmethod(Token tk);
void addglobalclass(Token tk);
int findGlobal(Token t);
void newScope();
void endScope();
void InitSymbolTable();
void InsertSymbol(Token tk);
int FindUndeclarSymbol(Token t);
int FindRedeclarSymbol(Token t);
void StopSymbolTable();
void globalParse();
void addJACKMethods(Token tk);



#endif
