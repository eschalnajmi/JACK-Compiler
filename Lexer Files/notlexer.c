// cc -std=c99 lexer.c -o lexer
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Eschal Najmi
Student ID: 201602938
Email: sc22en@leeds.ac.uk
Date Work Commenced: 08/02/2024
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

char* arrayinput;
int arraysize;
int currentindex;
int numchars = 0;
char *keywords[21] = {"class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return"};
char filename[32];
int line = 1;
char *delimiters[5] = {' ', '\n', '\t', '\r'};


char* TokenTypeString (TokenType t)
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

void PrintToken (Token t)
{
	printf ("<%s, %i, %s, %s>\n", t.fl, t.ln , t.lx, TokenTypeString (t.tp));
}


// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name){
  currentindex = 0;
  arraysize = 0;
  strcpy(filename, file_name);

  // open the file
  numchars = 0;
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
  /*

    // If C is a digit, it may be the first digit in a number, then keep reading more digits until you hit a character that is not a digit
    if(isdigit(arrayinput[i])){
      t.tp = INT;
      for(int j = i; j < arraysize; j++){
        if(isdigit(arrayinput[i])){
          currenttoken[ctindex] = arrayinput[i];
          ctindex++;
        } else{
          currentindex = i+1;
          break;
        }
      }
      break;
    }

    // if C is a letter, it may be the first letter in a keyword or identifier, then keep reading more letters and/or digits, putting them into a string, until you hit a character that isneither a letter nor a digitTry to find the resulting string in a list of keywords. If the string is found then the token is a keyword,otherwise it is an identifier.Put the string (lexeme) in a token with the proper token type (keyword or id) and return the token
    if(isalpha(arrayinput[i])){
      t.tp = ID;
      for(int j = i; j < arraysize; j++){
        if(isalpha(arrayinput[i] || isdigit(arrayinput[i]))){
          currenttoken[ctindex] = arrayinput[i];
          ctindex++;
        } else{
          currentindex = i+1;
          for(int i = 0; i < 12; i++){
            if(strcmp(currenttoken, keywords[i]) == 0){
              t.tp = RESWORD;
            }
          }
          break;
        }
      }
      break;
    }

    if(isspace(arrayinput[i])){
      break;
    }

    currenttoken[ctindex] = arrayinput[i];
    ctindex++;
    currentindex = i+1;
  }
  */

  Token t;
  strcpy(t.fl, filename);
  char currenttoken[128];
  char completetoken[128];
  int ctindex = 0;
  int start = currentindex;

  for(int i = start; i < arraysize; i++){
    if(arrayinput[i] =='\0'){
      t.tp = EOFile;
      break;
    }
    // skip any white space characters until you hit the first non-whitespace character
    if(isspace(arrayinput[i])){
      start++;
      continue;
    }
    
    if(arrayinput[i] == '\n'){
      line++;
      start++;
      continue;
    }

    // if C is the start symbol of a comment then skip all the characters in the body of the comment and go back to 1
    if(arrayinput[i] == '/' && arrayinput[i+1] == '*'){
      for(int j = i; j < arraysize; j++){
        if(arrayinput[j] == '\n'){
          line++;
        }
        if(arrayinput[j] == '*' && arrayinput[j+1] == '/'){
          i = j+2;
          break;
        }
      }
      continue;
    } else if(arrayinput[i] == '/' && arrayinput[i+1] == '/'){
      for(int j = i; j < arraysize; j++){
        if(arrayinput[j] == '\n'){
          line++;
          i = j;
          break;
        }
      }
      continue;
    }

    // If C is a quote ("), it may be the start of a literal string, then keep reading more characters, putting them into a string, until you hit the closing quote
    if(arrayinput[i] == '"'){
      t.tp = STRING;
      for(int j = i + 1; j < arraysize; j++){
        if(arrayinput[j] == '"'){
          currenttoken[ctindex] = '\0';
          ctindex++;
          currentindex = j+2;
          strcpy(completetoken, currenttoken);
          break;
        } else{
          currenttoken[ctindex] = arrayinput[j];
          ctindex++;
          continue;
        }
      }
      break;
    }

    // If C is a digit, it may be the first digit in a number, then keep reading more digits until you hit a character that is not a digit
    if(isdigit(arrayinput[i]) && isspace(arrayinput[i-1]) && isspace){
      t.tp = INT;
      for(int j = i; j < arraysize; j++){
        if(isdigit(arrayinput[i])){
          currenttoken[ctindex] = arrayinput[i];
          ctindex++;
        } else{
          currentindex = i+1;
          break;
        }
      }
      break;
    }

    if(isspace(arrayinput[i+1]) || arrayinput[i+1] == '\n' || arrayinput[i+1] == '\0'){
      t.tp = ID;
      if(arrayinput[i+1] == '\n'){
        line++;
      }
      currenttoken[ctindex] = arrayinput[i];
      ctindex++;
      currenttoken[ctindex] = '\0';
      ctindex++;
      currentindex = i+1;
      strcpy(completetoken, currenttoken);
      break;
    } 

    currenttoken[ctindex] = arrayinput[i];
    ctindex++;
    currentindex = i+2;

  }

  for(int i = 0; i < 21; i++){
    if(strcmp(currenttoken, keywords[i]) == 0){
      printf("Keyword");
      t.tp = RESWORD;
    }
  }

  t.ln = line;
  for(int i = 0; i < ctindex; i++){
    t.lx[i] = completetoken[i];
  }

  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken (){
  Token t;
  t.tp = ERR;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer (){
  free(arrayinput);
	return 0;
}

// do not remove the next line
#ifndef TEST
int main (){
	// implement your main function here
  // NOTE: the autograder will not use your main function
  InitLexer("notexist.jack");
  Token token = GetNextToken();

  while(token.tp != EOFile){
    PrintToken(token);
    token = GetNextToken ();
  }
  StopLexer();
  return 0;
}
// do not remove the next line
#endif
