/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Ethan McNamra
Student ID: sc20e2m / 201419445
Email: sc20e2m@leeds.ac.uk
Date Work Commenced: 12/02/2022
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

char reservedWords[21][12] = {"boolean", "char", "class", "constructor", "do",
"else", "false", "field", "function", "if", "int", "let", "method", "null",
"return", "static", "this", "true", "var", "void", "while"};
char trueFileName[32];
FILE *fptr;
int currentChar;
int lineCount = 1;
int prevChar;
int newFile = 0;
int nextTokenIsEmpty = 1;
int usePrev = 0;
Token currentToken;
Token nextToken;

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
  newFile = 1;
  fptr = fopen(file_name, "r");

  if(fptr == NULL) {
    return 0;
  } else {
    // Go through file_name until we know where the name/extension is
    int dirCount = 0;
    int secondPass = 0;
    int trueFileLen = 0;
    for(int i=0; i<strlen(file_name); i++) {
      if(file_name[i] == '/') {
        dirCount++;
      }
    }

    // Go through file_name again, now isolating the name/extension
    for(int i=0; i<strlen(file_name); i++) {
      if(secondPass == dirCount) {
        trueFileName[trueFileLen] = file_name[i];
        trueFileLen++;
      }

      if(file_name[i] == '/') {
        secondPass++;
      }
    }

    return 1;
  }
}


// Get the next token from the source file
Token GetNextToken ()
{
  if(newFile == 1) {
    newFile = 0;
  } else if(currentToken.tp == 5 || currentToken.tp == 6) {
    return currentToken;
  }

  Token t;

  if(nextTokenIsEmpty == 1) {
    // Used across all operations
    char lexeme[128];
    int lxLen = 0;

    // Used in comment operations
    int commentStart = 0;
    int ignoreText = 0;

    // Used in number operations
    int readNum = 0;

    // Used in operations on reserved words and identifiers
    int readName = 0;

    // Used in string operations
    int readString = 0;

    // Used in tracking new lines
    int pauseNewLineCheck = 0;

    // Search through code until first valid character found
    while(1) {
      if(usePrev == 1) {
        usePrev = 0;
      } else {
        prevChar = currentChar;
        currentChar = fgetc(fptr);
      }

      if(prevChar == '\n') {
        lineCount++;
      }

      if(readName == 1) {
        // Very lengthy way of checking if character is letter, number or '_'
        if((currentChar > 64 && currentChar < 91) ||
        (currentChar > 96 && currentChar < 123) || (currentChar == 95) ||
        (currentChar > 47 && currentChar < 58)) {
          // Keep reading
          lexeme[lxLen] = currentChar;
          lxLen++;
        } else {
          usePrev = 1;
          readName = 0;
          // End of word
          int reserveMatch = 0;
          lexeme[lxLen] = '\0';
          // Compare lexeme to all reserved words
          for(int i=0; i<21; i++) {
            if(strcmp(lexeme, reservedWords[i]) == 0) {
              reserveMatch = 1;
            }
          }

          if(reserveMatch == 1) {
            t.tp = RESWORD;
            strcpy(t.lx, lexeme);
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);
          } else {
            t.tp = ID;
            strcpy(t.lx, lexeme);
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);
          }

          for(int i=0; i<128; i++) {
            lexeme[i] = '\0';
          }

          lxLen = 0;

          currentToken = t;
          return currentToken;
        }
      } else if(readNum == 1) {
        if(currentChar > 47 && currentChar < 58) {
          // Keep reading
          lexeme[lxLen] = currentChar;
          lxLen++;
        } else {
          // End of number
          readNum = 0;
          usePrev = 1;

          lexeme[lxLen] = '\0';

          t.tp = INT;
          strcpy(t.lx, lexeme);
          t.ec = -1;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          for(int i=0; i<128; i++) {
            lexeme[i] = '\0';
          }

          lxLen = 0;

          currentToken = t;
          return currentToken;
        }
      } else if(readString == 1) { // If a '"' has been found in the code
        if(currentChar == '\n' || currentChar == '\r') {
          // Error, newline in string
          t.tp = ERR;
          strcpy(t.lx, "Error: new line in string constant");
          t.ec = 1;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          StopLexer();

          currentToken = t;
          return currentToken;
        } else if(currentChar == EOF) {
          // Error, EOF in string
          t.tp = ERR;
          strcpy(t.lx, "Error: unexpected eof in string constant");
          t.ec = 2;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          StopLexer();

          currentToken = t;
          return currentToken;
        } else {
          if(currentChar == '"') {
            // End of string, return token
            lexeme[lxLen] = '\0';
            readString = 0;
            lxLen = 0;

            t.tp = STRING;
            strcpy(t.lx, lexeme);
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);

            for(int i=0; i<128; i++) {
              lexeme[i] = '\0';
            }

            currentToken = t;
            return currentToken;
          } else {
            // Keep reading
            lexeme[lxLen] = currentChar;
            lxLen++;
          }
        }

      } else if(ignoreText == 0) { // Checks if a comment has started
        if(prevChar == '/' && currentChar == '/') {
          ignoreText = 1;
        } else if(prevChar == '/' && currentChar == '*') {
          commentStart = 1;
          ignoreText = 2;
        } else {
          // TEXT IS NOT A COMMENT, THIS TEXT SHOULD NOT BE IGNORED
          if(currentChar == EOF) {  // Stop at end of file
            t.tp = EOFile;
            strcpy(t.lx, "End of File");
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);

            StopLexer();

            currentToken = t;
            return currentToken;
          } else if(currentChar == '"') {
            readString = 1;
          } else if((currentChar > 64 && currentChar < 91) ||
          (currentChar > 96 && currentChar < 123) || currentChar == 95) {
            // lexeme is a reserved word or identifier
            readName = 1;
            lexeme[lxLen] = currentChar;
            lxLen++;
          } else if(currentChar > 47 && currentChar < 58){
            // lexeme is an integer
            readNum = 1;
            lexeme[lxLen] = currentChar;
            lxLen++;
          } else if((currentChar == 38) || (currentChar == 91) ||
          (currentChar == 93) ||
          (currentChar > 39 && currentChar < 48) ||
          (currentChar > 58 && currentChar < 63) ||
          (currentChar > 122 && currentChar < 127)) {

            if(currentChar == 47 && prevChar != 47) {
              prevChar = currentChar;
              currentChar = fgetc(fptr);
              usePrev = 1;

              if(prevChar == 47 && currentChar != 47 && currentChar != 42) {
                // Lexeme is a special symbol, namely '/'
                lexeme[0] = prevChar;
                lexeme[1] = '\0';

                t.tp = SYMBOL;
                strcpy(t.lx, lexeme);
                t.ec = -1;
                t.ln = lineCount;
                strcpy(t.fl, trueFileName);

                lexeme[0] = '\0';

                currentToken = t;
                return currentToken;
              }
            } else {
              // Lexeme is a special symbol
              lexeme[0] = currentChar;
              lexeme[1] = '\0';

              t.tp = SYMBOL;
              strcpy(t.lx, lexeme);
              t.ec = -1;
              t.ln = lineCount;
              strcpy(t.fl, trueFileName);

              lexeme[0] = '\0';

              currentToken = t;
              return currentToken;
            }
          } else if(isspace(currentChar) != 0) {
            // Ignore character
          } else {
            if(usePrev == 1) {
              // Do nothing
            } else {
              // Error, illegal symbol in file

              t.tp = ERR;
              strcpy(t.lx, "Error: illegal symbol in source file");
              t.ec = 3;
              t.ln = lineCount;
              strcpy(t.fl, trueFileName);

              StopLexer();

              currentToken = t;
              return currentToken;
            }
          }
        }
      } else if(ignoreText == 1) {  // Single-line comment is active
        if(currentChar == '\n' || currentChar == '\r') {
          ignoreText = 0;
        }
      } else if(ignoreText == 2) {  // Multi-line comment is active
        if(currentChar == EOF) {
          // Error, EOF in comment
          t.tp = ERR;
          strcpy(t.lx, "Error: unexpected eof in comment");
          t.ec = 0;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          StopLexer();

          currentToken = t;
          return currentToken;
        }

        if(commentStart == 1) {
          if(!(prevChar == '*' && currentChar == '*')) {
            commentStart = 0;
          }
        } else {
          if(prevChar == '*' && currentChar == '/') {
            ignoreText = 0;
          }
        }
      }
    }
  } else {
    currentToken = nextToken;
    nextTokenIsEmpty = 1;

    return currentToken;
  }

}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
  if(newFile == 0) {
    if(currentToken.tp == 5 || currentToken.tp == 6) {
      nextTokenIsEmpty = 0;
      return currentToken;
    }
  }

  if(nextTokenIsEmpty == 1) {
    // Find a token, place it in nextToken
    nextTokenIsEmpty = 0;

    // Used across all operations
    char lexeme[128];
    int lxLen = 0;
    Token t;

    // Used in comment operations
    int commentStart = 0;
    int ignoreText = 0;

    // Used in number operations
    int readNum = 0;

    // Used in operations on reserved words and identifiers
    int readName = 0;

    // Used in string operations
    int readString = 0;

    // Used in tracking new lines
    int pauseNewLineCheck = 0;

    // Search through code until first valid character found
    while(1) {
      if(usePrev == 1) {
        usePrev = 0;
      } else {
        prevChar = currentChar;
        currentChar = fgetc(fptr);
      }

      if(prevChar == '\n') {
        lineCount++;
      }

      if(readName == 1) {
        // Very lengthy way of checking if character is letter, number or '_'
        if((currentChar > 64 && currentChar < 91) ||
        (currentChar > 96 && currentChar < 123) || (currentChar == 95) ||
        (currentChar > 47 && currentChar < 58)) {
          // Keep reading
          lexeme[lxLen] = currentChar;
          lxLen++;
        } else {
          usePrev = 1;
          readName = 0;
          // End of word
          int reserveMatch = 0;
          lexeme[lxLen] = '\0';
          // Compare lexeme to all reserved words
          for(int i=0; i<21; i++) {
            if(strcmp(lexeme, reservedWords[i]) == 0) {
              reserveMatch = 1;
            }
          }

          if(reserveMatch == 1) {
            t.tp = RESWORD;
            strcpy(t.lx, lexeme);
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);
          } else {
            t.tp = ID;
            strcpy(t.lx, lexeme);
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);
          }

          for(int i=0; i<128; i++) {
            lexeme[i] = '\0';
          }

          lxLen = 0;

          nextToken = t;
          return nextToken;
        }
      } else if(readNum == 1) {
        if(currentChar > 47 && currentChar < 58) {
          // Keep reading
          lexeme[lxLen] = currentChar;
          lxLen++;
        } else {
          // End of number
          readNum = 0;
          usePrev = 1;

          lexeme[lxLen] = '\0';

          t.tp = INT;
          strcpy(t.lx, lexeme);
          t.ec = -1;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          for(int i=0; i<128; i++) {
            lexeme[i] = '\0';
          }

          lxLen = 0;

          nextToken = t;
          return nextToken;
        }
      } else if(readString == 1) { // If a '"' has been found in the code
        if(currentChar == '\n' || currentChar == '\r') {
          // Error, newline in string
          t.tp = ERR;
          strcpy(t.lx, "Error: new line in string constant");
          t.ec = 1;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          nextToken = t;
          return nextToken;
        } else if(currentChar == EOF) {
          // Error, EOF in string
          t.tp = ERR;
          strcpy(t.lx, "Error: unexpected eof in string constant");
          t.ec = 2;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          nextToken = t;
          return nextToken;
        } else {
          // Keep reading
          if(prevChar == '"') {
            lexeme[lxLen] = prevChar;
          }
          lxLen++;
          lexeme[lxLen] = currentChar;

          if(currentChar == '"') {
            // End of string, return token
            lexeme[lxLen+1] = '\0';
            readString = 0;
            lxLen = 0;

            t.tp = STRING;
            strcpy(t.lx, lexeme);
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);

            for(int i=0; i<128; i++) {
              lexeme[i] = '\0';
            }

            nextToken = t;
            return nextToken;
          }
        }

      } else if(ignoreText == 0) { // Checks if a comment has started
        if(prevChar == '/' && currentChar == '/') {
          ignoreText = 1;
        } else if(prevChar == '/' && currentChar == '*') {
          commentStart = 1;
          ignoreText = 2;
        } else {
          // IS NOT A COMMENT, THIS TEXT SHOULD NOT BE IGNORED
          if(currentChar == EOF) {  // Stop at end of file
            t.tp = EOFile;
            strcpy(t.lx, "End of File");
            t.ec = -1;
            t.ln = lineCount;
            strcpy(t.fl, trueFileName);

            nextToken = t;
            return nextToken;
          } else if(currentChar == '"') {
            readString = 1;
          } else if((currentChar > 64 && currentChar < 91) ||
          (currentChar > 96 && currentChar < 123) || currentChar == 95) {
            // lexeme is a reserved word or identifier
            readName = 1;
            lexeme[lxLen] = currentChar;
            lxLen++;
          } else if(currentChar > 47 && currentChar < 58){
            // lexeme is an integer
            readNum = 1;
            lexeme[lxLen] = currentChar;
            lxLen++;
          } else if((currentChar == 38) || (currentChar == 91) ||
          (currentChar == 93) ||
          (currentChar > 39 && currentChar < 48) ||
          (currentChar > 58 && currentChar < 63) ||
          (currentChar > 122 && currentChar < 127)) {
            if(prevChar != 47 && currentChar == 47) {
              prevChar = currentChar;
              currentChar = fgetc(fptr);
              usePrev = 1;

              if(prevChar == 47 && currentChar != 47  && currentChar != 42) {
                // Lexeme is a special symbol, namely '/'
                lexeme[0] = prevChar;
                lexeme[1] = '\0';

                t.tp = SYMBOL;
                strcpy(t.lx, lexeme);
                t.ec = -1;
                t.ln = lineCount;
                strcpy(t.fl, trueFileName);

                lexeme[0] = '\0';

                nextToken = t;
                return nextToken;
              }
            } else {
              // Lexeme is a special symbol
              lexeme[0] = currentChar;
              lexeme[1] = '\0';

              t.tp = SYMBOL;
              strcpy(t.lx, lexeme);
              t.ec = -1;
              t.ln = lineCount;
              strcpy(t.fl, trueFileName);

              lexeme[0] = '\0';

              nextToken = t;
              return nextToken;
            }
          } else if(isspace(currentChar) != 0) {
            // Ignore character
          } else {
            if(usePrev == 1) {
              // Do nothing
            } else {
              // Error, illegal symbol in file

              t.tp = ERR;
              strcpy(t.lx, "Error: illegal symbol in source file");
              t.ec = 3;
              t.ln = lineCount;
              strcpy(t.fl, trueFileName);

              nextToken = t;
              return nextToken;
            }
          }
        }
      } else if(ignoreText == 1) {  // Single-line comment is active

        if(currentChar == '\n' || currentChar == '\r') {
          ignoreText = 0;
        }
      } else if(ignoreText == 2) {  // Multi-line comment is active
        if(currentChar == EOF) {
          // Error, EOF in comment
          t.tp = ERR;
          strcpy(t.lx, "Error: unexpected eof in comment");
          t.ec = 0;
          t.ln = lineCount;
          strcpy(t.fl, trueFileName);

          nextToken = t;
          return nextToken;
        }

        if(commentStart == 1) {
          if(!(prevChar == '*' && currentChar == '*')) {
            commentStart = 0;
          }
        } else {
          if(prevChar == '*' && currentChar == '/') {
            ignoreText = 0;
          }
        }
      }
    }
  } else {
    // This may need to change in later code, depending on how it is tested

    return nextToken;
  }
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  if(fptr != NULL) {
    fclose(fptr);
    fptr = NULL;
  }
  nextTokenIsEmpty = 1;
  lineCount = 1;

  for(int i=0; i<32; i++) {
    trueFileName[i] = '\0';
  }

	return 0;
}

// do not remove the next line
#ifndef TEST
/*
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function

  // Change file to test here


	return 0;
}
*/
// do not remove the next line
#endif
