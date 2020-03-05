#ifndef _LEXICAL_ANALYSIS_H_
#define _LEXICAL_ANALYSIS_H_
#include <cstring>
#include <map>
#include <iostream>
#include "Globals.h"
using namespace std;

TokenType look_up(string s);
char getnextchar();
void back();
string char_to_string(char c);
void printToken( TokenType token, string tokenString );
TokenType getToken();
int Lexical(string path3);

#endif