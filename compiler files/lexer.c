#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

char* arrayinput;
int arraysize;
int currentindex;
char *keywords[21] = {"class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return"};
char filename[32];
int line = 1;
int pline = 1;
char delimiters[5] = {' ', '\n', '\t', '\r'};
char symbols[22] = {'{', '}', '(', ')', '[', ']', '.', ',', ';','+','-', '*', '/', '&', '|', '<', '>', '=', '~'};

int isletter(char c){
    if(c <= 122 && c>= 97){
        return 0;
    }
    if(c >= 65 && c <=90){
        return 0;
    }
    return 1;
}


// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name){
    line = 1;
    currentindex = 0;
    arraysize = 0;
    strcpy(filename, file_name);

    // open the file
    int numchars = 0;
    FILE *file;
    file = fopen(file_name,"rw");

    // check if file is empty
    if (file == NULL){
        return 0;
    }

    // get the file length
    while(fgetc(file) != EOF){
        numchars++;
    }

    fclose(file);
    file = fopen(file_name,"rw");

    arrayinput = malloc((numchars+1) * sizeof(char));
    

    for(int i = 0; i < numchars; i++){
        arrayinput[arraysize] = fgetc(file);
        arraysize++;
    }

    arrayinput[arraysize] = '\0';
    arraysize++;

    fclose(file);

    return 1;
}

// get the next token from the source file
Token GetNextToken (){
    Token t;
    strcpy(t.fl, filename);
    char currenttoken[128];
    char completetoken[128];
    int ctindex = 0;
    int start = currentindex;


    for(int i = start; i < arraysize; i++){
        if(isspace(arrayinput[i]) && arrayinput[i] != '\n'){
            continue;
        }
        // if C is the start symbol of a comment then skip all the characters in the body of the comment and go back to 1
        if(arrayinput[i] == '/' && arrayinput[i+1] == '*'){
            for(int j = i; j < arraysize; j++){
                if(arrayinput[j] == '\n'){
                    line++;
                } else if(arrayinput[j] == '\0'){
                    t.tp = ERR;
                    t.ec = EofInCom;
                    strcpy(completetoken, "Error: unexpected eof in comment");
                    break;
                }
                if(arrayinput[j] == '*' && arrayinput[j+1] == '/'){
                    i = j+1;
                    currentindex = i;
                    break;
                }
            }
            if(t.tp == ERR){
                break;
            }
            continue;
        } else if(arrayinput[i] == '/' && arrayinput[i+1] == '/'){
            for(int j = i; j < arraysize; j++){
                if(arrayinput[j] == '\n'){
                    line++;
                    i = j;
                    currentindex = i;
                    break;
                } else if(arrayinput[j] == '\0'){
                    t.tp = ERR;
                    t.ec = EofInCom;
                    strcpy(completetoken, "Error: unexpected eof in comment");
                    break;
                }
            }
            if(t.tp == ERR){
                break;
            }
            continue;
        }

        if(arrayinput[i] == '\n'){
            line++;
            continue;
        }

        if(i == arraysize - 1){
            t.tp = EOFile;
            break;
        }


        // If C is a quote ("), it may be the start of a literal string, then keep reading more characters, putting them into a string, until you hit the closing quote
        if(arrayinput[i] == '"'){
            t.tp = STRING;
            for(int j = i + 1; j < arraysize; j++){
                if(arrayinput[j] == '"'){
                    currenttoken[ctindex] = '\0';
                    ctindex++;
                    currentindex = j+1;
                    strcpy(completetoken, currenttoken);
                    break;
                } else if(arrayinput[j] == '\n'){
                    t.tp = ERR;
                    t.ec = NewLnInStr;
                    strcpy(completetoken, "Error: new line in string constant");
                    break;
                } else if(arrayinput[j] == '\0'){
                    t.tp = ERR;
                    t.ec = EofInStr;
                    strcpy(completetoken, "Error: unexpected eof in string constant");
                    break;
                }else{
                    currenttoken[ctindex] = arrayinput[j];
                    ctindex++;
                }
            }
            break;
        }

        if(isletter(arrayinput[i]) == 0 || arrayinput[i] == '_'){
            t.tp = ID;
            currenttoken[ctindex] = arrayinput[i];
            ctindex++;
            for(int j = i + 1; j < arraysize; j++){
                if(isletter(arrayinput[j]) == 0 || isdigit(arrayinput[j]) || arrayinput[j] == '_'){
                    currenttoken[ctindex] = arrayinput[j];
                    ctindex++;
                } else{
                    currenttoken[ctindex] = '\0';
                    ctindex++;
                    currentindex = j;
                    strcpy(completetoken, currenttoken);
                    break;
                }
            }
            for(int j = 0; j < 21; j++){

                if(strcmp(completetoken, keywords[j]) == 0){
                    t.tp = RESWORD;
                    break;
                }
            }

            break;
        }

        if(isdigit(arrayinput[i])){
            t.tp = INT;
            currenttoken[ctindex] = arrayinput[i];
            ctindex++;
            for(int j = i + 1; j < arraysize; j++){
                if(isdigit(arrayinput[j])){
                    currenttoken[ctindex] = arrayinput[j];
                    ctindex++;
                } else{
                    currenttoken[ctindex] = '\0';
                    ctindex++;
                    currentindex = j;
                    strcpy(completetoken, currenttoken);
                    break;
                }
            }
            break;
        }


        int symb_found = 0;
        for(int j = 0; j < 22; j++){
            if(arrayinput[i] == symbols[j]){
                symb_found = 1;
                t.tp = SYMBOL;
                currenttoken[ctindex] = arrayinput[i];
                ctindex++;
                currenttoken[ctindex] = '\0';
                ctindex++;
                currentindex = i+1;
                strcpy(completetoken, currenttoken);
                break;
            }
        }

        if (symb_found == 1) break;

        t.tp = ERR;
        t.ec = IllSym;
        strcpy(completetoken, "Error: illegal symbol in source file");
        break;
    }

    strcpy(t.lx, completetoken);
    t.ln = line;
    return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken (){
    Token t;
    strcpy(t.fl, filename);
    char currenttoken[128];
    char completetoken[128];
    int ctindex = 0;
    int start = currentindex;
    pline = line;

    for(int i = start; i < arraysize; i++){
        if(isspace(arrayinput[i]) && arrayinput[i] != '\n'){
            continue;
        }
        // if C is the start symbol of a comment then skip all the characters in the body of the comment and go back to 1
        if(arrayinput[i] == '/' && arrayinput[i+1] == '*'){
            for(int j = i; j < arraysize; j++){
                if(arrayinput[j] == '\n'){
                    pline++;
                } else if(arrayinput[j] == '\0'){
                    t.tp = ERR;
                    t.ec = EofInCom;
                    strcpy(completetoken, "Error: unexpected eof in comment");
                    break;
                }
                if(arrayinput[j] == '*' && arrayinput[j+1] == '/'){
                    i = j+1;
                    break;
                }
            }
            if(t.tp == ERR){
                break;
            }
            continue;
        } else if(arrayinput[i] == '/' && arrayinput[i+1] == '/'){
            for(int j = i; j < arraysize; j++){
                if(arrayinput[j] == '\n'){
                    pline++;
                    i = j;
                    break;
                } else if(arrayinput[j] == '\0'){
                    t.tp = ERR;
                    t.ec = EofInCom;
                    strcpy(completetoken, "Error: unexpected eof in comment");
                    break;
                }
            }
            if(t.tp == ERR){
                break;
            }
            continue;
        }

        if(arrayinput[i] == '\n'){
            pline++;
            continue;
        }

        if(i == arraysize - 1){
            t.tp = EOFile;
            break;
        }


        // If C is a quote ("), it may be the start of a literal string, then keep reading more characters, putting them into a string, until you hit the closing quote
        if(arrayinput[i] == '"'){
            t.tp = STRING;
            for(int j = i + 1; j < arraysize; j++){
                if(arrayinput[j] == '"'){
                    currenttoken[ctindex] = '\0';
                    ctindex++;
                    strcpy(completetoken, currenttoken);
                    break;
                } else if(arrayinput[j] == '\n'){
                    t.tp = ERR;
                    t.ec = NewLnInStr;
                    strcpy(completetoken, "Error: new line in string constant");
                    break;
                } else if(arrayinput[j] == '\0'){
                    t.tp = ERR;
                    t.ec = EofInStr;
                    strcpy(completetoken, "Error: unexpected eof in string constant");
                    break;
                }else{
                    currenttoken[ctindex] = arrayinput[j];
                    ctindex++;
                }
            }
            break;
        }

        if(isletter(arrayinput[i]) == 0 || arrayinput[i] == '_'){
            t.tp = ID;
            currenttoken[ctindex] = arrayinput[i];
            ctindex++;
            for(int j = i + 1; j < arraysize; j++){
                if(isletter(arrayinput[j]) == 0 || isdigit(arrayinput[j]) || arrayinput[j] == '_'){
                    currenttoken[ctindex] = arrayinput[j];
                    ctindex++;
                } else{
                    currenttoken[ctindex] = '\0';
                    ctindex++;
                    strcpy(completetoken, currenttoken);
                    break;
                }
            }
            for(int j = 0; j < 21; j++){

                if(strcmp(completetoken, keywords[j]) == 0){
                    t.tp = RESWORD;
                    break;
                }
            }

            break;
        }

        if(isdigit(arrayinput[i])){
            t.tp = INT;
            currenttoken[ctindex] = arrayinput[i];
            ctindex++;
            for(int j = i + 1; j < arraysize; j++){
                if(isdigit(arrayinput[j])){
                    currenttoken[ctindex] = arrayinput[j];
                    ctindex++;
                } else{
                    currenttoken[ctindex] = '\0';
                    ctindex++;
                    strcpy(completetoken, currenttoken);
                    break;
                }
            }
            break;
        }


        int symb_found = 0;
        for(int j = 0; j < 22; j++){
            if(arrayinput[i] == symbols[j]){
                symb_found = 1;
                t.tp = SYMBOL;
                currenttoken[ctindex] = arrayinput[i];
                ctindex++;
                currenttoken[ctindex] = '\0';
                ctindex++;
                strcpy(completetoken, currenttoken);
                break;
            }
        }

        if (symb_found == 1) break;

        t.tp = ERR;
        t.ec = IllSym;
        strcpy(completetoken, "Error: illegal symbol in source file");
        break;
    }

    strcpy(t.lx, completetoken);
    t.ln = pline;
    return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer (){
    free(arrayinput);
	return 0;
}
