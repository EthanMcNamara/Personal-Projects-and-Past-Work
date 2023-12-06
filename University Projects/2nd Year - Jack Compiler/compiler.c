/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Ethan McNamara
Student ID: 201419445 / sc20e2m
Email: sc20e2m@leeds.ac.uk
Date Work Commenced: 30/04/22
*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "compiler.h"
#include "symbols.h"


int InitCompiler ()
{
	InitTables();

	// Read in all built-in libraries
	InitParser("Array.jack");
	Parse();
	StopParser();

	InitParser("Keyboard.jack");
	Parse();
	StopParser();

	InitParser("Math.jack");
	Parse();
	StopParser();

	InitParser("Memory.jack");
	Parse();
	StopParser();

	InitParser("Output.jack");
	ParserInfo p = Parse();
	StopParser();

	InitParser("Screen.jack");
	Parse();
	StopParser();

	InitParser("String.jack");
	Parse();
	StopParser();

	InitParser("Sys.jack");
	Parse();
	StopParser();

	return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;
	// write your code below
	int dirLength = strlen(dir_name);
	char filePath[128] = "";

	// Finds all files ending in '.jack'
	char *isJack;
	DIR *d;
	struct dirent *dir;
	d = opendir(dir_name);
	if(d) {
		while((dir = readdir(d)) != NULL) {
			isJack = strstr(dir->d_name, ".jack");
			if(isJack) {
				// Get the correct File Path
				int fileLength = strlen(dir->d_name);
				int totalLength = dirLength + fileLength;
				strcat(filePath, dir_name);
				filePath[dirLength] = '/';
				strcat(filePath, dir->d_name);

				InitParser(filePath);
				p = Parse(dir->d_name);

				for(int i=0; i<128; i++) {
					filePath[i] = '\0';
				}

				if(p.er != none) {
					StopParser();
					return p;
				}

				StopParser();
			}
		}
		closedir(d);
	}

	Symbol s = CheckUnknown();
	if(s.isError == 1) {
		StopParser();

		ParserInfo pi;
		pi.er = undecIdentifier;
		pi.tk = s.tk;
		return pi;
	}


	p.er = none;
	StopParser();
	return p;
}

int StopCompiler ()
{
	StopTables();

	return 1;
}


int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("Pong");
	StopCompiler ();
	return 1;
}
