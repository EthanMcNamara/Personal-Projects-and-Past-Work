
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"

char currentClass[128];
char currentSub[128];
int stcCount = 0;
int fldCount = 0;
int argCount = 0;
int varCount = 0;
int classSymCount = 0;
int subSymCount = 0;
int unknownCount = 0;
Symbol* classSyms;
Symbol* subSyms;
Symbol* unknown;

int InitTables() {
  classSyms = (Symbol*)malloc(sizeof(Symbol));
  subSyms = (Symbol*)malloc(sizeof(Symbol));
  unknown = (Symbol*)malloc(sizeof(Symbol));
}

int StopTables() {
  free(classSyms);
  free(subSyms);
  free(unknown);

  classSymCount = 0;
  subSymCount = 0;
  unknownCount = 0;
}

int SetClass(char *s) {
  stcCount = 0;
  fldCount = 0;

  for(int i=0; i<128; i++) {
    currentClass[i] = '\0';
  }

  strcpy(currentClass, s);
}

int SetSub(char *s) {
  argCount = 0;
  varCount = 0;

  for(int i=0; i<128; i++) {
    currentSub[i] = '\0';
  }

  strcpy(currentSub, s);
  subSymCount = 0;

  subSyms = (Symbol*)realloc(subSyms, sizeof(Symbol));

  return 1;
}

const char* GetClass() {
  return currentClass;
}

const char* GetSub() {
  return currentSub;
}

int GetCount(int varType) {
  if(varType == 1) {
    return stcCount;
  } else if(varType == 2) {
    return fldCount;
  } else if(varType == 3) {
    return argCount;
  } else if(varType == 4) {
    return varCount;
  }
}

int Insert(Symbol s) {

  // Insert a symbol into the table, returns 1 if successful and 0 if not
  if(s.k < 3) {
    if(s.k == 1) {
      stcCount++;
    } else if(s.k == 2) {
      fldCount++;
    }

    classSymCount++;
    classSyms = (Symbol*)realloc(classSyms, (classSymCount)*sizeof(Symbol));
    classSyms[classSymCount - 1] = s;
    // Add to ClassSyms;

    return 1;
  } else {
    if(s.k == 3) {
     argCount++;
   } else if(s.k == 4) {
     varCount++;
   }

   subSymCount++;
   subSyms = (Symbol*)realloc(subSyms, (subSymCount)*sizeof(Symbol));
   subSyms[subSymCount -1] = s;
   // Add to SubSyms;

   return 1;
  }

  return 0;
}

int LookUp(Symbol inSym) {

  for(int i=0; i<subSymCount; i++) {
    Symbol currSym = subSyms[i];
    if(strcmp(currSym.id, inSym.id) == 0 && strcmp(currSym.classID, inSym.classID) == 0) {
      return 1;
    }
  }

  for(int i=0; i<classSymCount; i++) {
    Symbol currSym = classSyms[i];
    if(strcmp(currSym.id, inSym.id) == 0 && strcmp(currSym.classID, inSym.classID) == 0) {
      if(inSym.k  == 4) {
        // Ignore
      } else {
        return 2;
      }
    }
  }

  return 0;
}

Symbol GetSymbol(Symbol inSym) {
  for(int i=0; i<subSymCount; i++) {
    Symbol currSym = subSyms[i];
    if(strcmp(currSym.id, inSym.id) == 0 && strcmp(currSym.classID, inSym.classID) == 0) {
      currSym.isError = 0;
      return currSym;
    }
  }

  for(int i=0; i<classSymCount; i++) {
    Symbol currSym = classSyms[i];
    if(strcmp(currSym.id, inSym.id) == 0 && strcmp(currSym.classID, inSym.classID) == 0) {
      currSym.isError = 0;
      return currSym;
    }
  }

  Symbol s;
  s.isError = 1;
  s.k = NONE;
  s.kindCount = 3;
  return s;
}

int InsertUnknown(Symbol s) {
  unknownCount++;
  unknown = (Symbol*)realloc(unknown, (unknownCount)*sizeof(Symbol));
  unknown[unknownCount - 1] = s;

  return 0;
}

Symbol CheckUnknown() {
  for(int x=0; x<unknownCount; x++) {
    int matchFound = 0;

    Symbol uS = unknown[x];

    for(int y=0; y<classSymCount; y++) {
      Symbol cS = classSyms[y];

      if(uS.k == NONE) {
        // Check against all subroutines;
        if(strcmp(cS.tp, "Class") != 0 && cS.k == NONE) {
          if(strcmp(uS.id, cS.id) == 0) {
            matchFound++;
          }
        }
      } else {
        // Check against all classes and variables
        if(strcmp(uS.tp, cS.id) == 0 || strcmp(uS.id, cS.id) == 0) {
          if(uS.k == cS.k) {
            // Do nothing, invalid match
          } else {
            matchFound++;
          }

        }
      }
    }

    if(matchFound == 0) {
      uS.isError = 1;
      return uS;
    }
  }
  // Otherwise, no error
  Symbol s;
  s.isError = 0;
  return s;
}
