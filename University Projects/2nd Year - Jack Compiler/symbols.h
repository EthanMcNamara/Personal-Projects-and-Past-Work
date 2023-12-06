#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below

typedef enum {NONE, STATIC, FIELD, ARGUMENT, VAR} kind;

// a structure for representing each symbol
typedef struct {
  char id[128];                   // Identfier used to represent the symbol
  char tp[128];                   // Return type of the symbol (ie int, char, boolean, void, or some identifier)
  char classID[128];              // Identifier of the class the symbol belongs to
  char subID[128];                // Name of the subroutine the symbol belongs to
  kind k;                         // Kind of variable (ie static, field, argument or var)
  int kindCount;                  // Relative position of that kind of variable
  Token tk;                       // A token, used for returning errors checked at the end of the program
  int isError;                    // Error code, used when checking errors at the end of the program, 0 when no error and 1 when there is an error
} Symbol;

// Initialisation and ending of program
int InitTables();                 // Initialises the symbol tables and the 'unknown' array
int StopTables();                 // Frees all three arrays

// Management of current location
int SetClass(char *s);            // Set the current class the compiler is parsing, returns 1 if successful and 0 if not
int SetSub(char *s);              // Set the current subroutine the compiler is parsing, returns 1 if successful and 0 if not
const char* GetClass();           // Return the current class the compiler is parsing
const char* GetSub();             // Return the current subroutine the compiler is parsing
int GetCount(int varType);        // Return the current count of a kind of variable

// Management of arrays
int Insert(Symbol s);             // Insert a symbol into the table, returns 1 if successful and 0 if not
int LookUp(Symbol inSym);         // Search the current class for a symbol matching the identifier, returns 1 if successful and 0 if not
int InsertUnknown(Symbol s);      // Insert a symbol into a separate array to check at the end of the list, returns 1 if successful and 0 if not
Symbol CheckUnknown();            // For every symbol in the 'unknown' array, check if there is a match now the symbol table is complete, returns 1 if successful and 0 if not
Symbol GetSymbol(Symbol inSym);   // Search for a symbol we know exists to get all the information for it

#endif
