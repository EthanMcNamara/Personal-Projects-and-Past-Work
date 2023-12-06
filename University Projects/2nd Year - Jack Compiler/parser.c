#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"


// you can declare prototypes of parser functions below
char outFileName[128];
char subRetType[128];
int pushThat = 0;
int lateMath = 0;
int ifCount = 0;
int whileCount = 0;
int expCount = 0;
int subType = 0;
int tempCount = 0;
int varDecCheck = 1;
FILE *outFile;
Token tkn;

ParserInfo ClassDeclar(); // Implemented
ParserInfo MemberDeclar(); // Implemented
ParserInfo ClassVarDeclar(); // Implemented
ParserInfo SubroutineDeclar(); // Implemented
ParserInfo Type(); // Implemented
ParserInfo ParamList(); // Implemented
ParserInfo SubroutineBody(); // Implemented
ParserInfo Statement(); // Implemented
ParserInfo VarDeclarStatement(); // Implemented
ParserInfo LetStatement(); // Implemented
ParserInfo IfStatement(); // Implemented
ParserInfo WhileStatement(); // Implemented
ParserInfo DoStatement(); // Implemented
ParserInfo ReturnStatement(); // Implemented
ParserInfo Expression(); // Implemented
ParserInfo SubroutineCall(); // Implemented
ParserInfo RelationalExpression(); // Implemented
ParserInfo ExpressionList(); // Implemented
ParserInfo ArithmeticExpression(); // Implemented
ParserInfo Term(); // Implemented
ParserInfo Factor(); // Implemented
ParserInfo Operand(); // Implemented

ParserInfo ClassDeclar() {
	tkn = GetNextToken();

	// Check token is RESWORD and says "class"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "class") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = classExpected;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is ID
	if(tkn.tp != ID) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}

	SetClass(tkn.lx);

	Symbol s;
	strcpy(s.id, tkn.lx);
	strcpy(s.tp, "Class");
	strcpy(s.classID, tkn.lx);
	strcpy(s.subID, "0");
	s.k = NONE;

	Insert(s);

	// Check token is open brace
	tkn = GetNextToken();
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "{") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openBraceExpected;
		return pi;
	}

	tkn = PeekNextToken();
	// Check that there are 0 or more RESWORD tokens
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "}") == 0) && tkn.tp != RESWORD) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = memberDeclarErr;
		return pi;
	}
	while(tkn.tp == RESWORD) {
		ParserInfo pi;
		pi = MemberDeclar();
		if(pi.er != none) {
			return pi;
		}
		tkn = PeekNextToken();
	}

	// Check token is close brace
	tkn = GetNextToken();
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "}") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openBraceExpected;
		return pi;
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo MemberDeclar() {
	ParserInfo pi;
	tkn = PeekNextToken();

	// Check token is RESWORD and says either: "static", "field", "constructor",
	// "function" or "method"
	if(tkn.tp == RESWORD && (strcmp(tkn.lx, "static") == 0 ||
	strcmp(tkn.lx, "field") == 0)) {
		pi = ClassVarDeclar();
		if(pi.er != none) {
		}
	} else if(tkn.tp == RESWORD && (strcmp(tkn.lx, "constructor") == 0 ||
	strcmp(tkn.lx, "function") == 0 || strcmp(tkn.lx, "method") == 0)) {
		pi = SubroutineDeclar();
		if(pi.er != none) {
		}
	} else {
		pi.tk = tkn;
		pi.er = memberDeclarErr;
	}

	return pi;
}

ParserInfo ClassVarDeclar() {
	int varType = 0;
	Token tType;
	int knownVar = 0;
	char retType[128];

	tkn = GetNextToken();
	// Double check that token is RESWORD and says either "static" or "field"
	if(!(tkn.tp == RESWORD && (strcmp(tkn.lx, "static") == 0 ||
	strcmp(tkn.lx, "field") == 0))) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = classVarErr;
		return pi;
	}

	// Set up 'kind' of symbol
	if(strcmp(tkn.lx, "static") == 0) {
		varType = 1;
	} else if(strcmp(tkn.lx, "field") == 0) {
		varType = 2;
	}

	tkn = PeekNextToken();
	// Check that token is RESWORD or ID
	if(tkn.tp == RESWORD || tkn.tp == ID) {
		strcpy(retType, tkn.lx);
		strcpy(subRetType, tkn.lx);
		if(tkn.tp == RESWORD) {
			knownVar = 1;
		} else {
			tType = tkn;
		}

		ParserInfo pi = Type();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = illegalType;
		return pi;
	}

	tkn = GetNextToken();
	// Check that token is ID
	if(tkn.tp != ID) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}

	Symbol s;
	strcpy(s.id, tkn.lx);
	strcpy(s.tp, retType);
	strcpy(s.classID, GetClass());
	strcpy(s.subID, "0");
	s.k = varType;
	s.kindCount = GetCount(varType);

	if(LookUp(s) == 0) {
		Insert(s);
		if(knownVar != 1) {
			s.tk = tType;
			InsertUnknown(s);
		}
	} else {

		ParserInfo pi;
		pi.tk = tkn;
		pi.er = redecIdentifier;

		tkn = GetNextToken();

		return pi;
	}

	tkn = PeekNextToken();
	// Check that tokens are a series of commas and IDs
	while(tkn.tp == SYMBOL && strcmp(tkn.lx, ",") == 0) {
		tkn = GetNextToken();
		tkn = GetNextToken();
		if(tkn.tp != ID) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = idExpected;
			return pi;
		}

		Symbol s2;
		strcpy(s2.id, tkn.lx);
		strcpy(s2.tp, retType);
		strcpy(s2.classID, GetClass());
		strcpy(s2.subID, "0");
		s2.k = varType;
		s2.kindCount = GetCount(varType);

		if(LookUp(s2) == 0) {
			Insert(s2);
			if(knownVar != 1) {
				s2.tk = tType;
				InsertUnknown(s2);
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = redecIdentifier;

			tkn = GetNextToken();

			return pi;
		}

		tkn = PeekNextToken();
	}

	tkn = GetNextToken();
	// Checks that token is a semicolon
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ";") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = semicolonExpected;
		return pi;
	}

	for(int i=0; i<128; i++) {
		subRetType[i] = '\0';
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo SubroutineDeclar() {
	// Checks what type of subroutine, 1 if constructor, 2 if function and 3 if
	// method
	Token tType;
	int knownVar = 0;
	char retType[128];

	tkn = GetNextToken();
	// Check that token is RESWORD and says either "constructor", "function" or
	// "method"
	if(!(tkn.tp == RESWORD && (strcmp(tkn.lx, "constructor") == 0 ||
	strcmp(tkn.lx, "function") == 0 || strcmp(tkn.lx, "method") == 0))) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = subroutineDeclarErr;
		return pi;
	}

	if(strcmp(tkn.lx, "constructor") == 0) {
		subType = 1;
	} else if(strcmp(tkn.lx, "function") == 0) {
		subType = 2;
	} else {
		subType = 3;
	}

	tkn = PeekNextToken();
	// Check that token is RESOWRD and says "void", or that token is RESWORD or ID
	if(tkn.tp == RESWORD && strcmp(tkn.lx, "void") == 0) {
		knownVar = 1;
		strcpy(retType, tkn.lx);
		tkn = GetNextToken();
	} else if(tkn.tp == RESWORD || tkn.tp == ID) {
		if(tkn.tp == RESWORD) {
			knownVar = 1;
		} else {
			tType = tkn;
		}
		strcpy(retType, tkn.lx);
		ParserInfo pi = Type();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = illegalType;
		return pi;
	}

	tkn = GetNextToken();
	// Checks that token is ID
	if(tkn.tp != ID) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}

	SetSub(tkn.lx);

	Symbol s;
	strcpy(s.id, tkn.lx);
	strcpy(s.tp, retType);
	strcpy(s.classID, GetClass());
	strcpy(s.subID, "0");
	s.k = NONE;
	s.kindCount = subType;

	if(LookUp(s) == 0) {
		Insert(s);
		if(knownVar != 1 && (strcmp(s.tp, GetClass()) != 0)) {
			s.tk = tType;
			InsertUnknown(s);
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = redecIdentifier;

		tkn = GetNextToken();

		return pi;
	}

	tkn = GetNextToken();
	// Checks that token is an open parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "(") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openParenExpected;
		return pi;
	}

	tkn = PeekNextToken();
	// Checks that token is RESWORD or ID
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0) {
		// Do nothing
	}else if(tkn.tp == RESWORD || tkn.tp == ID) {
		ParserInfo pi = ParamList();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeParenExpected;
		return pi;
	}

	tkn = GetNextToken();
	// Check that token is a closed parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeParenExpected;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if this is the end of the Member Declaration
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, "{") == 0) {
		ParserInfo pi = SubroutineBody();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openBraceExpected;
		return pi;
	}

	varDecCheck = 1;
}

ParserInfo Type() {
	tkn = GetNextToken();
	ParserInfo pi;
	pi.tk = tkn;

	// Check that token is RESWORD and says "int", "char" or "boolean" or that
	// token is ID
	if(!((tkn.tp == RESWORD && (strcmp(tkn.lx, "int") == 0 ||
	strcmp(tkn.lx, "char") == 0 || strcmp(tkn.lx, "boolean") == 0) ||
	tkn.tp == ID))) {
		pi.er = illegalType;
	} else {
		pi.er = none;
	}

	return pi;
}

ParserInfo ParamList() {
	Token tType;
	int knownVar = 0;
	char retType[128];

	if(subType == 3) {
		// if subroutine is a method, add 'this'
		Symbol s;
		strcpy(s.id, "this");
		strcpy(s.tp, retType);
		strcpy(s.classID, GetClass());
		strcpy(s.subID, GetSub());
		s.k = ARGUMENT;
		s.kindCount = GetCount(3);
		Insert(s);
	}

	tkn = PeekNextToken();
	// Check if token is open parenthesis
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, ")")) {
		tkn = GetNextToken();
	} else {
		tkn = PeekNextToken();

		// Check if token is RESWORD or ID
		if(tkn.tp == RESWORD || tkn.tp == ID) {
			if(tkn.tp == RESWORD) {
				knownVar = 1;
			} else {
				tType = tkn;
			}
			strcpy(retType, tkn.lx);

			ParserInfo pi = Type();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = illegalType;
			return pi;
		}

		tkn = GetNextToken();
		// Check if token is ID
		if(tkn.tp != ID) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = idExpected;
			return pi;
		}

		Symbol s;
		strcpy(s.id, tkn.lx);
		strcpy(s.tp, retType);
		strcpy(s.classID, GetClass());
		strcpy(s.subID, GetSub());
		s.k = ARGUMENT;
		s.kindCount = GetCount(3);

		if(LookUp(s) == 0) {
			Insert(s);
			if(knownVar != 1) {
				s.tk = tType;
				InsertUnknown(s);
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = redecIdentifier;

			tkn = GetNextToken();

			return pi;
		}

		for(int i=0; i<128; i++) {
			retType[i] = '\0';
		}

		tkn = PeekNextToken();
		// Check that tokens are a series of commas Types and IDs
		while(tkn.tp == SYMBOL && strcmp(tkn.lx, ",") == 0) {
			knownVar = 0;
			tkn = GetNextToken();
			tkn = PeekNextToken();
			// Check that token is RESWORD or ID
			if(tkn.tp == RESWORD || tkn.tp == ID) {
				if(tkn.tp == RESWORD) {
					knownVar = 1;
				} else {
					tType = tkn;
				}
				strcpy(retType, tkn.lx);

				ParserInfo pi = Type();
				if(pi.er != none) {
					return pi;
				}
			} else {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = illegalType;
				return pi;
			}

			tkn = GetNextToken();
			// Check if token is ID
			if(tkn.tp != ID) {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = idExpected;
				return pi;
			}

			Symbol s2;
			strcpy(s2.id, tkn.lx);
			strcpy(s2.tp, retType);
			strcpy(s2.classID, GetClass());
			strcpy(s2.subID, GetSub());
			s2.k = ARGUMENT;
			s2.kindCount = GetCount(3);

			if(LookUp(s2) == 0) {
				Insert(s2);
				if(knownVar != 1) {
					s2.tk = tType;
					InsertUnknown(s2);
				}
			} else {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = redecIdentifier;

				tkn = GetNextToken();

				return pi;
			}

			tkn = PeekNextToken();
		}
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo SubroutineBody() {
	tkn = GetNextToken();
	// Check if token is open brace
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "{") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openBraceExpected;
		return pi;
	}

	tkn = PeekNextToken();
	while(tkn.tp == RESWORD) {
		// Check if token is RESWORD
		if(tkn.tp == RESWORD) {
			ParserInfo pi = Statement();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}
		tkn = PeekNextToken();
	}

	tkn = GetNextToken();
	// Check if token is closed brace
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "}") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeBraceExpected;
		return pi;
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo Statement() {
	tkn = PeekNextToken();
	ParserInfo pi;

	if(tkn.tp == RESWORD && strcmp(tkn.lx, "var") != 0) {
		// No more var statements in this subroutine
		if(varDecCheck == 1) {
			varDecCheck = 0;

			// Define the subroutine here instead
			int varCount = GetCount(4);
			if(subType == 1) {
				// Constructor
				int fldCount = GetCount(2);
				int varCount = GetCount(4);
				fprintf(outFile, "function %s.%s %i\n", GetClass(), GetSub(), varCount);
				fprintf(outFile, "push constant %i\n", fldCount);
				fprintf(outFile, "call Memory.alloc 1\n");
				fprintf(outFile, "pop pointer 0\n");
			} else if(subType == 2) {
				// Function
				fprintf(outFile, "function %s.%s %i\n", GetClass(), GetSub(), varCount);
			} else {
				// Method
				fprintf(outFile, "function %s.%s %i\n", GetClass(), GetSub(), varCount);
				fprintf(outFile, "push argument 0\n");
				fprintf(outFile, "pop pointer 0\n");
			}
		}

	}

	// Double check if token is RESWORD and says either "var", "let", "if",
	// "while", "do" or "return"
	if(tkn.tp == RESWORD && strcmp(tkn.lx, "var") == 0) {
		pi = VarDeclarStatement();
	} else if(tkn.tp == RESWORD && strcmp(tkn.lx, "let") == 0) {
		pi = LetStatement();
	} else if(tkn.tp == RESWORD && strcmp(tkn.lx, "if") == 0) {
		pi = IfStatement();
	} else if(tkn.tp == RESWORD && strcmp(tkn.lx, "while") == 0) {
		pi = WhileStatement();
	} else if(tkn.tp == RESWORD && strcmp(tkn.lx, "do") == 0) {
		pi = DoStatement();
	} else if(tkn.tp == RESWORD && strcmp(tkn.lx, "return") == 0) {
		pi = ReturnStatement();
	} else {
		pi.tk = tkn;
		pi.er = syntaxError;
	}

	return pi;
}

ParserInfo VarDeclarStatement() {
	Token tType;
	int knownVar = 0;
	char retType[128];

	tkn = GetNextToken();
	// Double check token is RESWORD annd says "var"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "var") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();
	// Check token is RESWORD or ID
	if(tkn.tp == RESWORD || tkn.tp == ID) {
		if(tkn.tp == RESWORD) {
			knownVar = 1;
		} else {
			tType = tkn;
		}
		strcpy(retType, tkn.lx);

		ParserInfo pi = Type();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = illegalType;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is ID
	if(tkn.tp != ID) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}

	Symbol s;
	strcpy(s.id, tkn.lx);
	strcpy(s.tp, retType);
	strcpy(s.classID, GetClass());
	strcpy(s.subID, GetSub());
	s.k = VAR;
	s.kindCount = GetCount(4);

	if(LookUp(s) == 0) {
		Insert(s);
		if(knownVar != 1) {
			s.tk = tType;
			InsertUnknown(s);
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = redecIdentifier;

		tkn = GetNextToken();

		return pi;
	}

	tkn = PeekNextToken();
	// Check tokens are a series of commas and IDs
	while(tkn.tp == SYMBOL && strcmp(tkn.lx, ",") == 0) {
		tkn = GetNextToken();
		tkn = GetNextToken();
		if(tkn.tp != ID) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = idExpected;
			return pi;
		}

		Symbol s2;
		strcpy(s2.id, tkn.lx);
		strcpy(s2.tp, retType);
		strcpy(s2.classID, GetClass());
		strcpy(s2.subID, GetSub());
		s2.k = 4;
		s2.kindCount = GetCount(4);

		if(LookUp(s2) == 0) {
			Insert(s2);
			if(knownVar != 1) {
				s.tk = tType;
				InsertUnknown(s);
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = redecIdentifier;

			tkn = GetNextToken();

			return pi;
		}

		tkn = PeekNextToken();
	}

	tkn = GetNextToken();
	// Check token is a semicolon
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ";") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = semicolonExpected;
		return pi;
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo LetStatement() {
	int useThat = 0;
	tkn = GetNextToken();
	// Check token is RESWORD and says "let"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "let") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is ID
	if(tkn.tp != ID) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}

	// Create a Symbol for the search
	Symbol s;
	strcpy(s.id, tkn.lx);
	strcpy(s.classID, GetClass());

	if(LookUp(s) == 0) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = undecIdentifier;

		tkn = GetNextToken();

		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is open bracket
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, "[") == 0) {
		tkn = GetNextToken();
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {

			ParserInfo pi = Expression();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = GetNextToken();
		// Check token is closed bracket
		if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "]") == 0)) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = closeBracketExpected;
			return pi;
		}

		Symbol pushSym = GetSymbol(s);

		if(pushSym.k == 1) {
			fprintf(outFile, "push static %i\n", pushSym.kindCount);
		} else if(pushSym.k == 2) {
			fprintf(outFile, "push this %i\n", pushSym.kindCount);
		} else if(pushSym.k == 3) {
			fprintf(outFile, "push argument %i\n", pushSym.kindCount);
		} else if(pushSym.k == 4) {
			fprintf(outFile, "push local %i\n", pushSym.kindCount);
		}

		fprintf(outFile, "add\n");

		useThat = 1;
	}

	if(pushThat == 1) {
		pushThat = 0;
	}

	tkn = GetNextToken();
	// Check token is an equals sign
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "=") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = equalExpected;
		GetNextToken(); // KEEP
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Expression();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is a semicolon
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ";") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = semicolonExpected;
		return pi;
	}

	if(useThat == 1) {
		fprintf(outFile, "pop temp 0\n");
		fprintf(outFile, "pop pointer 1\n");
		fprintf(outFile, "push temp 0\n");
		fprintf(outFile, "pop that 0\n");
	} else {
		Symbol popSym = GetSymbol(s);

		if(pushThat == 1) {
			pushThat = 0;
			fprintf(outFile, "pop pointer 1\n");
			fprintf(outFile, "push that 0\n");
			if(lateMath == 1) {
				fprintf(outFile, "add\n");
				lateMath = 0;
			} else if(lateMath == 2) {
				fprintf(outFile, "sub\n");
				lateMath = 0;
			}
		}


		if(popSym.k == 1) {
			fprintf(outFile, "pop static %i\n", popSym.kindCount);
		} else if(popSym.k == 2) {
			fprintf(outFile, "pop this %i\n", popSym.kindCount);
		} else if(popSym.k == 3) {
			fprintf(outFile, "pop argument %i\n", popSym.kindCount);
		} else if(popSym.k == 4) {
			fprintf(outFile, "pop local %i\n", popSym.kindCount);
		}
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo IfStatement() {
	int currentIfCount = ifCount;
	int hasElse = 0;
	tkn = GetNextToken();
	// Double check token is RESWORD and says "if"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "if") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	ifCount++;

	tkn = GetNextToken();
	// Check token is an open parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "(") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openParenExpected;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Expression();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is closed parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeParenExpected;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is open brace
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "{") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openBraceExpected;
		return pi;
	}

	fprintf(outFile, "if-goto IF_TRUE%i\n", currentIfCount);
	fprintf(outFile, "goto IF_FALSE%i\n", currentIfCount);
	fprintf(outFile, "label IF_TRUE%i\n", currentIfCount);

	tkn = PeekNextToken();
	// Check if token is RESWORD
	while(tkn.tp == RESWORD) {
		ParserInfo pi = Statement();
		if(pi.er != none) {
			return pi;
		}

		tkn = PeekNextToken();
	}

	tkn = GetNextToken();
	// Check token is closed brace
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "}") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeBraceExpected;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if next token is RESWORD and says "else"
	if(tkn.tp == RESWORD && strcmp(tkn.lx, "else") == 0) {
		tkn = GetNextToken();
		tkn = GetNextToken();
		// Check token is open brace
		if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "{") == 0)) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = openBraceExpected;
			return pi;
		}

		hasElse = 1;
		fprintf(outFile, "goto IF_END%i\n", currentIfCount);
		fprintf(outFile, "label IF_FALSE%i\n", currentIfCount);

		tkn = PeekNextToken();
		// Check if token is RESWORD
		while(tkn.tp == RESWORD) {
			ParserInfo pi = Statement();
			if(pi.er != none) {
				return pi;
			}

			tkn = PeekNextToken();
		}

		tkn = GetNextToken();
		// Check token is closed brace
		if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "}") == 0)) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = closeBraceExpected;
			return pi;
		}

		fprintf(outFile, "label IF_END%i\n", currentIfCount);
	}

	if(hasElse == 0) {
		fprintf(outFile, "label IF_FALSE%i\n", currentIfCount);
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo WhileStatement() {
	int currentWhileCount = whileCount;
	tkn = GetNextToken();
	// Double check token is RESOWRD and says "while"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "while") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	whileCount++;

	tkn = GetNextToken();
	// Check token is open parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "(") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openParenExpected;
		return pi;
	}

	fprintf(outFile, "label WHILE_EXP%i\n", currentWhileCount);

	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Expression();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is closed parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeParenExpected;
		return pi;
	}

	fprintf(outFile, "not\n");
	fprintf(outFile, "if-goto WHILE_END%i\n", currentWhileCount);

	tkn = GetNextToken();
	// Check token is open brace
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "{") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openBraceExpected;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is RESWORD
	while(tkn.tp == RESWORD) {
		ParserInfo pi = Statement();
		if(pi.er != none) {
			return pi;
		}

		tkn = PeekNextToken();
	}

	tkn = GetNextToken();
	// Check token is closed brace
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "}") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeBraceExpected;
		return pi;
	}

	fprintf(outFile, "goto WHILE_EXP%i\n", currentWhileCount);
	fprintf(outFile, "label WHILE_END%i\n", currentWhileCount);

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo DoStatement() {
	tkn = GetNextToken();
	// Double check token is RESWORD and says "do"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "do") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();
	// Check token is ID
	if(tkn.tp == ID) {
		ParserInfo pi = SubroutineCall();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}

	tkn = GetNextToken();
	// Check token is a semicolon
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ";") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = semicolonExpected;
		return pi;
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo ReturnStatement() {
	tkn = GetNextToken();

	// Double check token is RESWORD and says "return"
	if(!(tkn.tp == RESWORD && strcmp(tkn.lx, "return") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, ";") == 0) {
		fprintf(outFile, "push constant 0\n");
	} else if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Expression();
		if(pi.er != none) {
			// This means there should be a semicolon
			pi.tk = tkn;
			pi.er = semicolonExpected;
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	fprintf(outFile, "return\n");

	tkn = GetNextToken();
	// Check token is semicolon
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ";") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = semicolonExpected;
		return pi;
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo Expression() {
	int logicCount = 0;
	char logicQueue[128] = "";
	tkn = PeekNextToken();
	if(tkn.ln == 80) {
	}
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = RelationalExpression();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is "&" or "|"
	while(tkn.tp == SYMBOL && (strcmp(tkn.lx, "&") == 0 ||
	strcmp(tkn.lx, "|") == 0)) {
		logicQueue[logicCount] = tkn.lx[0];
		logicCount++;
		tkn = GetNextToken();
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {
			ParserInfo pi = RelationalExpression();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = PeekNextToken();
	}

	for(int i=0; i<logicCount; i++) {
		if(logicQueue[i] == '&') {
			fprintf(outFile, "and\n");
		} else if(logicQueue[i] == '|') {
			fprintf(outFile, "or\n");
		}
	}

	for(int i=0; i<128; i++) {
		logicQueue[i] = '\0';
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo SubroutineCall() {
	char potentialClass[128] = "";
	int hasDot = 0;
	int useClassName = 0;
	Token tType;
	Token tType2;
	tkn = GetNextToken();
	// Check token is ID
	if(tkn.tp != ID) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = idExpected;
		return pi;
	}
	tType = tkn;

	tkn = PeekNextToken();
	// Check if token is a dot
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, ".") == 0) {
		hasDot = 1;

		Symbol searchSym;
		strcpy(searchSym.id, tType.lx);
		strcpy(searchSym.classID, GetClass());
		Symbol pushSym = GetSymbol(searchSym);

		if(pushSym.isError == 0 && pushSym.k == VAR) {
			fprintf(outFile, "push local %i\n", pushSym.kindCount);
			if(expCount == 0) {
				expCount++;
			}
			useClassName = 1;
		} else if(pushSym.isError == 0 && pushSym.k == FIELD) {
			fprintf(outFile, "push this %i\n", pushSym.kindCount);
			if(expCount == 0) {
				expCount++;
			}
			useClassName = 1;
		}

		tkn = GetNextToken();
		tkn = GetNextToken();
		// Check token is ID
		if(tkn.tp != ID) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = idExpected;
			return pi;
		}

		tType2 = tkn;

		Symbol s;
		strcpy(s.id, tkn.lx);
		strcpy(s.classID, GetClass());
		s.tk = tkn;
		s.k = NONE;

		Symbol s2;
		strcpy(s2.id, tType.lx);
		strcpy(s2.classID, GetClass());
		if(LookUp(s2) == 0) {
			//ie we are working with a class, real or nonexistant
			s2.tk = tType;
			s2.k = 4;
			InsertUnknown(s2);
		}

		if(useClassName == 1) {
			for(int i=0; i<128; i++) {
				tType.lx[i] = '\0';
			}

			strcpy(tType.lx, pushSym.tp);
		}

		InsertUnknown(s);

	} else {
		// Check that subroutine exists!!

		Symbol s;
		strcpy(s.id, tType.lx);
		strcpy(s.classID, GetClass());

		if(LookUp(s) == 0) {
			s.tk = tType;
			s.k = NONE;
			InsertUnknown(s);
		}
	}

	Symbol s;
	Symbol pushSym;

	if(hasDot == 1) {
		strcpy(s.id, tType2.lx);
		strcpy(s.classID, tType.lx);
		pushSym = GetSymbol(s);
	} else  {
		strcpy(s.id, tType.lx);
		strcpy(s.classID, GetClass());
		pushSym = GetSymbol(s);
	}

	if(pushSym.k == NONE && pushSym.kindCount == 3 && useClassName != 1) {
		if(strcmp(s.classID, GetClass()) == 0 && hasDot == 1) {
			// Do nothing
		} else {
			fprintf(outFile, "push pointer 0\n");
			expCount++;
		}
	}

	tkn = GetNextToken();
	// Check token is open parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "(") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = openParenExpected;
		GetNextToken(); // KEEP
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0) {
		// Do nothing
	} else	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = ExpressionList();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	if(hasDot == 1) {
		fprintf(outFile, "call %s.%s %i\n", tType.lx, tType2.lx, expCount);
	} else  {
		fprintf(outFile, "call %s.%s %i\n", GetClass(), tType.lx, expCount);
	}

	fprintf(outFile, "pop temp 0\n");

	hasDot = 0;
	expCount = 0;

	tkn = GetNextToken();
	// Check token is closed parenthesis
	if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0)) {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = closeParenExpected;
		return pi;
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo RelationalExpression() {
	int relCount = 0;
	char relQueue[128] = "";
	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = ArithmeticExpression();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();

	while(tkn.tp == SYMBOL && (strcmp(tkn.lx, "=") == 0 ||
	strcmp(tkn.lx, ">") == 0 || strcmp(tkn.lx, "<") == 0)) {
		relQueue[relCount] = tkn.lx[0];
		relCount++;
		tkn = GetNextToken();
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {
			ParserInfo pi = ArithmeticExpression();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = PeekNextToken();
	}

	for(int i=0; i<relCount; i++) {
		if(relQueue[i] == '=') {
			fprintf(outFile, "eq\n");
		} else if(relQueue[i] == '>') {
			fprintf(outFile, "gt\n");
		} else if(relQueue[i] == '<') {
			fprintf(outFile, "lt\n");
		}
	}

	for(int i=0; i<128; i++) {
		relQueue[i] = '\0';
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo ExpressionList() {
	tkn = PeekNextToken();
	// Check if token is open parenthesis
	if(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0) {
		// Do nothing
	} else {
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {
			ParserInfo pi = Expression();
			expCount++;
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = PeekNextToken();
		int currentExpCount = expCount;
		// Check if tokens are a series of commas and SYMBOL, INT, ID, RESWORD or
		// STRING
		while(tkn.tp == SYMBOL && strcmp(tkn.lx, ",") == 0) {
			tkn = GetNextToken();
			tkn = PeekNextToken();
			// Check if token is SYMBOL, INT, ID, RESWORD or STRING
			if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
			tkn.tp == STRING) {
				currentExpCount = expCount;
				expCount = 0;
				ParserInfo pi = Expression();
				if(pi.er != none) {
					return pi;
				}
				expCount += currentExpCount;
				expCount++;
			} else {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = syntaxError;
				return pi;
			}

			tkn = PeekNextToken();
		}
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo ArithmeticExpression() {
	int mathCount = 0;
	char mathQueue[128] = "";
	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Term();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();
	// Check if token is plus or minus symbol
	while(tkn.tp == SYMBOL && (strcmp(tkn.lx, "+") == 0 ||
	strcmp(tkn.lx, "-") == 0)) {
		mathQueue[mathCount] = tkn.lx[0];
		mathCount++;
		tkn = GetNextToken();
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {
			ParserInfo pi = Term();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = PeekNextToken();
	}

	for(int i=0; i<mathCount; i++) {
		if(mathQueue[i] == '+') {
			if(pushThat == 0) {
				fprintf(outFile, "add\n");

				if(pushThat == 1) {
					pushThat = 0;
					fprintf(outFile, "pop pointer 1\n");
					fprintf(outFile, "push that 0\n");
				}
			} else {
				lateMath = 1;
			}
		} else if(mathQueue[i] == '-') {
			if(pushThat == 0) {
				fprintf(outFile, "sub\n");
			} else {
				lateMath = 2;
			}
		}
	}

	for(int i=0; i<128; i++) {
		mathQueue[i] = '\0';
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo Term() {
	char mathQueue[128] = "";
	int mathCount = 0;
	tkn = PeekNextToken();
	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Factor();
		if(pi.er != none) {
			return pi;
		}
	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	tkn = PeekNextToken();
	while(tkn.tp == SYMBOL && (strcmp(tkn.lx, "*") == 0 ||
	strcmp(tkn.lx, "/") == 0)) {
		mathQueue[mathCount] = tkn.lx[0];
		mathCount++;
		tkn = GetNextToken();
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {
			ParserInfo pi = Factor();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = PeekNextToken();
	}

	for(int i=0; i<mathCount; i++) {
		if(mathQueue[i] == '*') {
			fprintf(outFile, "call Math.multiply 2\n");
		} else if(mathQueue[i] == '/') {
			fprintf(outFile, "call Math.divide 2\n");
		}
	}

	for(int i=0; i<128; i++) {
		mathQueue[i] = '\0';
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo Factor() {
	tkn = PeekNextToken();
	Token tType = tkn;
	int checkLater = 0;
	// Check if token is "-" or "~"
	if(tkn.tp == SYMBOL && (strcmp(tkn.lx, "-") == 0 ||
	strcmp(tkn.lx, "~") == 0)) {
		checkLater = 1;
		tkn = GetNextToken();
		tkn = PeekNextToken();
	}

	// Check if token is SYMBOL, INT, ID, RESWORD or STRING
	if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
	tkn.tp == STRING) {
		ParserInfo pi = Operand();
		if(pi.er != none) {
			return pi;
		}

	} else {
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}

	if(checkLater == 1) {
		if(strcmp(tType.lx, "-") == 0) {
			fprintf(outFile, "neg\n");
		} else {
			fprintf(outFile, "not\n");
		}
	}

	ParserInfo pi;
	pi.tk = tkn;
	pi.er = none;
	return pi;
}

ParserInfo Operand() {
	tkn = GetNextToken();

	if(tkn.tp == INT) {

		fprintf(outFile, "push constant %s\n", tkn.lx);

		// If token is INT, return no error
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = none;
		return pi;
	} else if(tkn.tp == ID) {
		int altPush = 0;
		// If token is ID
		Token tType = tkn;
		Token tType2;
		int useClassName = 0;

		tkn = PeekNextToken();
		// Check if token is a dot
		if(tkn.tp == SYMBOL && strcmp(tkn.lx, ".") == 0) {

			Symbol searchSym;
			strcpy(searchSym.id, tType.lx);
			strcpy(searchSym.classID, GetClass());
			Symbol pushSym = GetSymbol(searchSym);

			if(pushSym.isError == 0 && pushSym.k == VAR) {
				fprintf(outFile, "push local %i\n", pushSym.kindCount);
				if (expCount == 0) {
					expCount++;
				}
				useClassName = 1;
			}

			tkn = GetNextToken();
			tkn = GetNextToken();
			if(tkn.tp != ID) {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = idExpected;
				return pi;
			}

			tType2 = tkn;

			Symbol s;
			strcpy(s.id, tkn.lx);
			strcpy(s.classID, tType.lx);
			s.tk = tkn;
			s.k = NONE;
			InsertUnknown(s);

			if(useClassName == 1) {
				for(int i=0; i<128; i++) {
					tType.lx[i] = '\0';
				}

				strcpy(tType.lx, pushSym.tp);
			}

			tkn = PeekNextToken();
		} else {
			// Check method exists;
			Symbol s;
			strcpy(s.id, tType.lx);
			strcpy(s.classID, GetClass());

			if(LookUp(s) == 0) {
				s.tk = tType;
				s.k = 4;
				InsertUnknown(s);
			}

			Token testToken = PeekNextToken();
			if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "[") == 0)) {
				Symbol pushSym = GetSymbol(s);

				if(pushSym.k == 1) {
					fprintf(outFile, "push static %i\n", pushSym.kindCount);
				} else if(pushSym.k == 2) {
					fprintf(outFile, "push this %i\n", pushSym.kindCount);
				} else if(pushSym.k == 3) {
					fprintf(outFile, "push argument %i\n", pushSym.kindCount);
				} else if(pushSym.k == 4) {
					fprintf(outFile, "push local %i\n", pushSym.kindCount);
				}
			}
		}

		// Check token is open bracket or open parenthesis
		if(tkn.tp == SYMBOL && strcmp(tkn.lx, "[") == 0) {
			pushThat = 1;
			altPush = 1;
			tkn = GetNextToken();
			tkn = PeekNextToken();
			// Check if token is SYMBOL, INT, ID, RESWORD or STRING
			if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
			tkn.tp == STRING) {
				ParserInfo pi = Expression();
				if(pi.er != none) {
					return pi;
				}
			} else {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = syntaxError;
				return pi;
			}

			tkn = GetNextToken();
			// Check token is closed bracket
			if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, "]") == 0)) {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = closeBracketExpected;
				return pi;
			}

			Symbol s;
			strcpy(s.id, tType.lx);
			strcpy(s.classID, GetClass());

			Symbol pushSym = GetSymbol(s);
			if(pushSym.k == 1) {
				fprintf(outFile, "push static %i\n", pushSym.kindCount);
			} else if(pushSym.k == 2) {
				fprintf(outFile, "push this %i\n", pushSym.kindCount);
			} else if(pushSym.k == 3) {
				fprintf(outFile, "push argument %i\n", pushSym.kindCount);
			} else if(pushSym.k == 4) {
				fprintf(outFile, "push local %i\n", pushSym.kindCount);
			}

			fprintf(outFile, "add\n");

			if(pushThat == 1) {
				pushThat = 0;
				fprintf(outFile, "pop pointer 1\n");
				fprintf(outFile, "push that 0\n");
			}

		} else if(tkn.tp == SYMBOL && strcmp(tkn.lx, "(") == 0) {
			tkn = GetNextToken();
			tkn = PeekNextToken();
			// Check if token is SYMBOL, INT, ID, RESWORD or STRING
			if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
			tkn.tp == STRING) {
				ParserInfo pi = ExpressionList();
				if(pi.er != none) {
					return pi;
				}
			} else {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = syntaxError;
				return pi;
			}

			Symbol searchSym;
			strcpy(searchSym.id, tType.lx);
			strcpy(searchSym.classID, GetClass());
			Symbol pushSym = GetSymbol(searchSym);

			if(pushSym.isError == 0 && pushSym.k == ARGUMENT) {
				fprintf(outFile, "push argument %i\n", pushSym.kindCount);
				if(expCount == 0) {
					expCount++;
				}
				fprintf(outFile, "call %s.%s %i\n", GetClass(), tType2.lx, expCount);
			} else if(pushSym.isError == 0 && pushSym.k == FIELD){
				fprintf(outFile, "push this %i\n", pushSym.kindCount);
				if(expCount == 0) {
					expCount++;
				}
				fprintf(outFile, "call %s.%s %i\n", pushSym.tp, tType2.lx, expCount);
			} else {
				fprintf(outFile, "call %s.%s %i\n", tType.lx, tType2.lx, expCount);
			}
			expCount = 0;


			tkn = GetNextToken();
			// Check token is closed bracket
			if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0)) {
				ParserInfo pi;
				pi.tk = tkn;
				pi.er = closeParenExpected;
				return pi;
			}
		}

		ParserInfo pi;
		pi.tk = tkn;
		pi.er = none;
		return pi;
	} else if(tkn.tp == SYMBOL && strcmp(tkn.lx, "(") == 0) {
		// If token is open parenthesis
		tkn = PeekNextToken();
		// Check if token is SYMBOL, INT, ID, RESWORD or STRING
		if(tkn.tp == SYMBOL || tkn.tp == INT || tkn.tp == ID || tkn.tp == RESWORD ||
		tkn.tp == STRING) {
			ParserInfo pi = Expression();
			if(pi.er != none) {
				return pi;
			}
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = syntaxError;
			return pi;
		}

		tkn = GetNextToken();
		// Check token is closed bracket
		if(!(tkn.tp == SYMBOL && strcmp(tkn.lx, ")") == 0)) {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = closeParenExpected;
			GetNextToken();	// KEEP
			return pi;
		} else {
			ParserInfo pi;
			pi.tk = tkn;
			pi.er = none;
			return pi;
		}
	} else if(tkn.tp == STRING) {
		fprintf(outFile, "push constant %i\n", strlen(tkn.lx) - 2);
		fprintf(outFile, "call String.new 1\n");
		for(int i=1; i<strlen(tkn.lx) - 1; i++) {
			fprintf(outFile, "push constant %d\n", tkn.lx[i]);
			fprintf(outFile, "call String.appendChar 2\n");
		}

		// If token is STRING
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = none;
		return pi;
	} else if(tkn.tp == RESWORD && (strcmp(tkn.lx, "true") == 0 ||
	strcmp(tkn.lx, "false") == 0 || strcmp(tkn.lx, "null") == 0 ||
	strcmp(tkn.lx, "this") == 0)) {
		// If token is RESWORD and says either "true", "false" "null" or "this"
		if(strcmp(tkn.lx, "this") == 0) {
			fprintf(outFile, "push pointer 0\n");
		} else if(strcmp(tkn.lx, "true") == 0) {
			fprintf(outFile, "push constant 0\n");
			fprintf(outFile, "not\n");
		} else if(strcmp(tkn.lx, "false") == 0) {
			fprintf(outFile, "push constant 0\n");
		} else if(strcmp(tkn.lx, "null") == 0) {
			fprintf(outFile, "push constant 0\n");
		}

		ParserInfo pi;
		pi.tk = tkn;
		pi.er = none;
		return pi;
	} else {
		// Token is none of the above, return error
		ParserInfo pi;
		pi.tk = tkn;
		pi.er = syntaxError;
		return pi;
	}
}

int InitParser (char* file_name)
{
	int status = InitLexer(file_name);
	if(status == 0) {
		return status;
	} else {
		// Go through file_name and remove the .jack extension
		int outFileLen = 0;
		for(int i=0; i<strlen(file_name); i++) {
			if(file_name[i] == '.') {
				outFileLen = i;
			}
		}

		for(int i=0; i<outFileLen+1; i++) {
			outFileName[i] = file_name[i];
		}

		strcat(outFileName, "vm");

		outFile = fopen(outFileName, "w");

		if(outFile == NULL) {
			return 1;
		}
	}
	return status;
}

ParserInfo Parse ()
{
	ParserInfo pi;

	// implement the function
	pi.tk = PeekNextToken();

	if(pi.tk.tp == ERR) {
		pi.er = lexerErr;
		return pi;
	}

	while(pi.tk.tp != EOFile) {
		pi = ClassDeclar();

		if(pi.er != none) {
			break;
		}

		pi.tk = PeekNextToken();
	}

	return pi;
}


int StopParser ()
{
	GetNextToken();
	GetNextToken();

	int status = StopLexer();

	if(outFile != NULL) {
    fclose(outFile);
    outFile = NULL;
  }

	for(int i=0; i<128; i++) {
		outFileName[i] = '\0';
	}

	pushThat = 0;
	tempCount = 0;

	return status;
}

#ifndef TEST_PARSER
/*
int main ()
{
	return 1;
}
*/
#endif
