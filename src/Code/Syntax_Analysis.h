#ifndef _SYNTAX_ANALYSIS_H_
#define _SYNTAX_ANALYSIS_H_

#include<iostream>
#include<cstring>
#include "Globals.h"
using namespace std;

TreeNode * newNode(NodeKind kind);
TreeNode* Syntax_parse();
TokenType read_token();
#endif