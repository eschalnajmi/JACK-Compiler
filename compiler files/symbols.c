#include "symbols.h"



SymbolTable* symbol_tables[1000];
dirSymbolTable* globalST;
int pointer = 0;


void initglobalST(){
    globalST = malloc(sizeof(dirSymbolTable));
    globalST->methodpointer = 0;
    globalST->classpointer = 0;
    globalST->librariesUsedPointer = 0;
}

void addglobalmethod(Token tk){
    globalST->methodidentifiers[globalST->methodpointer].token = tk;
    globalST->methodpointer++;
}

void addglobalclass(Token tk){
    globalST->classIdentifiers[globalST->classpointer].token = tk;
    globalST->classpointer++;
}

void globalParse(){
	Token token = GetNextToken();
	while(token.tp != EOFile){
		if(strcmp(token.lx, "class") == 0){
			addglobalclass(GetNextToken());
		} else if(strcmp(token.lx, "function") == 0 || strcmp(token.lx, "method") == 0 || strcmp(token.lx, "constructor") == 0 || strcmp(token.lx, "function") == 0){
			GetNextToken();
			addglobalmethod(GetNextToken());
		} else if(strcmp(token.lx, "Math") == 0 || strcmp(token.lx, "Memory") == 0 || strcmp(token.lx, "Screen") == 0 || strcmp(token.lx, "Output") == 0 || strcmp(token.lx, "Keyboard") == 0 || strcmp(token.lx, "String") == 0 || strcmp(token.lx, "Array") == 0 || strcmp(token.lx, "Sys") == 0){
            if(findGlobal(token) == -1 && globalST->librariesUsedPointer < 8){
                globalST->librariesUsed[globalST->librariesUsedPointer].token = token;
                globalST->librariesUsedPointer++;
                addJACKMethods(token);
            }
        }
		token = GetNextToken();
	}
}

void addJACKMethods(Token tk){
    Token newmethod;
    newmethod.tp = ID;
    if(strcmp(tk.lx, "Math") == 0){
        strcpy(newmethod.lx, "abs");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "multiply");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "divide");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "min");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "max");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "sqrt");
        addglobalmethod(newmethod);
    } else if(strcmp(tk.lx, "Memory") == 0){
        strcpy(newmethod.lx, "peek");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "poke");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "alloc");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "deAlloc");
        addglobalmethod(newmethod);

    } else if(strcmp(tk.lx, "Screen") == 0){
        strcpy(newmethod.lx, "clearScreen");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "setColor");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "drawPixel");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "drawLine");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "drawRectangle");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "drawCircle");
        addglobalmethod(newmethod);

    } else if(strcmp(tk.lx, "Output") == 0){
        strcpy(newmethod.lx, "init");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "moveCursor");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "printChar");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "printString");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "println");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "backSpace");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "printInt");
        addglobalmethod(newmethod);

    } else if(strcmp(tk.lx, "Keyboard") == 0){
        strcpy(newmethod.lx, "keyPressed");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "readChar");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "readLine");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "readInt");
        addglobalmethod(newmethod);

    } else if(strcmp(tk.lx, "String") == 0){
        strcpy(newmethod.lx, "new");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "dispose");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "length");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "charAt");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "setCharAt");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "appendChar");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "eraseLastChar");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "intValue");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "setInt");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "backSpace");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "newLine");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "doubleQuote");
        addglobalmethod(newmethod);

    } else if(strcmp(tk.lx, "Array") == 0){
        strcpy(newmethod.lx, "new");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "dispose");
        addglobalmethod(newmethod);

    } else if(strcmp(tk.lx, "Sys") == 0){
        strcpy(newmethod.lx, "halt");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "error");
        addglobalmethod(newmethod);

        strcpy(newmethod.lx, "wait");
        addglobalmethod(newmethod);
    }
}

int findGlobal(Token t){
    for(int i = 0; i < globalST->methodpointer; i++){
        if(strcmp(globalST->methodidentifiers[i].token.lx, t.lx) == 0){
            return 0;
        }
    }

    for(int i = 0; i < globalST->classpointer; i++){
        if(strcmp(globalST->classIdentifiers[i].token.lx, t.lx) == 0){
            return 0;
        }
    }

    for(int i = 0; i < globalST->librariesUsedPointer; i++){
        if(strcmp(globalST->librariesUsed[i].token.lx, t.lx) == 0){
            return 0;
        }
    }
    return -1;
}

void newScope(){
    symbol_tables[pointer] = malloc(sizeof(SymbolTable));
    symbol_tables[pointer]->address = 0;
    symbol_tables[pointer]->scope = pointer;
    pointer++;
}

void endScope(){
    if(pointer > 0){
        free(symbol_tables[pointer-1]);
        pointer--;
    }
}

void InitSymbolTable(){
    symbol_tables[pointer] = malloc(sizeof(SymbolTable));
    symbol_tables[pointer]->address = 0;
    symbol_tables[pointer]->scope = pointer;
    pointer++;
}

void InsertSymbol(Token tk){
    // insert a symbol into the symbol table
    symbol_tables[pointer-1]->symbols[symbol_tables[pointer-1]->address].token = tk;
    symbol_tables[pointer-1]->address++;
}

int FindUndeclarSymbol(Token t){
    // lookup a symbol in the symbol table
    for(int i = pointer-1; i >= 0; i--){
        for(int j = 0; j < symbol_tables[i]->address; j++){
            if(strcmp(symbol_tables[i]->symbols[j].token.lx, t.lx) == 0){
                return 0;
            }
        }
    }

    return findGlobal(t);
}

int FindRedeclarSymbol(Token t){
    // lookup a symbol in the symbol table
    for(int i = pointer-1; i >= 0; i--){
        for(int j = 0; j < symbol_tables[i]->address; j++){
            if(strcmp(symbol_tables[i]->symbols[j].token.lx, t.lx) == 0 && i == pointer-1 && strcmp(t.fl, symbol_tables[i]->symbols[j].token.fl) == 0){
                return 0;
            }
        }
    }
    return -1;
}

void StopSymbolTable(){
    for(int i = 0; i < pointer; i++){
        free(symbol_tables[i]);
    }

    free (globalST); 

    pointer = 0;
}